// Libraries defines
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// Global variables
byte iCommand;
char BUFFER[200];
char Command[50];

// Project defines
#include "defines.h"
#include "common.h"
#include "motor.h"
#include "communication.h"


/**
 * Global variables
 */
Adafruit_PWMServoDriver pwm;


/**
 * setup() - Constructor
 */
void setup() {
    serialConnect();
    motorInit();
} /**/


/**
 * loop() - MAIN LOOP
 */
void loop() {
    // Command detection
    if (iCommand > 0) {
        serialGetCommand();
        if (!strncmp_P(Command, CMD_MOTOR, strlen_P(CMD_MOTOR))) {
            motorCommand(Command);
        } else if (!strcmp_P(Command, CMD_INFO)) {
            Serial.println(FPrintCMD(INFO_VERSION));
        } else {
            Serial.print(FPrintCMD(CMD_ERROR));
            Serial.print(FPrint(" command: "));
            Serial.println(Command);
        }
    }
    delay(MAIN_LOOP_DELAY);
} /**/


/**
 * Generic serial event function, all results ALWAYS in [BUFFER], use [isBuffer] to check if it's complete
 */
void serialEvent() {
    while (Serial.available()) {
        int  iLen  = strlen(BUFFER);
        char iChar = (char)Serial.read();
        BUFFER[iLen] = iChar;
        BUFFER[iLen+1] = '\0';
        if (iChar == '\n') {
            iCommand++;
        }
    }
} /**/
/** NOTE: ProMicro does NOT use standard 'serialEvent()' but 'serialEventRun()' instead, adding both for compatibility */
void serialEventRun() { serialEvent(); }
