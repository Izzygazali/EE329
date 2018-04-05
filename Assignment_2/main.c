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
    /*
   if (time_us == 1 && Freq_MHz == 48){
       return;
   }
   */
   // if(time_us < 10){ return;}
    float tuningFactor = (time_us * Freq_MHz) - (time_us)*4;
    float numCycles = (time_us * Freq_MHz) - tuningFactor;
    int delayLoop;
    for(delayLoop = (int)numCycles; delayLoop >= 0; delayLoop--){}
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
            /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
             PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));

            /* Configure Flash wait-state to 1 for both banks 0 & 1 */
            FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
             ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
            FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
             ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;

            /* Configure DCO to 48MHz */
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
       delay_us(100, 48);
    }

}
