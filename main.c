#include <msp430.h> 
#include "LegendPWM.h"
#include "LegendI2C.h"
#include "MDtypes.h"




/**
 * Main Function - Set up timers, interrupts, and libraries,
 * and then go into low power mode.
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//Initially...

	state.currentSpeed = 0; //Hopefully!
	state.desiredSpeed = 0;
	state.inShutdown = 1;   //Start off in shutdown; wait to turn on.

	//Initialize various communications protocols.

	setupPWM();
	setupI2C(&state);
	setupHall();


	//Enter low power mode with interrupts enabled.

	_BIS_SR(CPUOFF + GIE);
	return 0;
}



