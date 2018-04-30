#include "msp.h"
#include "delay.h"

/*Define integer associated with the various availble DCO frequencies.*/
/*This function will also set MCLK to the DCO.*/
#define FREQ_3_MHz      1
#define CHIPSEL   BIT0
#define SCLK BIT5
#define SIMO BIT6
#define SOMI BIT7
#define SHDN BIT4
#define GAIN BIT5
#define CCR0_COUNT 30150
#define CCR1_COUNT 12


/*Define functions implemented in DAC.c*/
void SPI_INIT(void);
void WRITE_DAC(uint16_t data);
void INIT_TIMER();
