#include "ledTask.h"

#include <PCA9622PWM.h>

xQueueHandle ledQueue = NULL;
#define DEFAULT_I2C_ADDR_9622 0x70  // At your preference (of your h/w setting)

TwoWire ledBus = TwoWire(1);
PCA9622PWM lightPwm(DEFAULT_I2C_ADDR_9622, &ledBus);

uint8_t nPorts = 0;
uint8_t* pwmData;   // array of nPorts length for I2C communication
uint8_t curPwmVal;  // current global pwm val

const uint8_t PWM_OFF = 0;   // pwm for light off
const uint8_t PWM_ON = 127;  // pwm when switched on

const uint8_t PWM_MIN = 1;    // dont dim below this val
const uint8_t PWM_MAX = 255;  // dont dim above this val

millis_t lastToggleTime = 0;
const millis_t minToggleInterval = 1000;

bool isLightOn() { return curPwmVal != PWM_OFF; }

// keep pwm value in range
uint8_t normVal(uint8_t val) {
    return std::min(std::max(val, PWM_MIN), PWM_MAX);
}

// send pwm data to all channels and set curPwmVal
void updatePwm(uint8_t val) {
    for (int i = 0; i < nPorts; i++) {
        pwmData[i] = curPwmVal;
    }
    lightPwm.pwm2(pwmData);
    curPwmVal = val;
}

void dimTo(uint8_t val) {
    LOGD(LEDS, "Dimming to %.1d.", val);
    updatePwm(normVal(val));
}

void dimBy(uint8_t val, bool increase) {
    LOGD(LEDS, "%s by %s%.1d.", increase ? "Brighten" : "Dim",
         increase ? "" : "-", val);

    // use integer to not go accross uint8_t boundaries
    int newVal = curPwmVal + (increase ? val : -val);
    updatePwm(normVal(newVal));
}

void off() {
    LOGD(LEDS, "off");
    updatePwm(PWM_OFF);
}

void on() {
    LOGD(LEDS, "on");
    updatePwm(PWM_OFF);
}

void toggle() {
    LOGD(LEDS, "toggle");
    if (isLightOn()) {
        off();
    } else {
        on();
    }
}

uint8_t scanDevice() {
    byte error, addressFound = 0;
    int nDevices;
    LOGI(LEDS, "Scanning...");
    nDevices = 0;
    // Wire.begin();
    for (byte address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            LOGI(LEDS, "I2C device found at address %#04x.", address);
            addressFound = address;
            nDevices++;
        } else if (error == 4) {
            LOGW(LEDS, "Unknow error at address %#04x.", address);
        }
    }
    if (nDevices == 0) {
        LOGI(LEDS, "No I2C devices found.");
    } else {
        LOGI(LEDS, "Scan completed.");
    }
    return addressFound;
}

// light control logic
void lightControl(LightCommand cmd) {
    LightCommandType type = cmd.getType();

    if (type == TOF_LONG_HOLD) {
        millis_t now = millis();
        // dont toggle immediately after last toggle
        if (now - lastToggleTime > minToggleInterval) {
            toggle();
            lastToggleTime = millis();
        }
    } else if (isLightOn() && (type == TOF_UP || type == TOF_DOWN)) {
        dimBy(cmd.getField(1), type == TOF_UP);
    }
}

void ledTask(void* arg) {
    RawCommand rawCmd;

    for (;;) {
        // grab next command
        if (xQueueReceive(ledQueue, &(rawCmd), portMAX_DELAY)) {
            LightCommand cmd(rawCmd);
            LOGD(LEDS, "Receiving cmd: %s", cmd.toString().c_str());

            switch (cmd.getType()) {
                case LIGHT_ON:
                    dimTo(255);
                    break;
                case LIGHT_OFF:
                    dimTo(0);
                    break;
                case LIGHT_TOGGLE:
                    toggle();
                    break;
                case LIGHT_SET:
                    dimTo(cmd.getField(1));
                    break;
                case LIGHT_BRIGHTEN:
                    dimBy(cmd.getField(1), true);
                    break;
                case LIGHT_DIM:
                    dimBy(cmd.getField(1), false);
                    break;
                default:
                    lightControl(cmd);
                    break;
            }
        }
    }
}

bool led_setup(uint8_t queueSize) {
    // Wire.setClock(400000);
    ledBus.begin(16, 17, 100000);

    ledQueue = xQueueCreate(queueSize, sizeof(RawCommand));
    delay(1000);

    LOGW(LEDS, "Connecting to PCA9622 at %#04x ...", DEFAULT_I2C_ADDR_9622);
    if (lightPwm.begin() == false) {
        LOGW(LEDS, "Unable to connect to PCA9622. Attempting to reset...");
        lightPwm.reset();
        if (lightPwm.begin() == false) {
            LOGE(LEDS,
                 "Device does not appear to be connected. Please check "
                 "wiring.");
            scanDevice();
            return false;
        }
    }

    nPorts = lightPwm.number_of_ports();
    pwmData = (uint8_t*)malloc(nPorts * sizeof(uint8_t*));

    LOGD(LEDS, "Number of LED ports: %d", nPorts);

    lightPwm.exponential_adjustment(true);
    off();  // start with lights off
    return true;
}

void led_start(uint8_t prio) {
    xTaskCreate(ledTask, "ledTask", 4096, NULL, prio, NULL);
}
