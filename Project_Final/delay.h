/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/05/2018
 * Description: This library file implements useful functions
 *              for setting DCO and MCLK, as well as implementing delay
 *              functions. The functions are documented prior to their source code.
 */
#include "msp.h"

// define integers associated with the various available DCO frequencies.
#define FREQ_1_5_MHz    0
#define FREQ_3_MHz      1
#define FREQ_6_MHz      2
#define FREQ_12_MHz     3
#define FREQ_24_MHz     4
#define FREQ_48_MHz     5

//function prototypes
void set_DCO(int freq);
void delay_us(float time_us, int freq);
void delay_ms(float time_ms, int freq);
