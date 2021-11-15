#include "stepperTask.h"

xQueueHandle stepperQueue = NULL;

void stepperTask(void* arg) {
    RawCommand rawCmd;

    for (;;) {
        // grab next command
        if (xQueueReceive(stepperQueue, &(rawCmd), portMAX_DELAY)) {
            LightCommand cmd(rawCmd);
            LOGD(STEPPER, "Receiving cmd: %s", cmd.toString().c_str());

            // TODO: do what needs to be done
        }
    }
}

void stepper_setup(uint8_t queueSize) {
    stepperQueue = xQueueCreate(queueSize, sizeof(RawCommand));
}

void stepper_start(uint8_t prio) {
    xTaskCreate(stepperTask, "stepperTask", 4096, NULL, prio, NULL);
}
