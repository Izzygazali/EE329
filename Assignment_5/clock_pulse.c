/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/21/2018
 */

#include "clock_pulse.h"

/*
 * Function that sets the value for CCR0 and CCR1 on TIMERA0 to values
 * that correspond to a desired pulse width
 * INPUTS:  FREQ_kHz        = Desired waveform frequency in kHz
 *          DUTY_CYCLE      = Desired waveform duty cycle
 *          SMCLK_FREQ_MHz  = Frequency that SMCLK is set to
 */
void TIMERA0_variablePulse(float FREQ_kHz, float DUTY_CYCLE, float SMCLK_FREQ_MHz)
{
    //Formulas for determining the count for period and time on.
    int periodCount = (int)((SMCLK_FREQ_MHz/FREQ_kHz)*(1000) + 4);
    int timeOnCount = (int)((periodCount)*(DUTY_CYCLE/100) + 1);

    // Enable CCR0 and CCR1 interrupts and clear flags.
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;

    //Set CCR0 and CCR1 values.
    TIMER_A0->CCR[0] = periodCount;
    TIMER_A0->CCR[1] = timeOnCount;

    //Select SMCLK as the source for TIMERA0 and use UP mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;

    // Enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
}
