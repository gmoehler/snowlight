
#include "button/buttonTask.h"
#include "led/ledTask.h"
#include "snowlight.h"
#include "snowlight_utils.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include "stepper/stepperTask.h"

// unless defined differently below this is the default log level
#define DEFAULT_LOG_LEVEL ESP_LOG_INFO

void logging_setup() {
    esp_log_level_set(MAIN, ESP_LOG_DEBUG);         // main program task
    esp_log_level_set(LIGHTCMD, ESP_LOG_DEBUG);     // light command task
    esp_log_level_set(LEDS, DEFAULT_LOG_LEVEL);     // led task
    esp_log_level_set(STEPPER, DEFAULT_LOG_LEVEL);  // stepper task
    esp_log_level_set(BUTTON, ESP_LOG_DEBUG);       // button task
}

void disableBrownOut() { WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); }

void setup1() {
    // disableBrownOut();
    logging_setup();

    // setup tasks and queues with sizes
    button_setup();
    led_setup(10);
    // stepper_setup(10);

    // start tasks with prios
    button_start(8);
    led_start(7);
    // stepper_start(7);

    sendRawToLight({OFF}, MAIN);  // black

    // waiting for button interaction
}

// everything works with tasks, we dont need the loop...
void loop1() {
    delay(100000);  // snow white sleep
}