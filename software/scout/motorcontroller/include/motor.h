#ifndef __MOTOR_H__
#define __MOTOR_H__
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include "defines.h"


// External includes
extern Adafruit_PWMServoDriver pwm;

// Local functions
void motorInit();
void motorCommand(char *command);
void motorAction(byte motorNumber, byte direction, unsigned int dutyCycle);


#endif
