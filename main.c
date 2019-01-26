#include <msp430.h> 
#include "LegendPWM.h"
#include "LegendI2C.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	setupPWM();
	setupI2C();

	_BIS_SR(CPUOFF + GIE);
	return 0;
}
