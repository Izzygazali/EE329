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

#define FREQ_100Hz 1
#define FREQ_200Hz 2
#define FREQ_300Hz 3
#define FREQ_400Hz 4
#define FREQ_500Hz 5
#define WAVE_SQR   0
#define WAVE_SAW   1
#define WAVE_SIN   2
uint8_t freq_flag;
uint8_t state_flag;


static const uint16_t samples[]={
32, 64, 96, 128, 160, 192, 224, 256, 288, 320,
351, 383, 415, 446, 477, 509, 540, 571, 601,
632, 663, 693, 723, 753, 783, 812, 842, 871,
900, 929, 957, 986, 1014, 1042, 1069, 1096,
1123, 1150, 1177, 1203, 1229, 1254, 1279,
1304, 1329, 1353, 1377, 1401, 1424, 1447,
1470, 1492, 1514, 1535, 1556, 1577, 1597,
1617, 1636, 1656, 1674, 1693, 1710, 1728,
1745, 1761, 1778, 1793, 1809, 1823, 1838,
1852, 1865, 1878, 1891, 1903, 1914, 1925,
1936, 1946, 1956, 1965, 1974, 1982, 1990,
1997, 2004, 2010, 2016, 2021, 2026, 2030,
2034, 2037, 2040, 2042, 2044, 2045, 2046,
2047, 2046, 2045, 2044, 2042, 2040, 2037,
2034, 2030, 2026, 2021, 2016, 2010, 2004,
1997, 1990, 1982, 1974, 1965, 1956, 1946,
1936, 1925, 1914, 1903, 1891, 1878, 1865,
1852, 1838, 1823, 1809, 1793, 1778, 1761,
1745, 1728, 1710, 1693, 1674, 1656, 1636,
1617, 1597, 1577, 1556, 1535, 1514, 1492,
1470, 1447, 1424, 1401, 1377, 1353, 1329,
1304, 1279, 1254, 1229, 1203, 1177, 1150,
1123, 1096, 1069, 1042, 1014, 986, 957, 929,
900, 871, 842, 812, 783, 753, 723, 693, 663,
632, 601, 571, 540, 509, 477, 446, 415, 383,
351, 320, 288, 256, 224, 192, 160, 128, 96,
64, 32, 0
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
    freq_flag = FREQ_100Hz;
    state_flag = WAVE_SAW;
    P1->DIR |= BIT0;
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
    static int16_t max_step = 199;
    //Variable for the amplitude of that sample
    static uint16_t level = 0;
    //flag to keep track of pos/neg half cycle
    static uint8_t pos_neg_flag = 0x00;

    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        switch (state_flag)
        {
            case WAVE_SIN:
                if (pos_neg_flag == 0x01)
                    level = 2047-samples[step];
                else
                    level = 2047+samples[step];
                max_step = 199;
                break;
            case WAVE_SAW:
                level = (int)(step*10.25);
                max_step = 399;
                break;
            case WAVE_SQR:
                level = 4095;
                break;
        }
        //write this amplitude to the DAC
        WRITE_DAC(level);
        //check if we are at the top of the triangle wave
        switch (freq_flag)
        {
            case FREQ_100Hz:
                step+=1;
                break;
            case FREQ_200Hz:
                step+=2;
                break;
            case FREQ_300Hz:
                step+=3;
                break;
            case FREQ_400Hz:
                step+=4;
                break;
            case FREQ_500Hz:
                step+=5;
                break;
        }
        if (step >= max_step){
            //if we are at the top of the wave count down
            step = 0;
            pos_neg_flag ^= 0x01;
        }
    }


    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
      if (state_flag == WAVE_SQR)
          WRITE_DAC(0);
    }
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}

