/*
 * LegendI2C.h
 *
 *  Created on: Dec 20, 2018
 *      Author: AQaut
 */

#ifndef LEGENDI2C_H_
#define LEGENDI2C_H_

#include <msp430.h>

#define I2C_CHECKSUM 'A'

typedef struct I2Cmessage {
    unsigned char checksum;
    unsigned char data;
} message_t;


volatile unsigned char RXData;
volatile message_t incoming;

void setupI2C(void);
void processIncomingByte(unsigned char byte);



#endif /* LEGENDI2C_H_ */
