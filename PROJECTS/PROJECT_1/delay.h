#include "msp.h"

/*Define integer associated with the various availble DCO frequencies.*/
/*This function will also set MCLK to the DCO.*/
#define FREQ_1_5_MHz    0
#define FREQ_3_MHz      1
#define FREQ_6_MHz      2
#define FREQ_12_MHz     3
#define FREQ_24_MHz     4
#define FREQ_48_MHz     5

/*Define functions implmented in delay.c*/
void set_DCO(int freq);			//freq: sets DCO to desired freqency using defined frequency values.
void delay_us(float time_us, int freq);	//time_us: time to delay in microseconds, freq: the current DCO frequency that is set.
void delay_ms(float time_ms, int freq);	//time_ms: time to delay in milliseconds, freq: the current DCO frequency that is set.
