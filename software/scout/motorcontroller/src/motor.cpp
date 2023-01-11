#include "motor.h"

/**
 * INIT Motor Pins
 */
void motorInit() {
    ;           // TODO: PWM init
} /**/


/**
 * Detect motor command
 * @see Command:   motor <Motor> [direction] [DutyCycle]
 *                      Motor     (0..6)    0 means all motors (default: 0)
 *                      Direction (0..1)    0: Forward, 1: Backward.  Default: 0,forward
 *                      DutyCycle (0..100)  Motor duty cycle (0: OFF, 100: Full throttle), default: 0
 */
void motorCommand(char *command) {
    byte motor = 0;
    byte direction = 0;
    byte dutyCycle = 0;
    const char delimiter[] = " ";
    char *token = strtok(command, delimiter);
    if (token != NULL) {                    // Command
        token = strtok(NULL, delimiter);    // Motor
        motor = atoi(token);
        if (motor > 6) {
            motor = 0;
        }
        token = strtok(NULL, delimiter);    // Direction
        direction = atoi(token);
        if (direction > 1) {
            direction = 0;
        }
        token = strtok(NULL, delimiter);    // Duty Cycle
        dutyCycle = atoi(token);
        if (dutyCycle > 100) {
            dutyCycle = 100;
        }
        Serial.println(FPrintCMD(CMD_OK));
        // Do something with [motor] and [dutycycle]
        // TODO:
        Serial.print("Motor: ");
        Serial.print(motor);
        Serial.print(" (");
        Serial.print(direction);
        Serial.print(") -> ");
        Serial.println(dutyCycle);
    }
} /**/
