#include "communication.h"


void serialClear() {
    iCommand = 0;
    memset(BUFFER,  0x00, sizeof(BUFFER));
    memset(Command, 0x00, sizeof(Command));
} /**/

/**
 * Get [Command] from [BUFFER]
 */
void serialGetCommand() {
    char *Pos = strchr(BUFFER, '\n');
    if (Pos != NULL) {
        // Detect [Command] and [iCommand].   ([iCommand] is the commands count in [BUFFER])
        memcpy(Command, BUFFER, Pos-BUFFER);
        Command[Pos-BUFFER] = '\0';
        iCommand--;
        // Remove [Command] from [BUFFER]
        memmove(BUFFER, Pos+1, strlen(BUFFER)-(Pos-BUFFER)+1);
        // Remove all occurrences of \r from [Command], if any
        while ((Pos = strchr(Command, '\r')) != NULL) {
            memmove(Pos, Pos+1, strlen(Pos+1)+1);
        }
    } else {
        memset(Command, 0x00, sizeof(Command));
    }
} /**/

void serialConnect() {
    // Serial communication init
    serialClear();
    delay(500);
    Serial.begin(9600);                 // This pipes to the serial monitor
    delay(500);
    Serial.println(FPrint("Type 'connect' to start"));
    Serial.flush();
    // Waiting first handshake, remote has to start with 'ok' to connect
    byte load = 1;
    while (1) {
        // Waiting for ACK command
        serialEvent();
        if (iCommand > 0) {
            serialGetCommand();
            if (!strcmp_P(Command, PSTR("connect"))) {
                Serial.println(FPrintCMD(CMD_OK));
                return;
            } else {
                for (load=0; load<5; load++) {
                    ledError();
                }
                serialClear();
                Serial.println(FPrintCMD(CMD_ERROR));
            }
        // Waiting blink
        } else {
            if (load) {
                RXLED1;
                TXLED0;
            } else {
                RXLED0;
                TXLED1;
            }
            load = !load;
            delay(1000);
        }
    }
} /**/
