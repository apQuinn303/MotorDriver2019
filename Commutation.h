/*
 * Commutation.h
 *
 *  Created on: Feb 4, 2019
 *      Author: AQaut
 */

#ifndef COMMUTATION_H_
#define COMMUTATION_H_



// Hall Commutation State Flags
#define LS_U    0x01
#define HS_U    0x02
#define LS_V    0x04
#define HS_V    0x08
#define LS_W    0x10
#define HS_W    0x20

#define DEFAULT_MOTOR_STRENGTH 25


unsigned char currentHallState;


void initializeCommutation(void);
void initializeHallSensors(void);
void shutdownMotor(void);
void updateCommutationState(void);
void updateMotorSpeed(void);




#endif /* COMMUTATION_H_ */
