/*
 * Commutation.c
 *
 *  Created on: Feb 4, 2019
 *      Author: AQaut
 */

#include "LegendPWM.h"
#include "Commutation.h"
#include "MDtypes.h"

#define AdamHack 1

unsigned char hallStateRecord[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char hallStateRecordStart;

unsigned char CCWhallStateSequence[] = {2, 3, 1, 5, 4, 6};
unsigned char hallStateSequence[] = {6, 4, 5, 1, 3, 2};

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


#pragma vector = TIMER0_A0_VECTOR
__interrupt void timerAISR0(void){

    updateCommutationState();
}

/* We source the commutation clock from ACLK, which is a 32kHz clock. We divide the clock by 8, which means
 * it fills up every eight seconds,
 */
void initializeCommutation(void)
{
    currentHallState = 0;
    hallStateRecordStart = 0;

    //TA0CCR0 defines the commutation speed. We write "0" to it so we don't immediately start commutating.
    TA0CCR0 = 0;

    //Set the clock select to "ACLK" and Mode Control to "UP" and Input Divider to /8 and then clear the clock.
    TA0CTL = TASSEL_1 | MC_1 | ID_3 | TACLR;

    //Enable interrupts on CCTL0
    TA0CCTL0 |= CCIE;

}

/*The user passes us a uint8 to define speed, and I want this to map to a value from 0 to 0.5 rev/sec (0-30rpm)
* There are 24 electrical phases per revolution, so at the fastest, I want somewhere from 0 to 12 phases/sec.
* A value of 256 should respond to a phase change every 1/12 = 0.0833 sec = 334 clock ticks.
* (32 kHz clock, divided by 8 in above function = 4000 clock ticks/sec, 334/4000 = 0.0835.)
* If I was inputing values from 1-100, I could divide by that number to get at minimum 320, so if I can rescale 256
* to be 0-100.
*/
void updateMotorSpeed()
{
    if(state.desiredSpeed == 0) TA0CCR0 = 0; // set commutation timer to 0
    else TA0CCR0 = 32000 / ((state.desiredSpeed*100)/256);
}

void initializeHallSensors(void)
{


#ifdef AdamHack
    //In my hacked-together board, Hall U = PJ.6 Hall V = PJ.7 Hall W = PJ.2

    PJDIR &= ~BIT6; //Set as an input
    PJDIR &= ~BIT7; //Set as an input
    PJDIR &= ~BIT2; //Set as an input


#else
    //P4.7 ~ Hall U
    P4DIR &= ~BIT7; //Set as an input

    if(P4IN & BIT7) P4IES |= BIT7; //Set up interrupt edge based on state.
    else P4IN &= ~BIT7;

    P4IFG &= ~BIT7; //Clear the interrupt flag.
    //P4IE |= BIT7; //Enable interrupt

    //P3.2 ~ Hall W
        P3DIR &= ~BIT2; //Set as an input

        if(P3IN & BIT2) P3IES |= BIT2; //Set up interrupt edge based on state.
        else P3IES &= ~BIT2;

        P3IFG &= ~BIT2; //Clear the interrupt flag.
        //P3IE |= BIT2; //Enable interrupt

        //P3.3 ~ Hall V
        P3DIR &= ~BIT3; //Set as an input

        if(P3IN & BIT3) P3IES |= BIT3; //Set up interrupt edge based on state.
        else P3IES &= ~BIT3;

        P3IFG &= ~BIT3; //Clear the interrupt flag.
        //P3IE |= BIT3; //Enable interrupt

#endif


}

/*If it takes us "ticks"-many ticks of TA0 to do one electrical cycle, and
 * TA0_ticks = 32000 / ((speed*100)/256), then
 * speed = 32000 * 256 / (TA0_ticks*100)
 */
void setCurrentSpeed(unsigned int ticks)
{
    //Split up the multiplication to make sure it stays in INT range.
    state.currentSpeed = (state.desiredSpeed * 6) / ticks;
}

void shutdownMotor(void)
{
    TB0CCR0 = 0;
    TA0CCR0 = 0;
    P4OUT &= ~BIT6; //Phase A
    P4OUT &= ~BIT4; //Phase B
    P3OUT &= ~BIT6; //Phase C

    state.desiredSpeed = 0;
}


void updateCommutationState(void)
{
    if(state.inShutdown)
    {
        shutdownMotor();
        return;
    }
    else
    {
        TB0CCR0 = CLOCK_PERIOD;
    }

    unsigned char pwm_max_duty_cycle = DEFAULT_MOTOR_STRENGTH;

    if(pwm_max_duty_cycle > 100) pwm_max_duty_cycle = 100;

    //Go through the states one at a time.
    unsigned char hallState;
    if(state.counterClockwise) hallState = CCWhallStateSequence[currentHallState];
    else hallState = hallStateSequence[currentHallState];
    currentHallState = (currentHallState + 1) % 6;


    //Form a bit vector out of the hall state inputs.
    //NOTE: May need to change this if the windings are different than anticipated.
    unsigned char realHallState = 0;
#ifdef AdamHack
    if(PJIN & BIT6) realHallState |= BIT2;
    if(PJIN & BIT7) realHallState |= BIT1;
    if(PJIN & BIT2) realHallState |= BIT0;
#else
    if(P4IN & BIT7) realHallState |= BIT2;
    if(P3IN & BIT3) realHallState |= BIT2;
    if(P3IN & BIT2) realHallState |= BIT0;
#endif

    hallStateRecord[hallStateRecordStart] = realHallState;

    if(hallStateRecord[(hallStateRecordStart + 9)%10] == realHallState
            && hallStateRecord[(hallStateRecordStart + 8)%10] == realHallState)
    {
        //Three identical hall states in a row --- we must be stalled!
        state.inShutdown = 1;
        shutdownMotor();
    }

    unsigned char i, k;
    for(k = 8; k > 0; k--)
    {
        i = (hallStateRecordStart + k)%10;
        if(hallStateRecord[i] == realHallState)
        {
            setCurrentSpeed(((unsigned int)(10-k)));
        }
    }

    hallStateRecordStart = (hallStateRecordStart+1) % 10;

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
        setPhaseALow();
    }
    else
    {
        disconnectPhaseA();
    }

    //Phase B/V
    if(commutationState & HS_V)
        {
            setPhaseB(pwm_max_duty_cycle);
        }
        else if(commutationState & LS_V)
        {
            setPhaseBLow();
        }
        else
        {
            disconnectPhaseB();
        }

    //Phase C/W (not to be confused with "CW" ;) )
    if(commutationState & HS_W)
        {
            setPhaseC(pwm_max_duty_cycle);
        }
        else if(commutationState & LS_W)
        {
            setPhaseCLow();
        }
        else
        {
            disconnectPhaseC();
        }
}


