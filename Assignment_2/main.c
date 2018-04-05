#include "msp.h"

#define FREQ_15_MHz 0
#define FREQ_3_MHz 1
#define FREQ_6_MHz 2
#define FREQ_12_MHz 3
#define FREQ_24_MHz 4
#define FREQ_48_MHz 5

void delay_ms(int time_ms, int Freq_MHz){
    float tuningFactor = 0.1 ;
    float numCycles = tuningFactor * (time_ms * Freq_MHz * 1000);
    int delayLoop;
    for(delayLoop = (int)numCycles; delayLoop >= 0; delayLoop--){}
    return;
}

void delay_us(int time_us, int Freq_MHz){
   // if(time_us < 10){ return;}
    //float tuningFactor = .01 ;
    //float numCycles = tuningFactor * (time_us * Freq_MHz);
    //int delayLoop;
    //for(delayLoop = (int)numCycles; delayLoop >= 0; delayLoop--){}
    return;
}

void set_DCO(int FREQ){
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;

    switch(FREQ){
        case 0:
            CS->CTL0 |= CS_CTL0_DCORSEL_0;
            break;
        case 1:
            CS->CTL0 |= CS_CTL0_DCORSEL_1;
            break;
        case 2:
            CS->CTL0 |= CS_CTL0_DCORSEL_2;
            break;
        case 3:
            CS->CTL0 |= CS_CTL0_DCORSEL_3;
            break;
        case 4:
            CS->CTL0 |= CS_CTL0_DCORSEL_4;
            break;
        case 5:
            CS->CTL0 |= CS_CTL0_DCORSEL_5;
            break;
        default:
            break;
    }
    CS->CTL1 |= CS_CTL1_SELM_3;
    CS->KEY = 0;
}


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    set_DCO(FREQ_3_MHz);

    P1 ->SEL0 &= ~BIT0;
    P1 ->SEL1 &= ~BIT1;
    P1 ->DIR |= BIT0;

    //Output MCLK frequency for verification
    P4 -> SEL0 |= BIT3;
    P4 -> SEL1 &= ~BIT3;
    P4 -> DIR |= BIT3;


    while (1){
       P1 ->OUT ^= BIT0;
       delay_us(1, 3);
    }

}
