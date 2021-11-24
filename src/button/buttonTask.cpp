#include "buttonTask.h"

// number of millisec that have to pass by before a click is assumed as safe.
int _debounceTicks = 50;
// number of millisec that have to pass by before a long button press is
// detected.
int _longTicks = 600;
// number of millisec after which we send a release
int _holdDownTicks = 2000;

volatile uint32_t _lastPressedTime = 0;
volatile SemaphoreHandle_t _longPressTimerSemaphore;

// PoiTimer longTimer(TIMER2, false);    // timer waiting for long click

// called at any edge
void IRAM_ATTR buttonIsrHandler(void* arg) {
    int level = gpio_get_level(BUTTON_ID);
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    int pressDuration = now - _lastPressedTime;

    LightCommand cmd;

    // determine type of click when button is released
    if (level == 1 && _lastPressedTime > 0) {
        if (pressDuration >= _debounceTicks && pressDuration < _longTicks) {
            cmd.setType(BUTTON0_CLICK);
        }
        // should usually be caught by onLongPressTimer
        else if (pressDuration >= _longTicks) {
            cmd.setType(BUTTON0_LONGCLICK);
        }
    }

    // remember time when button is pressed down
    else if (level == 0) {
        _lastPressedTime = now;
        // start time for long press detection
        xSemaphoreGiveFromISR(_longPressTimerSemaphore, NULL);
        // longTimer.setInterval(_longTicks);
    }

    if (!cmd.isEmpty()) {
        // clear press memory
        _lastPressedTime = 0;
        // send out event
        // xQueueSendFromISR(dispatchQueue, &cmd, NULL);
    }
}

static void buttonLongPressedTask(void* arg) {
    for (;;) {
        // wait for button being pressed
        if (xSemaphoreTake(_longPressTimerSemaphore, portMAX_DELAY)) {
            // wait long click time to see what happened
            delay(_longTicks);

            // now look whether we are still pressing the button
            int level = gpio_get_level(BUTTON_ID);
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            int pressDuration = now - _lastPressedTime;

            // still pressed: check long press
            if (level == 0 && _lastPressedTime > 0 &&
                pressDuration >= _longTicks) {
                // clear press memory
                _lastPressedTime = 0;
                // send out event
                sendRawToLed({BUTTON0_LONGCLICK}, BUTTON);
            }
        }
    }
}

void button_setup() {
    gpio_config_t io_conf;
    // interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    // bit mask of the pins, use GPIO0 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // change gpio intrrupt type for one pin
    // send interrupt for any edge
    gpio_set_intr_type(BUTTON_ID, GPIO_INTR_ANYEDGE);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(BUTTON_ID, buttonIsrHandler, (void*)GPIO_INPUT_IO_0);

    _longPressTimerSemaphore = xSemaphoreCreateBinary();
    // longTimer.init(onLongPressTimer);
}

void button_start(uint8_t prio) {
    xTaskCreate(buttonLongPressedTask, "buttonLongPressedTask", 4096, NULL,
                prio, NULL);
}
