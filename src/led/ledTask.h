#ifndef LED_TASK
#define LED_TASK

#include "LightCommand.h"
#include "snowlight.h"

extern xQueueHandle ledQueue;  // for reading only

void led_setup(uint8_t queueSize);
void led_start(uint8_t prio);

#endif