/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/24/2018 */
#include "msp.h"
#include "delay.h"

//define bits associated with inputs used for SPI
#define CHIPSEL BIT0
#define SCLK    BIT5
#define SIMO    BIT6
#define SOMI    BIT7

//define control bits used for interfacing with the DAC
#define SHDN BIT4
#define GAIN BIT5
//define CCR count which gives correct "sampling rate" for
//20 ms period triangle wave
#define CCR0_COUNT 150
#define CCR1_COUNT 75

/*Define functions implemented in DAC.c*/
void SPI_INIT(void);
void WRITE_DAC(uint16_t data);
void INIT_TIMER();
