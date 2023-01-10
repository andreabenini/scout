#ifndef __MOTOR_H__
#define __MOTOR_H__
#include <Arduino.h>

#include "defines.h"


void motorInit();
void motorCommand(char *command);

#endif
