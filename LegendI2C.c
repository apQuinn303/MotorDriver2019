/*
 * LegendI2C.c
 *
 *  Created on: Dec 20, 2018
 *      Author: AQaut
 */

#include <stdio.h>
#include "msp430.h"
#include "LegendI2C.h"
#include "LegendPWM.h"

#pragma vector = EUSCI_B2_VECTOR
__interrupt void USCI_B2_ISR(void)
{
    switch(UCB2IV)
    {
    case USCI_I2C_UCRXIFG0:             // If we received sth.
        RXData = UCB2RXBUF;             // Get RX data
        processIncomingByte(RXData);
        break;
    default:
        return;
    }
}

void setupI2C(void)
{
    //Make sure high-impedance mode is disabled.
    PM5CTL0 &= ~LOCKLPM5;

    //Activate the I2C mode on P7.0 and P7.1
    P7SEL0 |= BIT0 | BIT1;
    P7SEL1 &= ~(BIT0 | BIT1);

    RXData = 0; //Zero our 1-byte buffer.
    incoming.checksum = 0; //Zero our incoming message buffer.
    incoming.data = 0;

    // Configure USCI_B2 for I2C mode
    //Note: b/c the master provides the clock, we don't have to configure a clock source.

    UCB2CTLW0 = UCSWRST;                    // Assert SW reset.
    UCB2CTLW0 |= UCMODE_3 | UCSYNC;         // I2C mode, sync mode
    UCB2CTLW0 &= ~UCMST;                    // NOT master mode (slave mode)
    UCB2I2COA0 = 0x48 | UCOAEN;             // own address is 0x48 + enable
    UCB2CTLW0 &= ~UCSWRST;                  // Deassert SW reset (release the peripheral)
    UCB2IE |= UCRXIE0;                      // Enable Rx interrupt.
}


void processIncomingByte(unsigned char byte)
{
    if(incoming.checksum == 0)
    {
        incoming.checksum = byte;
        return;
    }
    if(incoming.data == 0)
    {
        incoming.data = byte;

        if(incoming.checksum == I2C_CHECKSUM)
        {
            /*Process data*/
            setPhaseA((int)incoming.data);
            setPhaseB((int)incoming.data);
            setPhaseC((int)incoming.data);
        }

        incoming.data = 0;
        incoming.checksum = 0;
        return;
    }
}
