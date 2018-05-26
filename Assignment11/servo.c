#include "servo.h"
void PWM_init(void)
{
    CS -> KEY = CS_KEY_VAL;
    CS -> CTL0 = 0;
    CS -> CTL0 |= CS_CTL0_DCORSEL_1;
    CS -> CTL1 |= CS_CTL1_SELS__DCOCLK;
    CS -> KEY = 0;

    P6 -> SEL0 |= BIT6 | BIT7;
    P6 -> DIR  |= BIT6 | BIT7;

    //Select SMCLK as the source for TIMERA0 and use UP mode
    TIMER_A2-> CTL =  TIMER_A_CTL_SSEL__SMCLK |
                      TIMER_A_CTL_MC__UP      |
                      TIMER_A_CTL_CLR;

    TIMER_A2->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7;

    TIMER_A2->CCR[0] = PERIOD_COUNT;
    TIMER_A2->CCR[3] = 1500;
    return;
}

void servo_angle(uint8_t degree)
{
    if(degree <= 18)
    {
        uint32_t timeOnCount = 1500 + degree*310;
        TIMER_A2->CCR[0] = PERIOD_COUNT;
        TIMER_A2->CCR[3] = timeOnCount;
    }
    return;
}
