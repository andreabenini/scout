#ifndef __COMMON_H__
#define __COMMON_H__
#include <Arduino.h>

#include "defines.h"


#define ledError()      RXLED0;TXLED0;delay(300);RXLED1;TXLED1;delay(300);

// void ledError();
void dumpString(char *p);

#endif
