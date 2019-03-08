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
#include "Commutation.h"
#include "MDtypes.h"

#pragma vector = EUSCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
    switch(UCB0IV)
    {
    case USCI_I2C_UCSTTIFG:
        RXCounter = 0;
        TXCounter = 0;
        break;

    case USCI_I2C_UCRXIFG0:             // If we received sth.
        RXData = UCB0RXBUF;             // Get RX data
        processIncomingByte(RXData);
        break;

    case USCI_I2C_UCTXIFG0:
        UCB0TXBUF = generateOutgoingByte();
        break;

    default:
        return;
    }
}

void setupI2C(volatile MDstate_t* state)
{
    //Make sure we have a reference to the overall state struct.
    boardState = state;

    //Make sure high-impedance mode is disabled.
    PM5CTL0 &= ~LOCKLPM5;

    //Activate the I2C mode on P7.0 and P7.1
    P1SEL0 &= ~(BIT6 | BIT7);
    P1SEL1 |= BIT6 | BIT7;

    RXData = 0; //Zero our 1-byte buffers
    TXData = 0;
    RXCounter = 0;
    TXCounter = 0;

    incoming.checksum = 0; //Zero our incoming message buffer.
    incoming.data = 0;
    incoming.flags = 0;

    outgoing.checksum = 'A';
    outgoing.data = 0;
    outgoing.flags = 0;

    // Configure USCI_B2 for I2C mode
    //Note: b/c the master provides the clock, we don't have to configure a clock source.

    UCB0CTLW0 = UCSWRST;                    // Assert SW reset.
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;         // I2C mode, sync mode
    UCB0CTLW0 &= ~UCMST;                    // NOT master mode (slave mode)
    UCB0I2COA0 = SELF_I2C_ADDRESS | UCOAEN;             // own address is 0x48 + enable
    UCB0CTLW0 &= ~UCSWRST;                  // Deassert SW reset (release the peripheral)
    UCB0IE |= UCRXIE0 | UCTXIE0 | UCSTTIE;            // Enable Rx & Tx interrupt.
}


void processIncomingByte(unsigned char byte)
{
    switch(RXCounter)
    {
    case 0:
        incoming.checksum = byte;
        RXCounter++;
        return;

    case 1:
        incoming.data = byte;
        RXCounter++;
        return;

    case 2:
        incoming.flags = byte;
        RXCounter++;
        break;
    }


    if(incoming.checksum == I2C_CHECKSUM)
    {

        if((parity(incoming.checksum) ^
           parity(incoming.data) ^
           parity(incoming.flags & ~PARITY)) != (incoming.flags & PARITY))
        {
            //Parity Errors!
            errorFlag = 1;
            return;
        }


        /*Process data*/
        boardState->desiredSpeed = incoming.data;

        if(incoming.flags & CCW) boardState->counterClockwise = 1;
        else boardState->counterClockwise = 0;

        if(incoming.flags & STARTUP) boardState->inShutdown = 0;


        //If both STARTUP and SHUTDOWN, then SHUTDOWN takes priority.
        if(incoming.flags & SHUTDOWN)
        {
            boardState->inShutdown = 1;
            shutdownMotor();
        }
        else
        {
            updateMotorSpeed();
        }
    }
    else
    {
        //Checksum error!
        errorFlag = 1;
    }
}

unsigned char generateOutgoingByte()
{
    switch(TXCounter)
    {
    case 0:
        outgoing.checksum = 'A';
        outgoing.data = boardState->currentSpeed;
        outgoing.flags = 0;
        if(boardState->inShutdown) outgoing.flags |= MD_NOK;
        else outgoing.flags |= MD_OK;

        if(boardState->counterClockwise) outgoing.flags |= CCW;

        if(errorFlag)
        {
            outgoing.flags |= ERROR;
            errorFlag = 0;
        }

        TXCounter++;
        return outgoing.checksum;

    case 1:
        TXCounter++;
        return outgoing.data;

    case 2:
        TXCounter++;
        return outgoing.flags;
    }

    return 0;
}

/*
 * Returns the parity of an individual char.
 */
unsigned char parity(unsigned char x)
{
    unsigned char par = 0;
    int i;
    for(i = 0; i < 8; i++)
    {
        par ^= x;
        x >>= 1;
    }

    return par & 0x01;
}
