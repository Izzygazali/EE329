#include "msp.h"


/*Define integer associated with the various availble DCO frequencies.*/
/*This function will also set MCLK to the DCO.*/
#define CHIPSEL   BIT0
#define SCLK BIT5
#define SIMO BIT6
#define SOMI BIT7
#define SHDN BIT4
#define GAIN BIT5

/*Define functions implemented in DAC.c*/
void SPI_INIT(void);
void WRITE_DAC(uint16_t data);

