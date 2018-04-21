#include "msp.h"

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
            WDT_A_CTL_HOLD;

    // Configure GPIO
    P1->DIR |= BIT0;
    P1->OUT |= BIT0;

    //enable writing to clock systems
    CS ->KEY = CS_KEY_VAL;
    //clear control register 0
    CS ->CTL0 = 0;
    //set DCO freq to 24 MHz
    CS ->CTL0 |= CS_CTL0_DCORSEL_4;
    //Set DCO as the source for SMCLK
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;
    //disable writing to clock systems
    CS ->KEY = 0;

    //Output SMCLK frequency for verification
    P4 -> SEL0 |= BIT4;
    P4 -> SEL1 &= ~BIT4;
    P4 -> DIR |= BIT4;


    // TACCR0 interrupt enabled
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;

    //Time on, 25% duty cycle
    TIMER_A0->CCR[0] = 242;
    //Period 25kHz clock
    TIMER_A0->CCR[1] = 968;

    // SMCLK, continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    // Enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);

    while (1);
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void)
{
    //Turn LED off after 25%
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        P1->OUT &= ~BIT0;
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
        TIMER_A0->CCR[0] += 968;              // Add Offset to TACCR0
    }
}
void TA0_N_IRQHandler(void)
{
    //Turn LED on after full period
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
        P1->OUT |= BIT0;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
        TIMER_A0->CCR[1] += 968;              // Add Offset to TACCR1
    }
}


