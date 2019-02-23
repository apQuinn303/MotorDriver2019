#include <stdio.h>
#include "msp430.h"
#include "MDtypes.h"

//Hall W = P3.2_A14_C14 pin 6
//Hall_V = P3.3_A15_C15 pin 7
//HALL_U = P4.7 pin 8

void setupHALL() {
	//Enabling Interrupts
	P3IE |= BIT2;
	P3IE |= BIT3;
	P4IE |= BIT7;
	//Setting IE rising edge
	P3IES &= ~BIT2;
	P3IES &= ~BIT3;
	P4IEs &= ~BIT7;

	//Clearing Flags
	P3FG &= ~BIT2;
	P3FG &= ~BIT3;
	P4FG &= ~BIT7;

	//Setting Up Timer
	CCTL0 &= BIT8
	TACTL = TASSEL_1 + MC_2 + ID_0; //ACLK Continuous Mode 1 Divider
	TA1CCTL1 &= BIT11 + BIT8;
	_EINT();
}

#pragma vector=PORT3_VECTOR //Sets Current Speed as frequency of motor revolutions per minute
__interrupt void Port_3(void) {
    TA1CCTL1 ^= BIT13;
    unsigned int currTime = TA1CCR1;
    deltaT = currTime - lastTime;
    double seconds = ((double)deltaT/32768);
    state.currentSpeed = 60/(3*deltaT*128); //RPM
    lastTime = currTime;
    P3FG &= ~BIT2;
    P3FG &= ~BIT3;
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void) {
    unsigned int currTime =
    deltaT = currTime - lastTime;
    state.currentSpeed = 1/(3*deltaT);
    lastTime = currTime;
    P4FG &= ~BIT7;
}
