#include "common.h"


void dumpString(char *p) {
    for (byte i=0; i<strlen(p); i++) {
        Serial.print("0x");
        Serial.print(p[i] < 16 ? "0" : "");
        Serial.print(p[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");
} /**/
