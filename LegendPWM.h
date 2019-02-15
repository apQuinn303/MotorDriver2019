/*
 * LegendPWM.h
 *
 *  Created on: Dec 20, 2018
 *      Author: AQaut
 */


#ifndef LEGENDPWM_H_
#define LEGENDPWM_H_

#include <msp430.h>

//#define CLOCK_PERIOD 32 //1kHz
#define CLOCK_PERIOD 320 //100Hz

void setupPWM(void);
void setPhaseA(unsigned char dutycycle);
void setPhaseB(unsigned char dutycycle);
void setPhaseC(unsigned char dutycycle);




#endif /* LEGENDPWM_H_ */
