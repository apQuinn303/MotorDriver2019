/*
 * LegendPWM.c
 *
 *  Created on: Dec 20, 2018
 *      Author: AQaut
 */

#include "msp430.h"
#include "LegendPWM.h"

#pragma vector = TIMER0_B0_VECTOR
__interrupt void timerBISR0(void){

    //Assert all outputs.
    P3OUT |= BIT6;
    P4OUT |= BIT4;
    P4OUT |= BIT6;

}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void timerBISR1(void){

    switch(TB0IV)
    {
    case 2:
        P4OUT &= ~BIT6; //Phase A
        break;

    case 4:
        P4OUT &= ~BIT4; //Phase B
        break;

    case 6:
        P3OUT &= ~BIT6; //Phase C
        break;

    default: //If it's none of the interrupts we care about, return.
        return;

    }

}


/*
 * Sets up the PWM to source from 32 kHz ACLK
 */
void setupPWM(void)
{
    //TB0CCR0 defines the clock period. For example 32kHz / 32 period = 1 kHz PWM frequency
    TB0CCR0 = CLOCK_PERIOD;

    //Set the clock select to "ACLK" and Mode Control to "UP" and Input Divider to /1 and then clear the clock.
    TB0CTL = TBSSEL_1 | MC_1 | ID_0 | TBCLR;

    //Enable interrupts on CCTL0
    TB0CCTL0 |= CCIE;

    //Make sure GPIO output pins are activated.
    PM5CTL0 &= ~LOCKLPM5;

    //Set the three PWM pins to GPIO function.
    P3SEL0 &= ~BIT6;
    P4SEL0 &= ~BIT4;
    P4SEL0 &= ~BIT6;
    P3SEL1 &= ~BIT6;
    P4SEL1 &= ~BIT4;
    P4SEL1 &= ~BIT6;

    //Set the three PWM pins to output direction.
    P3DIR |= BIT6;
    P4DIR |= BIT4;
    P4DIR |= BIT6;

    //Zero the outputs.
    P3OUT &= ~BIT6;
    P4OUT &= ~BIT4;
    P4OUT &= ~BIT6;

    //Set up the first three CCR to generate interrupts, each at 50% duty cycle.
    TB0CCR1 = CLOCK_PERIOD/2;
    TB0CCR2 = CLOCK_PERIOD/2;
    TB0CCR3 = CLOCK_PERIOD/2;

    TB0CCTL1 |= CCIE;
    TB0CCTL2 |= CCIE;
    TB0CCTL3 |= CCIE;

}

void setPhaseA(unsigned char dutycycle)
{
    TB0CCR1 = (((unsigned int)dutycycle)*CLOCK_PERIOD)/100;
}

void setPhaseB(unsigned char dutycycle)
{
    TB0CCR2 = (((unsigned int)dutycycle)*CLOCK_PERIOD)/100;
}

void setPhaseC(unsigned char dutycycle)
{
    TB0CCR3 = (((unsigned int)dutycycle)*CLOCK_PERIOD)/100;
}




