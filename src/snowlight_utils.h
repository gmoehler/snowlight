#ifndef LEDPOI_UTILS_H
#define LEDPOI_UTILS_H

#include "LightCommand.h"
#include "snowlight.h"

extern xQueueHandle lightQueue;
extern xQueueHandle stepperQueue;

enum QueueType {
    LIGHT_QUEUE,
    STEPPER_QUEUE,
};

// utils and demo setup

bool sendToQueue(QueueType q, LightCommand& cmd, const char* logtag,
                 bool debugPrintout = true);
bool sendToLight(LightCommand& cmd, const char* logtag,
                 bool debugPrintout = true);
bool sendRawToLight(RawCommand rawCmd, const char* logtag,
                    bool debugPrintout = true);
void clearQueue(QueueType q);
uint8_t getQueueSize(QueueType q);

#endif