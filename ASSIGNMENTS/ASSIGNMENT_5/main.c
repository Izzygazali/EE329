#include "msp.h"
#define CCR0_COUNT 1500
#define CCR1_COUNT 750

//for 25% duty cycle CCRO_COUNT 242 and CCR1_COUNT 968
//lowest pulse achieved at 84
//30250 CCR0_COUNT for 20ms pulse at 1.5MHz iterate through ISR 500 to get 10s pulse

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
            WDT_A_CTL_HOLD;

    // Configure GPIO
    P4->DIR |= BIT0;
    P4->OUT |= BIT0;

    //enable writing to clock systems
    CS ->KEY = CS_KEY_VAL;
    //clear control register 0
    CS ->CTL0 = 0;
    //set DCO freq to 24 MHz
    CS ->CTL0 |= CS_CTL0_DCORSEL_0;
    //Set DCO as the source for SMCLK
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;
    //disable writing to clock systems
    CS ->KEY = 0;

    // TACCR0 interrupt enabled
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;

    //Time on, 25% duty cycle
    TIMER_A0->CCR[0] = CCR0_COUNT;
    //Period 25kHz clock
    TIMER_A0->CCR[1] = CCR1_COUNT;

    // SMCLK, continuous mode for 50% and 25% duty cycles
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;

    // SMCLK, up mode for 20s period pulse
    //TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    // Enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);

    while (1);
}



void TA0_0_IRQHandler(void)
{
    //Turn LED off after 25%
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        P4->OUT ^= BIT0;
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
}
void TA0_N_IRQHandler(void)
{
    //Turn LED on after full period
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
        //Turn LED off after 25%
            if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
            {
                P4->OUT ^= BIT0;

                TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
            }

    }
}

