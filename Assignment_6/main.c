/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/25/2018
 * Description: A program that uses the MCP4921 DAC to output
 * a 2Vpp, 50Hz, 50% duty cycle, square wave with a 1V DC offset.
 */
#include "DAC.h"

int main(void) {
    //Disable Watchdog Timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    //initializing Timer, SPI, and DAC
    Timer_init();
    SPI_init();
    DAC_init();

    while(1);
}

//TIMER_A0 CCR0 ISR, set signal high, 2 Volts
void TA0_0_IRQHandler(void)
{
    if ( TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        DAC_data(2700); //tuned to output 2V
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

//TIMER_A0 CCR1 ISR, set signal low, 0 Volts
void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
        DAC_data(0);
    }
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}


