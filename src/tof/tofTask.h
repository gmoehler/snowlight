#ifndef TOF_H
#define TOF_H

#include "Adafruit_VL53L0X.h"
#include "LightCommand.h"
#include "snowlight.h"
#include "snowlight_utils.h"

// extern xQueueHandle dispatchQueue;  // for interrupt send

void tof_setup();
void tof_start(uint8_t prio);

#endif