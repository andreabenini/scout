/**
 * Project general defines
 */
#ifndef __DEFINES_H__
#define __DEFINES_H__

// Serial communication
extern byte iCommand;
extern char BUFFER[200];
extern char Command[50];

// Program information
#define MAIN_LOOP_DELAY     500
#define INFO_VERSION        PSTR("scout controller v1.0")

// Commands
#define FPrint(s)           (__FlashStringHelper*)PSTR(s)
#define FPrintCMD(cmd)      (__FlashStringHelper*)cmd
#define CMD_CONNECT         PSTR("connect")                 // connect
#define CMD_OK              PSTR("ok")
#define CMD_ERROR           PSTR("error")
#define CMD_INFO            PSTR("info")                    // info
#define CMD_STOP            PSTR("stop")                    // stop
#define CMD_MOTOR           PSTR("motor")                   // motor <MotorN> <DutyCycle>

#endif
