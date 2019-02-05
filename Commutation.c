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
    case P3IV_P3IFG3:
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
        updateCommutationState();
        return;

    default:
        return;
    }
}


void initializeCommutation(void)
{
    //TODO: Set up interrupts on P3.2, P3.3, and P4.7

    pwm_max_duty_cycle = state.desiredSpeed;
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


    unsigned char hallState = 0;

    //TODO: Put values from P3.2, P3.3, P4.7 into hallState's lowest bits in some order.
    //NOTE: Doing this right/wrong will determine if the motor spins or not.

    unsigned char commutationState = CWCommutationTable[hallState];

    //TODO: Need a new global state variable for CW vs CCW, and an IF statement to determine which table we use.


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


