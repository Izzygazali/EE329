#include "msp.h"


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //Unlock Clock Registers
    CS ->KEY = CS_KEY_VAL;

    //sets MCLK to 12MHz with DCO
    CS ->CTL0 = 0;
    CS ->CTL0 |= CS_CTL0_DCORSEL_3 ;

    //sets ACLK to 32.768kHz with REFO and SMCLK 6MHz to with DCO
    CS ->CTL1 = CS_CTL1_SELM__DCOCLK|CS_CTL1_SELS__DCOCLK|CS_CTL1_SELA__REFOCLK|CS_CTL1_DIVS1;

    //Re-lock Clock Registers
    CS ->KEY = 0;

    //Blink LED
    P1 ->SEL0 &= -BIT0;
    P1 ->SEL1 &= -BIT1;
    P1 ->DIR |= BIT0;
    while (1){
        int i;
        P1 ->OUT ^= BIT0;
        for (i = 50000; i > 0; i--);
    }

}
