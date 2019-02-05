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

unsigned char CounterCWCommutationTable[] = { 0x00,
                                      HS_W|LS_V,       // Hall position 001
                                      HS_V|LS_U,       // Hall position 010
                                      HS_W|LS_U,       // Hall position 011
                                      HS_U|LS_W,       // Hall position 100
                                      HS_U|LS_V,       // Hall position 101
                                      HS_V|LS_W,       // Hall position 110
                                        0x00   };


unsigned char CWCommutationTable[] = {  0x00,
                                      HS_V|LS_W,       // Hall position 001
                                      HS_U|LS_V,       // Hall position 010
                                      HS_U|LS_W,       // Hall position 011
                                      HS_W|LS_U,       // Hall position 100
                                      HS_V|LS_U,       // Hall position 101
                                      HS_W|LS_V,       // Hall position 110
                                        0x00   };



void initializeCommutation(void);
void shutdownMotor(void);
void updateCommutationState(void);




#endif /* COMMUTATION_H_ */
