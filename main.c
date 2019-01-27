#include <msp430.h> 
#include "LegendPWM.h"
#include "LegendI2C.h"

//Struct to keep track of the Motor Driver State.
typedef struct MDstate {
    unsigned char currentSpeed;
    unsigned char desiredSpeed;
    unsigned char inShutdown; //1 or 0
} MDstate_t;


MDstate_t state;


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
	setupI2C();


	//Enter low power mode with interrupts enabled.

	_BIS_SR(CPUOFF + GIE);
	return 0;
}
