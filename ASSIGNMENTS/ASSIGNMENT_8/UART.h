#include "msp.h"

/*Define integer associated with the various availble DCO frequencies.*/
/*This function will also set MCLK to the DCO.*/
#define RX BIT2
#define TX BIT3

/*Define functions implemented in DAC.c*/
void init_UART(void);
uint8_t get_value_flag(void);
uint16_t get_uart_value(void);
void reset_value_flag(void);


