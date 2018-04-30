#include "msp.h"
#include "delay.h"
#include "DAC.h"


int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    set_DCO(FREQ_3_MHz);
    SPI_INIT();
    INIT_TIMER();
    __enable_irq();
    while(1);
}

void TA0_0_IRQHandler(void)
{
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        P2 ->OUT |= BIT2;
        TIMER_A0->CCR[1] += 750;
    }
    P2 ->OUT &= ~BIT2;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}
