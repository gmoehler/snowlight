#include "snowlight_utils.h"

const char* _getQueueName(QueueType q) {
    switch (q) {
        case LED_QUEUE:
            return "ledQueue";

        case STEPPER_QUEUE:
            return "stepperQueue";
    }
    return "ledQueue";
}

xQueueHandle _getQueue(QueueType q) {
    switch (q) {
        case LED_QUEUE:
            return ledQueue;

        case STEPPER_QUEUE:
            return stepperQueue;
    }

    return ledQueue;
}

bool sendToQueue(QueueType q, LightCommand& cmd, const char* logtag,
                 bool debugPrintout) {
    if (debugPrintout) {
        LOGD(logtag, "Sending to queue %s:  %s", _getQueueName(q),
             cmd.toString().c_str());
    } else {
        LOGV(logtag, "Sending to queue %s:  %s", _getQueueName(q),
             cmd.toString().c_str());
    }

    xQueueHandle toQueue = _getQueue(q);
    RawCommand rawCmd = cmd.getRawCommand();

    if (xQueueSendToBack(toQueue, &(rawCmd), portMAX_DELAY) != pdTRUE) {
        LOGE(logtag, "Could not add  command to queue %s.", _getQueueName(q));
        return false;
    }
    return true;
}

bool sendToLed(LightCommand& cmd, const char* logtag, bool debugPrintout) {
    return sendToQueue(LED_QUEUE, cmd, logtag, debugPrintout);
}

bool sendRawToLed(RawCommand rawCmd, const char* logtag, bool debugPrintout) {
    LightCommand cmd(rawCmd);
    return sendToQueue(LED_QUEUE, cmd, logtag, debugPrintout);
}

void clearQueue(QueueType q) {
    xQueueHandle queue = _getQueue(q);
    xQueueReset(queue);
}

uint8_t getQueueSize(QueueType q) {
    xQueueHandle queue = _getQueue(q);
    return uxQueueMessagesWaiting(queue);
}
