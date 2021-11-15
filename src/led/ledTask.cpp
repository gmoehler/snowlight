#include "ledTask.h"

xQueueHandle ledQueue = NULL;

void ledTask(void* arg) {
    RawCommand rawCmd;

    for (;;) {
        // grab next command
        if (xQueueReceive(ledQueue, &(rawCmd), portMAX_DELAY)) {
            LightCommand cmd(rawCmd);
            LOGD(LEDS, "Receiving cmd: %s", cmd.toString().c_str());

            // TODO: do what needs to be done
        }
    }
}

void led_setup(uint8_t queueSize) {
    ledQueue = xQueueCreate(queueSize, sizeof(RawCommand));
}

void led_start(uint8_t prio) {
    xTaskCreate(ledTask, "ledTask", 4096, NULL, prio, NULL);
}
