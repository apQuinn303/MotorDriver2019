/*
 * LegendI2C.h
 *
 *  Created on: Dec 20, 2018
 *      Author: AQaut
 */

#ifndef LEGENDI2C_H_
#define LEGENDI2C_H_

#include <msp430.h>
#include "MDtypes.h"

#define SELF_I2C_ADDRESS 0x48

#define I2C_CHECKSUM 'A'
#define PARITY 0x1
#define MD_OK 0x2
#define MD_NOK 0x4
#define SHUTDOWN 0x8
#define STARTUP 0x10
#define ERROR 0x20
#define CCW 0x40

typedef struct I2Cmessage {
    unsigned char checksum;
    unsigned char data;
    unsigned char flags;
} message_t;


/**** I2C Message Specification ****
 *
 * An I2C message sent to or from the motor driver (hereafter "a message")
 * shall be three bytes long.
 *
 * The first byte will serve as a checksum.
 * The checksum will always be the ASCII character 'A'. Messages without
 * a correct checksum may be disregarded.
 *
 * The second byte will contain speed data.
 * Motor speed will be encoded as an unsigned 8 bit integer. Messages
 * sent by the motor driver will contain the actual speed. Messages sent
 * to the motor driver should contain the desired speed.
 *
 * The bits of the third byte will be flags with the following meanings:
 * bit 0 - parity
 * bit 1 - MD_OK (Asserted by Motor Driver if OK)
 * bit 2 - MD_NOK (Asserted by Motor Driver if in error state)
 * bit 3 - Request Motor Shutdown
 * bit 4 - Request Motor Startup (doesn't have to be 1 if already running)
 * bit 5 - Error Flag (indicates the last transmission was not
 *                     received properly due to an error.)
 * bit 6 - Counter-Clockwise Flag (EWISOTT)
 * bit 7 - Unused
 *
 ********
 */


volatile unsigned char errorFlag;

volatile unsigned char RXData;
volatile unsigned char TXData;

volatile unsigned char RXCounter;
volatile unsigned char TXCounter;

volatile message_t incoming;
volatile message_t outgoing;

volatile static MDstate_t* boardState;

void setupI2C(volatile MDstate_t* state);
void processIncomingByte(unsigned char byte);
unsigned char generateOutgoingByte();

unsigned char parity(unsigned char x);



#endif /* LEGENDI2C_H_ */
