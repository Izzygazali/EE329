/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/21/2018
 * Description: Program that outputs two signals to ports 1.0 and 6.0
 *              The program resembles a 2-bit counter with port 1.0 having the
 *              most significant bit.
 */

#include "msp.h"
#include "clock_pulse.h"


int main(void) {
    //Disable Watchdog Timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    //Set DCO clock frequency and select as source for SMCLK
    CS ->KEY = CS_KEY_VAL;              //enable writing to clock systems
    CS ->CTL0 = 0;                      //clear control register 0
    CS ->CTL0 |= CS_CTL0_DCORSEL_0;     //set DCO freq to 1.5 MHz
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;  //Set DCO as the source for SMCLK
    CS ->KEY = 0;                       //disable writing to clock systems

    // Configure GPIO, P1.0 MSB and P6.0 LSB
    P1->DIR |= BIT0;
    P1->OUT |= BIT0;
    P6 -> SEL0 &= ~BIT0;
    P6 -> SEL1 &= ~BIT0;
    P6 -> DIR |= BIT0;

    //Generate signal.
    TIMERA0_variablePulse(1, 50, 1.5);

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    while (1);
}

// Timer A0 interrupt service routine
// Port 1.0 signal is set to a have a 50% duty cycle and 1ms period.
// Port 6.0 toggles every 2 port 1.0 clock pulses.
void TA0_0_IRQHandler(void)
{
    static uint8_t count = 0;
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        P1->OUT &= ~BIT0;

        if(count == 1){
            P6-> OUT &= ~BIT0;
            count = 0;
        }else{
            P6-> OUT |= BIT0;
            count++;
        }
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
void TA0_N_IRQHandler(void)
{

    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
        P1->OUT |= BIT0;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
