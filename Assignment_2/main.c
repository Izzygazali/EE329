#include "msp.h"
#include "delay.h"

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    set_DCO(FREQ_48_MHz);

    P1 ->SEL0 &= ~BIT0;
    P1 ->SEL1 &= ~BIT1;
    P1 ->DIR |= BIT0;

    //Output MCLK frequency for verification
    P4 -> SEL0 |= BIT3;
    P4 -> SEL1 &= ~BIT3;
    P4 -> DIR |= BIT3;

    while (1){
       P1 ->OUT ^= BIT0;
       delay_us(100, FREQ_48_MHz);
    }

}
