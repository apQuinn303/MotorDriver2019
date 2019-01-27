/*
 * MDtypes.h
 *
 *  Created on: Jan 27, 2019
 *      Author: AQaut
 */

#ifndef MDTYPES_H_
#define MDTYPES_H_

//Struct to keep track of the Motor Driver State.
typedef struct MDstate {
    unsigned char currentSpeed;
    unsigned char desiredSpeed;
    unsigned char inShutdown; //1 or 0
} MDstate_t;

volatile MDstate_t state;


#endif /* MDTYPES_H_ */
