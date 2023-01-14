#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include <Arduino.h>
#include <string.h>

#include "defines.h"
#include "common.h"


// External includes
extern void serialEvent();
extern void serialClear();

// Local functions
void serialClear();
void serialConnect();
void serialGetCommand();


#endif
