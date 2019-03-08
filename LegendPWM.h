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

unsigned char Phase_A_DC;
unsigned char Phase_B_DC;
unsigned char Phase_C_DC;

void setupPWM(void);
void setPhaseA(unsigned char dutycycle);
void setPhaseB(unsigned char dutycycle);
void setPhaseC(unsigned char dutycycle);
void disconnectPhaseA(void);
void disconnectPhaseB(void);
void disconnectPhaseC(void);
void setPhaseALow();
void setPhaseBLow();
void setPhaseCLow();



#endif /* LEGENDPWM_H_ */
