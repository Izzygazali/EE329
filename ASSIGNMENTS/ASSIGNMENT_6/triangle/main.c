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
                                 16,
                                 32,
                                 48,
                                 64,
                                 80,
                                 96,
                                 112,
                                 128,
                                 144,
                                 160,
                                 176,
                                 192,
                                 208,
                                 224,
                                 240,
                                 256,
                                 272,
                                 288,
                                 304,
                                 320,
                                 336,
                                 351,
                                 367,
                                 383,
                                 399,
                                 415,
                                 430,
                                 446,
                                 462,
                                 477,
                                 493,
                                 509,
                                 524,
                                 540,
                                 555,
                                 571,
                                 586,
                                 601,
                                 617,
                                 632,
                                 647,
                                 663,
                                 678,
                                 693,
                                 708,
                                 723,
                                 738,
                                 753,
                                 768,
                                 783,
                                 798,
                                 812,
                                 827,
                                 842,
                                 856,
                                 871,
                                 886,
                                 900,
                                 914,
                                 929,
                                 943,
                                 957,
                                 972,
                                 986,
                                 1000,
                                 1014,
                                 1028,
                                 1042,
                                 1055,
                                 1069,
                                 1083,
                                 1096,
                                 1110,
                                 1123,
                                 1137,
                                 1150,
                                 1163,
                                 1177,
                                 1190,
                                 1203,
                                 1216,
                                 1229,
                                 1241,
                                 1254,
                                 1267,
                                 1279,
                                 1292,
                                 1304,
                                 1317,
                                 1329,
                                 1341,
                                 1353,
                                 1365,
                                 1377,
                                 1389,
                                 1401,
                                 1412,
                                 1424,
                                 1436,
                                 1447,
                                 1458,
                                 1470,
                                 1481,
                                 1492,
                                 1503,
                                 1514,
                                 1524,
                                 1535,
                                 1546,
                                 1556,
                                 1566,
                                 1577,
                                 1587,
                                 1597,
                                 1607,
                                 1617,
                                 1627,
                                 1636,
                                 1646,
                                 1656,
                                 1665,
                                 1674,
                                 1683,
                                 1693,
                                 1702,
                                 1710,
                                 1719,
                                 1728,
                                 1736,
                                 1745,
                                 1753,
                                 1761,
                                 1770,
                                 1778,
                                 1785,
                                 1793,
                                 1801,
                                 1809,
                                 1816,
                                 1823,
                                 1831,
                                 1838,
                                 1845,
                                 1852,
                                 1858,
                                 1865,
                                 1872,
                                 1878,
                                 1884,
                                 1891,
                                 1897,
                                 1903,
                                 1909,
                                 1914,
                                 1920,
                                 1925,
                                 1931,
                                 1936,
                                 1941,
                                 1946,
                                 1951,
                                 1956,
                                 1961,
                                 1965,
                                 1970,
                                 1974,
                                 1978,
                                 1982,
                                 1986,
                                 1990,
                                 1994,
                                 1997,
                                 2001,
                                 2004,
                                 2007,
                                 2010,
                                 2013,
                                 2016,
                                 2019,
                                 2021,
                                 2024,
                                 2026,
                                 2028,
                                 2030,
                                 2032,
                                 2034,
                                 2036,
                                 2037,
                                 2039,
                                 2040,
                                 2041,
                                 2042,
                                 2043,
                                 2044,
                                 2045,
                                 2045,
                                 2046,
                                 2046,
                                 2046,
                                 2047,
                                 2046,
                                 2046,
                                 2046,
                                 2045,
                                 2045,
                                 2044,
                                 2043,
                                 2042,
                                 2041,
                                 2040,
                                 2039,
                                 2037,
                                 2036,
                                 2034,
                                 2032,
                                 2030,
                                 2028,
                                 2026,
                                 2024,
                                 2021,
                                 2019,
                                 2016,
                                 2013,
                                 2010,
                                 2007,
                                 2004,
                                 2001,
                                 1997,
                                 1994,
                                 1990,
                                 1986,
                                 1982,
                                 1978,
                                 1974,
                                 1970,
                                 1965,
                                 1961,
                                 1956,
                                 1951,
                                 1946,
                                 1941,
                                 1936,
                                 1931,
                                 1925,
                                 1920,
                                 1914,
                                 1909,
                                 1903,
                                 1897,
                                 1891,
                                 1884,
                                 1878,
                                 1872,
                                 1865,
                                 1858,
                                 1852,
                                 1845,
                                 1838,
                                 1831,
                                 1823,
                                 1816,
                                 1809,
                                 1801,
                                 1793,
                                 1785,
                                 1778,
                                 1770,
                                 1761,
                                 1753,
                                 1745,
                                 1736,
                                 1728,
                                 1719,
                                 1710,
                                 1702,
                                 1693,
                                 1683,
                                 1674,
                                 1665,
                                 1656,
                                 1646,
                                 1636,
                                 1627,
                                 1617,
                                 1607,
                                 1597,
                                 1587,
                                 1577,
                                 1566,
                                 1556,
                                 1546,
                                 1535,
                                 1524,
                                 1514,
                                 1503,
                                 1492,
                                 1481,
                                 1470,
                                 1458,
                                 1447,
                                 1436,
                                 1424,
                                 1412,
                                 1401,
                                 1389,
                                 1377,
                                 1365,
                                 1353,
                                 1341,
                                 1329,
                                 1317,
                                 1304,
                                 1292,
                                 1279,
                                 1267,
                                 1254,
                                 1241,
                                 1229,
                                 1216,
                                 1203,
                                 1190,
                                 1177,
                                 1163,
                                 1150,
                                 1137,
                                 1123,
                                 1110,
                                 1096,
                                 1083,
                                 1069,
                                 1055,
                                 1042,
                                 1028,
                                 1014,
                                 1000,
                                 986,
                                 972,
                                 957,
                                 943,
                                 929,
                                 914,
                                 900,
                                 886,
                                 871,
                                 856,
                                 842,
                                 827,
                                 812,
                                 798,
                                 783,
                                 768,
                                 753,
                                 738,
                                 723,
                                 708,
                                 693,
                                 678,
                                 663,
                                 647,
                                 632,
                                 617,
                                 601,
                                 586,
                                 571,
                                 555,
                                 540,
                                 524,
                                 509,
                                 493,
                                 477,
                                 462,
                                 446,
                                 430,
                                 415,
                                 399,
                                 383,
                                 367,
                                 351,
                                 336,
                                 320,
                                 304,
                                 288,
                                 272,
                                 256,
                                 240,
                                 224,
                                 208,
                                 192,
                                 176,
                                 160,
                                 144,
                                 128,
                                 112,
                                 96,
                                 80,
                                 64,
                                 48,
                                 32,
                                 16,
                                 0
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
    state_flag = WAVE_SIN;
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
    P1->OUT |= BIT0;
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
                max_step = 399;
                break;
            case WAVE_SAW:
                level = (int)(step*10.25);
                max_step = 799;
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

    P1->OUT &= ~BIT0;

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

