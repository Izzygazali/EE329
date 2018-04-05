#include "msp.h"
#define FREQ_1_5_MHz 0
#define FREQ_3_MHz 1
#define FREQ_6_MHz 2
#define FREQ_12_MHz 3
#define FREQ_24_MHz 4
#define FREQ_48_MHz 5




void delay_ms(float time_ms, int freq)
{
    int numCycles = (int)(time_ms*freq*50);
    int cycles;
    for (cycles = numCycles; cycles > 0; cycles--);
    return;
}

void set_DCO(int freq)
{
    CS ->KEY = CS_KEY_VAL;
    CS ->CTL0 = 0;
    switch(freq){
        case 0:
            CS ->CTL0 |= CS_CTL0_DCORSEL_0;
            break;
        case 1:
            CS ->CTL0 |= CS_CTL0_DCORSEL_1;
            break;
        case 2:
            CS ->CTL0 |= CS_CTL0_DCORSEL_2;
            break;
        case 3:
            CS ->CTL0 |= CS_CTL0_DCORSEL_3;
            break;
        case 4:
            CS ->CTL0 |= CS_CTL0_DCORSEL_4;
            break;
        case 5:
            CS ->CTL0 |= CS_CTL0_DCORSEL_5;
            break;
        default:
            break;
    }
    CS ->CTL1 |= CS_CTL1_SELM__DCOCLK;
    CS ->KEY = 0;
    return;
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    //Blink LED
    set_DCO(2);
    P1 ->SEL0 &= -BIT0;
    P1 ->SEL1 &= -BIT0;
    P1 ->DIR |= BIT0;

    P4 ->SEL0 |= BIT3;
    P4 ->SEL1 &= -BIT3;
    P4 ->DIR |= BIT3;

    while (1){
       P1 ->OUT ^= BIT0;
       delay_ms(10, 6);
    }
}


