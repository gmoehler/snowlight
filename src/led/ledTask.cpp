#include "ledTask.h"

#include <PCA9622PWM.h>

xQueueHandle ledQueue = NULL;
#define DEFAULT_I2C_ADDR_9622 0x70  // At your preference (of your h/w setting)
#define OE_PORT 2                   // port for PCA9622 output enablement

PCA9622PWM* lightPwm;

uint8_t nPorts = 0;
float* pwmData;  // array for I2C communication

void dimTo(float ratio) {
    LOGD(LEDS, "Dimming to %.1f.", ratio);
    float ratio2 = fmin(fmax(ratio, 0.0), 1.0);
    lightPwm->exponential_adjustment(true);
    // allocate array here, to allow async calls
    float data2send[nPorts];
    for (int i = 0; i < nPorts; i++) {
        data2send[i] = ratio2;
    }
    lightPwm->pwm(data2send);
}

void testGradient() {
    uint8_t num_steps = 20;
    float step = 1.0 / num_steps;
    lightPwm->exponential_adjustment(true);
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
    Wire.begin();
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
                dimTo(1.0);
            } else if (cmd.getType() == LIGHT_OFF) {
                dimTo(0.0);
            } else if (cmd.getType() == LIGHT_DIM) {
                uint8_t valRaw = cmd.getField(1);
                float val = (float)cmd.getField(1) / 255;
                LOGD(LEDS, "Dim: %d -> %.2f", valRaw, val);
                dimTo(val);
            }
        }
    }
}

bool led_setup(uint8_t queueSize) {
    ledQueue = xQueueCreate(queueSize, sizeof(RawCommand));
    delay(1000);

    // uint8_t port = scanDevice();
    LOGW(LEDS, "Connecting to PCA9622 at %#04x ...", DEFAULT_I2C_ADDR_9622);
    lightPwm = new PCA9622PWM(DEFAULT_I2C_ADDR_9622);
    if (lightPwm->begin() == false) {
        LOGW(LEDS, "Unable to connect to PCA9622. Attempting to reset...");
        lightPwm->reset();
        if (lightPwm->begin() == false) {
            LOGE(
                LEDS,
                "Device does not appear to be connected. Please check wiring.");
            scanDevice();
            return false;
        }
    }

    Wire.setClock(400000);
    nPorts = lightPwm->number_of_ports();
    pwmData = (float*)malloc(nPorts * sizeof(float*));

    LOGD(LEDS, "Number of LED ports: %d", nPorts);
    return true;
}

void led_start(uint8_t prio) {
    // testGradient();
    xTaskCreate(ledTask, "ledTask", 4096, NULL, prio, NULL);
}
