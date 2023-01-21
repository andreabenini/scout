#include "motor.h"

/**
 * INIT Motor Pins
 */
void motorInit() {
    Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();        // (const uint8_t addr)
    pwm.begin();
    /**
     * In theory the internal oscillator (clock) is 25MHz but it really isn't that precise. 
     * You can 'calibrate' this by tweaking this number until you get the PWM update frequency you're expecting !
     * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and is used for calculating things like writeMicroseconds()
     * Analog servos run at ~50 Hz updates, It is importaint to use an oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
     *     1) Attach the oscilloscope to one of the PWM signal pins and ground on the I2C PCA9685 chip you are setting the value for.
     *     2) Adjust setOscillatorFrequency() until the PWM update frequency is the expected value (50Hz for most ESCs)
     * Setting the value here is specific to each individual I2C PCA9685 chip and affects the calculations for the PWM update frequency. 
     * Failure to correctly set the int.osc value will cause unexpected PWM results
     */
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(1600);                   // Max PWM frequency
    /**
     * if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode.
     * Some i2c devices dont like this so much so if you're sharing the bus, watch out for this!
     */
    Wire.setClock(400000);
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
    unsigned int dutyCycle = 0;             // FIXME: Need to be switched back to byte
    const char delimiter[] = " ";
    char *token = strtok(command, delimiter);
    if (token != NULL) {                    // Command
        token = strtok(NULL, delimiter);    // Motor
        motor = atoi(token);
        if (motor > MOTOR_NUMBER) {
            motor = 0;
        }
        token = strtok(NULL, delimiter);    // Direction
        direction = atoi(token);
        if (direction > 1) {
            direction = 0;
        }
        token = strtok(NULL, delimiter);    // Duty Cycle
        dutyCycle = atoi(token);
        // if (dutyCycle > 100) {           // FIXME: Temporarily removing duty cycle controls
        //     dutyCycle = 100;
        // }
        Serial.println(FPrintCMD(CMD_OK));
        // Do something with [motor] and [dutycycle]
        if (motor == 0) {
            for (motor=1; motor<=MOTOR_NUMBER; motor++) {
                motorAction(motor, direction, dutyCycle);
            }
        } else {
            motorAction(motor, direction, dutyCycle);
        }
    }
} /**/


void motorAction(byte motorNumber, byte direction, byte dutyCycle) {
    // DEBUG:
    Serial.print("Motor: ");
    Serial.print(motorNumber);
    Serial.print(" (");
    Serial.print(direction);
    Serial.print(") -> ");
    Serial.println(dutyCycle);
    // DEBUG: End
    /*
     *   @param  num One of the PWM output pins, from 0 to 15
     *   @param  val The number of ticks out of 4096 to be active, should be a value from 0 to 4095 inclusive.
     *   @param  invert If true, inverts the output, defaults to 'false'
     */
    // pwm.setPin(motorNumber-1, 4096*dutyCycle/100, direction==0? true: false);    // FIXME: Use this one with the 0..100% scale
    // DEBUG: PWM debug with direct commands to PWMServoDriver
    pwm.setPin(motorNumber-1, dutyCycle, direction==0? true: false);
    
    // pwm.setPWM(motorNumber, 0, (i + (4096/16)*motorNumber) % 4096 );
    // setPin(uint8_t num, uint16_t val, bool invert)
}