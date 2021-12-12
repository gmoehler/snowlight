
#include "example/exampleTask.h"
#include "led/ledTask.h"
#include "snowlight.h"
#include "snowlight_utils.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include "stepper/stepperTask.h"
#include "tof/tofTask.h"

// unless defined differently below this is the default log level
#define DEFAULT_LOG_LEVEL ESP_LOG_DEBUG

void logging_setup() {
    Serial.begin(115200);
    esp_log_level_set(MAIN, DEFAULT_LOG_LEVEL);      // main program task
    esp_log_level_set(LIGHTCMD, DEFAULT_LOG_LEVEL);  // light command task
    esp_log_level_set(LEDS, DEFAULT_LOG_LEVEL);      // led task
    esp_log_level_set(TOFS, DEFAULT_LOG_LEVEL);      // tof task
    esp_log_level_set(GEST, ESP_LOG_INFO);           // gestures
    esp_log_level_set(STEPPER, DEFAULT_LOG_LEVEL);   // stepper task
    esp_log_level_set(BUTTON, DEFAULT_LOG_LEVEL);    // button task
    esp_log_level_set(EXAMPL, DEFAULT_LOG_LEVEL);    // example task
}

void disableBrownOut() { WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); }

void setup() {
    bool setupOk = true;

    // disableBrownOut();
    logging_setup();

    // setup tasks and queues with sizes
    tof_setup();

    // button_setup();

    bool ledOk = led_setup(10);
    setupOk = setupOk && ledOk;

    // stepper_setup(10);

    if (!setupOk) {
        LOGE(MAIN, "Setup failed. Not starting tasks.");
        return;
    }

    // start tasks with prios
    // button_start(8);
    led_start(7);

    // stepper_start(7);
    tof_start(7);

    // now waiting for interaction

    // start example
    // LOGD(MAIN, "Starting example...");
    // example_start(DIM_EXAMPLE, 5);
}

// everything works with tasks, we dont need the loop...
void loop() {
    delay(100000);  // snow white sleep
}