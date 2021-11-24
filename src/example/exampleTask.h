#ifndef EXAMPLE_TASK_H
#define EXAMPLE_TASK_H

#include "LightCommand.h"
#include "led/ledTask.h"
#include "snowlight.h"
#include "snowlight_utils.h"

enum Example { ONOFF_EXAMPLE, SET_EXAMPLE, DIM_EXAMPLE };

// start example task
void example_start(Example example, uint8_t prio);

#endif