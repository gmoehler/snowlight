#ifndef STEPPER_TASK
#define STEPPER_TASK

#include "LightCommand.h"
#include "snowlight.h"

extern xQueueHandle stepperQueue;  // for reading only

void stepper_setup(uint8_t queueSize);
void stepper_start(uint8_t prio);

#endif