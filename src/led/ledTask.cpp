#include "ledTask.h"

#include <PCA9622PWM.h>

xQueueHandle ledQueue = NULL;
#define DEFAULT_I2C_ADDR_9622 0x70  // At your preference (of your h/w setting)

TwoWire ledBus = TwoWire(1);
PCA9622PWM lightPwm(DEFAULT_I2C_ADDR_9622, &ledBus);

uint8_t nPorts = 0;
uint8_t* pwmData;     // array of nPorts length for I2C communication
uint8_t* curPwmData;  // array of nPorts length for current state of leds

void dimTo(uint8_t val) {
    LOGD(LEDS, "Dimming to %.1d.", val);
    uint8_t valNorm = std::min(std::max(val, (uint8_t)0), (uint8_t)255);
    lightPwm.exponential_adjustment(true);

    for (int i = 0; i < nPorts; i++) {
        pwmData[i] = valNorm;
        curPwmData[i] = valNorm;
    }
    lightPwm.pwm2(pwmData);
}

void dimBy(uint8_t val, bool increase) {
    LOGD(LEDS, "%s by %s%.1d.", increase ? "brighten" : "dim",
         increase ? "" : "-", val);
    lightPwm.exponential_adjustment(true);

    for (int i = 0; i < nPorts; i++) {
        // use integer to not go accross uint8_t boundaries
        int newVal = curPwmData[i] + (increase ? val : -val);
        uint8_t newValNorm = std::min(std::max(newVal, 0), 255);
        pwmData[i] = newValNorm;
        curPwmData[i] = newValNorm;
    }
    lightPwm.pwm2(pwmData);
}

void toggle() {
    LOGD(LEDS, "toggle");
    lightPwm.exponential_adjustment(true);

    for (int i = 0; i < nPorts; i++) {
        uint8_t newVal = curPwmData[i] > 0 ? 0 : 255;
        pwmData[i] = newVal;
        curPwmData[i] = newVal;
    }
    lightPwm.pwm2(pwmData);
}

void testGradient() {
    uint8_t num_steps = 20;
    float step = 1.0 / num_steps;
    lightPwm.exponential_adjustment(true);
    for (int j = 1; j <= num_steps; j++) {
        float ratio = step * j;
        LOGD(LEDS, "Dimming %d ratio: %.2f", j, ratio);
        dimTo(ratio);
        delay(100);
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

void ledTask(void* arg) {
    RawCommand rawCmd;

    for (;;) {
        // grab next command
        if (xQueueReceive(ledQueue, &(rawCmd), portMAX_DELAY)) {
            LightCommand cmd(rawCmd);
            LOGD(LEDS, "Receiving cmd: %s", cmd.toString().c_str());

            if (cmd.getType() == LIGHT_ON) {
                dimTo(255);
            } else if (cmd.getType() == LIGHT_OFF) {
                dimTo(0);
            } else if (cmd.getType() == LIGHT_TOGGLE) {
                toggle();
            } else if (cmd.getType() == LIGHT_SET) {
                dimTo(cmd.getField(1));
            } else if (cmd.getType() == LIGHT_BRIGHTEN) {
                dimBy(cmd.getField(1), true);
            } else if (cmd.getType() == LIGHT_DIM) {
                dimBy(cmd.getField(1), false);
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
            LOGE(
                LEDS,
                "Device does not appear to be connected. Please check wiring.");
            scanDevice();
            return false;
        }
    }

    nPorts = lightPwm.number_of_ports();
    pwmData = (uint8_t*)malloc(nPorts * sizeof(uint8_t*));
    curPwmData = (uint8_t*)malloc(nPorts * sizeof(uint8_t*));

    LOGD(LEDS, "Number of LED ports: %d", nPorts);
    return true;
}

void led_start(uint8_t prio) {
    // testGradient();
    xTaskCreate(ledTask, "ledTask", 4096, NULL, prio, NULL);
}
