#include "msp.h"

/*Define integer associated with the various availble DCO frequencies.*/
/*This function will also set MCLK to the DCO.*/
#define RX BIT2
#define TX BIT3

/*Define functions implemented in DAC.c*/
uint16_t get_gps_flags(void);
void reset_gps_flags(void);
void gps_state_decode(void);
void gps_FSM(void);
void init_GPS(void);
void reset_gps_odometer(void);
