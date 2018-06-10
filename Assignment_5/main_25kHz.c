/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/21/2018
 * Description: Program that outputs a 25% duty cycle, 25kHz clock to
 *              port 1.0
 */
#include "clock_pulse.h"

int main(void) {
    //Disable Watchdog Timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;


    //Set DCO clock frequency and select as source for SMCLK
    CS ->KEY = CS_KEY_VAL;              //enable writing to clock systems
    CS ->CTL0 = 0;                      //clear control register 0
    CS ->CTL0 |= CS_CTL0_DCORSEL_4;     //set DCO freq to 24 MHz
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;  //Set DCO as the source for SMCLK
    CS ->KEY = 0;                       //disable writing to clock systems

    // Configure GPIO
    P1->DIR |= BIT0;
    P1->OUT |= BIT0;

    //Output SMCLK frequency for verification
    P4 -> SEL0 |= BIT4;
    P4 -> SEL1 &= ~BIT4;
    P4 -> DIR |= BIT4;

    //Generate a pulse with 25% duty cycle at 25kHz, using SMCLK 24MHz
    TIMERA0_variablePulse(25, 25, 24);

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    while (1);
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void)
{
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        P1->OUT |= BIT0;
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
        P1->OUT &= ~BIT0;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}

