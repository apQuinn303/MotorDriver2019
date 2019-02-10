/*
 * Commutation.c
 *
 *  Created on: Feb 4, 2019
 *      Author: AQaut
 */

#include "LegendPWM.h"
#include "Commutation.h"
#include "MDtypes.h"

#pragma vector = PORT3_VECTOR
__interrupt void P3_ISR(void)
{
    switch(P3IV)
    {
    case P3IV_P3IFG2:
        if(P3IN & BIT2) P3IES |= BIT2; //Swap interrupt edge direction.
        else P3IN &= ~BIT2;
        updateCommutationState();
        return;
    case P3IV_P3IFG3:
        if(P3IN & BIT3) P3IES |= BIT3; //Swap interrupt edge direction.
        else P3IN &= ~BIT3;
        updateCommutationState();
        return;

    default:
        return;
    }
}

#pragma vector = PORT4_VECTOR
__interrupt void P4_ISR(void)
{
    switch(P4IV)
    {
    case P4IV_P4IFG7:
        if(P4IN & BIT7) P4IES |= BIT7; //Swap interrupt edge direction.
        else P4IN &= ~BIT7;
        updateCommutationState();
        return;

    default:
        return;
    }
}


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


void initializeCommutation(void)
{
    //P3.2
    P3DIR &= ~BIT2; //Set as an input

    if(P3IN & BIT2) P3IES |= BIT2; //Set up interrupt edge based on state.
    else P3IN &= ~BIT2;

    P3IFG &= ~BIT2; //Clear the interrupt flag.
    P3IE |= BIT2; //Enable interrupt

    //P3.3
    P3DIR &= ~BIT3; //Set as an input

    if(P3IN & BIT3) P3IES |= BIT3; //Set up interrupt edge based on state.
    else P3IN &= ~BIT3;

    P3IFG &= ~BIT3; //Clear the interrupt flag.
    P3IE |= BIT3; //Enable interrupt

    //P4.7
    P4DIR &= ~BIT7; //Set as an input

    if(P4IN & BIT7) P4IES |= BIT7; //Set up interrupt edge based on state.
    else P4IN &= ~BIT7;

    P4IFG &= ~BIT7; //Clear the interrupt flag.
    P4IE |= BIT7; //Enable interrupt

}

void shutdownMotor(void)
{
    setPhaseA(0);
    setPhaseB(0);
    setPhaseC(0);
    state.desiredSpeed = 0;
}


void updateCommutationState(void)
{
    if(state.inShutdown)
    {
        shutdownMotor();
        return;
    }

    unsigned char pwm_max_duty_cycle = state.desiredSpeed;

    if(pwm_max_duty_cycle > 100) pwm_max_duty_cycle = 100;


    //Form a bit vector out of the hall state inputs.
    //NOTE: May need to change this if the windings are different than anticipated.
    unsigned char hallState = 0;
    if(P4IN & BIT7) hallState |= BIT0;
    if(P3IN & BIT3) hallState |= BIT1;
    if(P3IN & BIT2) hallState |= BIT2;

    unsigned char commutationState;

    if(state.counterClockwise)  commutationState = CounterCWCommutationTable[hallState];
    else commutationState = CWCommutationTable[hallState];


    //Set up PWM outputs.

    //Phase A/U
    if(commutationState & HS_U)
    {
        setPhaseA(pwm_max_duty_cycle);
    }
    else if(commutationState & LS_U)
    {
        setPhaseA(0);
    }
    else
    {
        setPhaseA(pwm_max_duty_cycle/2);
    }

    //Phase B/V
    if(commutationState & HS_V)
        {
            setPhaseB(pwm_max_duty_cycle);
        }
        else if(commutationState & LS_V)
        {
            setPhaseB(0);
        }
        else
        {
            setPhaseB(pwm_max_duty_cycle/2);
        }

    //Phase C/W (not to be confused with "CW" ;) )
    if(commutationState & HS_W)
        {
            setPhaseC(pwm_max_duty_cycle);
        }
        else if(commutationState & LS_W)
        {
            setPhaseC(0);
        }
        else
        {
            setPhaseC(pwm_max_duty_cycle/2);
        }


}


