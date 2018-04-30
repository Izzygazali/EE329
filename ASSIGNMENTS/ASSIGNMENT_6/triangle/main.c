/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/24/2018
 * Description: This main file implements a 50Hz triangle wave
 *              with 2Vp-p and 1V DC offset. It utilizes DAC.c
 *              for functions necessary for proper operation.
 *
 * Pin Assignments: P4.0 -> CS
 *                  P1.5 -> SCLK
 *                  P1.6 -> SIMO
 */
#include "msp.h"
#include "delay.h"
#include "DAC.h"

static const uint16_t samples[]={

                                 2062,
                                 2078,
                                 2093,
                                 2109,
                                 2124,
                                 2140,
                                 2156,
                                 2171,
                                 2187,
                                 2202,
                                 2218,
                                 2233,
                                 2249,
                                 2264,
                                 2280,
                                 2295,
                                 2311,
                                 2326,
                                 2342,
                                 2357,
                                 2372,
                                 2388,
                                 2403,
                                 2419,
                                 2434,
                                 2449,
                                 2464,
                                 2480,
                                 2495,
                                 2510,
                                 2525,
                                 2540,
                                 2556,
                                 2571,
                                 2586,
                                 2601,
                                 2616,
                                 2631,
                                 2646,
                                 2660,
                                 2675,
                                 2690,
                                 2705,
                                 2720,
                                 2734,
                                 2749,
                                 2764,
                                 2778,
                                 2793,
                                 2807,
                                 2822,
                                 2836,
                                 2851,
                                 2865,
                                 2879,
                                 2893,
                                 2907,
                                 2922,
                                 2936,
                                 2950,
                                 2964,
                                 2978,
                                 2991,
                                 3005,
                                 3019,
                                 3033,
                                 3046,
                                 3060,
                                 3073,
                                 3087,
                                 3100,
                                 3114,
                                 3127,
                                 3140,
                                 3153,
                                 3166,
                                 3179,
                                 3192,
                                 3205,
                                 3218,
                                 3231,
                                 3243,
                                 3256,
                                 3269,
                                 3281,
                                 3293,
                                 3306,
                                 3318,
                                 3330,
                                 3342,
                                 3354,
                                 3366,
                                 3378,
                                 3390,
                                 3402,
                                 3413,
                                 3425,
                                 3436,
                                 3448,
                                 3459,
                                 3470,
                                 3481,
                                 3493,
                                 3504,
                                 3514,
                                 3525,
                                 3536,
                                 3547,
                                 3557,
                                 3568,
                                 3578,
                                 3588,
                                 3599,
                                 3609,
                                 3619,
                                 3629,
                                 3639,
                                 3648,
                                 3658,
                                 3668,
                                 3677,
                                 3686,
                                 3696,
                                 3705,
                                 3714,
                                 3723,
                                 3732,
                                 3741,
                                 3749,
                                 3758,
                                 3766,
                                 3775,
                                 3783,
                                 3791,
                                 3799,
                                 3807,
                                 3815,
                                 3823,
                                 3831,
                                 3838,
                                 3846,
                                 3853,
                                 3861,
                                 3868,
                                 3875,
                                 3882,
                                 3889,
                                 3895,
                                 3902,
                                 3909,
                                 3915,
                                 3921,
                                 3927,
                                 3934,
                                 3940,
                                 3945,
                                 3951,
                                 3957,
                                 3962,
                                 3968,
                                 3973,
                                 3978,
                                 3983,
                                 3988,
                                 3993,
                                 3998,
                                 4003,
                                 4007,
                                 4012,
                                 4016,
                                 4020,
                                 4024,
                                 4028,
                                 4032,
                                 4036,
                                 4039,
                                 4043,
                                 4046,
                                 4050,
                                 4053,
                                 4056,
                                 4059,
                                 4061,
                                 4064,
                                 4067,
                                 4069,
                                 4072,
                                 4074,
                                 4076,
                                 4078,
                                 4080,
                                 4081,
                                 4083,
                                 4085,
                                 4086,
                                 4087,
                                 4088,
                                 4089,
                                 4090,
                                 4091
};


int main(void)
{
    //Stop watch dog timer.
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    //set MCLK and DCO to 6MHz
    set_DCO(FREQ_24_MHz);
    //Initialize SPI interface
    SPI_INIT();
    //Initialize Timer
    INIT_TIMER();
    //Enable interrupts globally
    __enable_irq();
    //Wait for interrupts
    while(1);
}

/*
 * Interrupt handler determines the sample value and outputs it
 * using the DAC. The sample-rate is determined by the timing
 * of interrupts from Timer_A.
 */
void TA0_0_IRQHandler(void)
{
    //Variable for the "sample" we are currently on
    static int16_t step = 0;
    //Variable for the amplitude of that sample
    static uint16_t level = 0;
    //flag to keep track of if we are counting up or down
    static uint8_t up_down_flag = 0;
    static uint8_t pos_neg_flag = 0x01;
    static uint8_t i = 0;

    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
         //set amplitude of sample to 8 times the sample number
         if (pos_neg_flag == 0){
            level = 4095-samples[step];
           }else{
            level = samples[step];
        }
        //write this amplitude to the DAC
        if (step <= 199)
            WRITE_DAC(level);
            //check if we are at the top of the triangle wave
            if (step >= 198){
                //if we are at the top of the wave count down
                up_down_flag = 1;
                i++;
            }
            else if(step <= 1){
                //if we are at the bottom of the wave count up
                up_down_flag = 0;
                i++;
            }
            //check flag and count accordingly
            if (up_down_flag == 0)
                //count samples up
                step+=5;
            else
                //count samples down
                step-=5;
            if (i >=  4){
                pos_neg_flag ^= 0x01;
                i=0;
            }

    }
    //reset interrupt flag for Timer
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

}



