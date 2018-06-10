/*
 * Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/05/2018
 */
#include "delay.h"

/* Function that sets the DCO Clock frequency. MCLK is also set to the DCO clock frequency
 * INPUTS       int freq = frequency to set the DCO from pre-defined frequency values
 * RETURN       NONE
 */
void set_DCO(int freq)
{
    CS ->KEY = CS_KEY_VAL;                          //enable writing to clock systems
    CS ->CTL0 = 0;                                  //clear control register 0

    switch(freq)
    {   //set DCO SEL to desired frequency setting
        case FREQ_1_5_MHz:
            CS ->CTL0 |= CS_CTL0_DCORSEL_0;
            break;
        case FREQ_3_MHz:
            CS ->CTL0 |= CS_CTL0_DCORSEL_1;
            break;
        case FREQ_6_MHz:
            CS ->CTL0 |= CS_CTL0_DCORSEL_2;
            break;
        case FREQ_12_MHz:
            CS ->CTL0 |= CS_CTL0_DCORSEL_3;
            break;
        case FREQ_24_MHz:
            CS ->CTL0 |= CS_CTL0_DCORSEL_4;
            break;

        //for the 48 MHz setting, PCM modifications are necessary
        case FREQ_48_MHz:
            /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            /*Configure Flash wait-state to 1 for both banks 0 & 1 */
            FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
             ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
            FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
             ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
            /*Set the DCO setting to 48 MHz */
            CS ->CTL0 |= CS_CTL0_DCORSEL_5;
            break;
        default:
            break;
    }
    CS ->CTL1 |= CS_CTL1_SELM__DCOCLK;                  //Set DCO as the source for MCLK
    CS ->KEY = 0;                                       //disable writing to clock systems
    return;
}



/* Function that delays program execution for a given number of micro seconds.
 * INPUTS       float time_us = time in micro seconds
 *              int freq = frequency of the program execution clock, MCLK.
 * RETURN       NONE
 */
void delay_us(float time_us, int freq)
{
     float calcCycles;                      //calculated cycles to run through the delay loop

     switch(freq)
     {
        //The case statements correspond to the frequency definitions above (#define's)
        //The function for each case is "tuned" to produce the correct delay times
        case FREQ_1_5_MHz:
            calcCycles = 0.145*time_us-36;
            break;
        case FREQ_3_MHz:
            calcCycles = 0.293*time_us-35;
            break;
        case FREQ_6_MHz:
            calcCycles = 0.55*time_us-33;
            break;
        case FREQ_12_MHz:
            calcCycles = 1.12*time_us-35;
            break;
        case FREQ_24_MHz:
            calcCycles = 2.198*time_us-35;
            break;
        case FREQ_48_MHz:
            calcCycles = 3.9*time_us-47;
            break;
        default:
            break;
     }

     //loop utilizes nop assembly instruction in a loop to create delay
     int cycles;
     for(cycles = calcCycles; cycles > 0; --cycles){__asm(" nop");}
     return;
}


/* Function that delays program execution for a given number of milli seconds
 * INPUTS       float time_us = time in milli seconds
 *              int freq = frequency of the program execution clock, MCLK.
 * RETURN       NONE
 */
void delay_ms(float time_ms, int freq)
{
    //Utilizes the microsecond delay to create millisecond delays
    //It isn't multiplied by 1000 to account for the time introduced by calling delay_us
    delay_us(time_ms*950, freq);
    return;
}
