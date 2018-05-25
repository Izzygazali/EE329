#include "msp.h"

/*Define integer associated with the various availble DCO frequencies.*/
/*This function will also set MCLK to the DCO.*/
#define RX BIT2
#define TX BIT3

/*Define functions implemented in DAC.c*/
uint8_t get_gps_char(void);
uint16_t get_gps_flag(void);
void reset_gps_flag(void);
void init_GPS(void);
