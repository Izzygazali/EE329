/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/10/2018
 * Description: This library file implements for setting MSP432 DCO clock and
 *              generating software delays. The functions are documented prior
 *              to their source code.
 */
#include "msp.h"
//Define integers associated with the various available DCO frequencies.

#define FREQ_1_5_MHz    0
#define FREQ_3_MHz      1
#define FREQ_6_MHz      2
#define FREQ_12_MHz     3
#define FREQ_24_MHz     4
#define FREQ_48_MHz     5

//prototypes for functions implemented in delay.c
void set_DCO(int freq);
void delay_us(float time_us, int freq);
void delay_ms(float time_ms, int freq);
