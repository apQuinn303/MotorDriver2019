#include <msp430.h> 
#include "LegendPWM.h"
#include "LegendI2C.h"
#include "MDtypes.h"

//Forward Declarations
void setupFaultInterrupts(void);


#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    switch(P1IV)
    {
    case P1IV_P1IFG1:
        state.inShutdown = 1;
        //TODO: Initiate immediate motor shutdown.
        return;

    default:
        return;
    }
}


/**
 * Main Function - Set up timers, interrupts, and libraries,
 * and then go into low power mode.
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//Make sure high-impedance mode is disabled.
	PM5CTL0 &= ~LOCKLPM5;

	//Initially...

	state.currentSpeed = 0; //Hopefully!
	state.desiredSpeed = 0;
	state.inShutdown = 1;   //Start off in shutdown; wait to turn on.

	//Initialize various communications protocols.

	setupFaultInterrupts();

	setupPWM();
	setupI2C(&state);


	//Enter low power mode with interrupts enabled.

	_BIS_SR(CPUOFF + GIE);
	return 0;
}


void setupFaultInterrupts(void)
{
    PM5CTL0 &= ~LOCKLPM5;

    //Setup P1.1 to be an input with a pullup resistor.
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    P1IES |= BIT1; //P1.1 should have a high->low interrupt.
    P1IFG &= ~BIT1; //Clear the interrupt flag.
    P1IE |= BIT1; //Enable interrupt on P1.1
}
