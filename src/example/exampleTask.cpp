#include "exampleTask.h"

TaskHandle_t exampleTaskHandle = NULL;
Example exampleToRun = ONOFF_EXAMPLE;

const uint8_t DEMO_BRIGHTNESS = 32;  // Limits brightness

void sendCommandsTo(xQueueHandle queue, RawCommand rawCmd[], uint8_t numCmds) {
    for (int i = 0; i < numCmds; i++) {
        LightCommand cmd(rawCmd[i]);
        LOGD(EXAMPL, "Sending cmd:   %s", cmd.toString().c_str());
        if (xQueueSendToBack(queue, &(rawCmd[i]), portMAX_DELAY) != pdTRUE) {
            LOGE(EXAMPL, "Could not add command %d to playerQueue.", i);
        }
    }
}

void sendCommandTo(xQueueHandle queue, RawCommand rawCmd) {
    LightCommand cmd(rawCmd);
    LOGD(EXAMPL, "Sending cmd:   %s", cmd.toString().c_str());
    if (xQueueSendToBack(queue, &(rawCmd), portMAX_DELAY) != pdTRUE) {
        LOGE(EXAMPL, "Could not add single command to playerQueue.");
    }
}

static void printLedQueue() {
    uint8_t numLedMsg = getQueueSize(LED_QUEUE);
    LOGI(EXAMPL, "Queues: LED:%d", numLedMsg);
}

static void onoffExampleStart() {
    for (int i = 0; i < 10; i++) {
        printLedQueue();
        LOGD(EXAMPL, "switching lights off");
        sendRawToLed({LIGHT_OFF}, EXAMPL);  // black
        printLedQueue();
        delay(1000);

        LOGD(EXAMPL, "switching lights on");
        sendRawToLed({LIGHT_ON}, EXAMPL);  // bright
        printLedQueue();
        delay(1000);
    }
}

static void setLightExampleStart() {
    uint8_t num_steps = 20;
    float step = 255 / num_steps;

    for (int j = 0; j <= num_steps; j++) {
        uint8_t val = step * j;
        sendRawToLed({LIGHT_SET, val}, EXAMPL);  // dim
        delay(500);
    }
}

static void dimLightExampleStart() {
    uint8_t num_steps = 20;
    uint8_t step = 255 / num_steps;

    sendRawToLed({LIGHT_OFF}, EXAMPL);  // black

    for (int j = 0; j <= num_steps; j++) {
        sendRawToLed({LIGHT_BRIGHTEN, step}, EXAMPL);  // dim
        delay(500);
    }
    for (int j = 0; j <= num_steps; j++) {
        sendRawToLed({LIGHT_DIM, step}, EXAMPL);  // dim
        delay(500);
    }
}

static void exampleTask(void* arg) {
    switch (exampleToRun) {
        case ONOFF_EXAMPLE:
            LOGI(EXAMPL, "Starting on/off example");
            onoffExampleStart();
            break;

        case SET_EXAMPLE:
            LOGI(EXAMPL, "Starting set example");
            setLightExampleStart();
            break;

        case DIM_EXAMPLE:
            LOGI(EXAMPL, "Starting dim example");
            dimLightExampleStart();
            break;

        default:
            LOGE(EXAMPL, "No example to start");
            break;
    }

    // just filling queues then end
    vTaskDelete(exampleTaskHandle);
}

// important: only start one example at a time,
// since task is deleted after example has run
void example_start(Example example, uint8_t prio) {
    LOGI(EXAMPL, "Starting example");
    exampleToRun = example;
    xTaskCreate(exampleTask, "exampleTask", 4096, NULL, prio,
                &exampleTaskHandle);
}
