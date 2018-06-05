#include "msp.h"

/*Define integer associated with the various availble DCO frequencies.*/
/*This function will also set MCLK to the DCO.*/
#define RX BIT2
#define TX BIT3
#define data_valid_flag BIT0
#define new_data_flag BIT1
#define gps_ack_flag BIT2

/*Define functions implemented in DAC.c*/
uint16_t get_gps_flags(void);
void reset_gps_flags(uint16_t flags);
void gps_state_decode(void);
void gps_FSM(void);
void init_GPS(void);
void reset_gps_odometer(void);
uint32_t get_curr_lat(void);
uint32_t get_curr_lon(void);
uint32_t get_curr_speed(void);
uint32_t get_curr_tow(void);
uint32_t get_diff_tow(void);
uint32_t get_curr_dist(void);
uint16_t get_curr_year(void);
uint8_t get_curr_month(void);
uint8_t get_curr_day(void);
uint8_t get_curr_hour(void);
uint8_t get_curr_minute(void);
uint8_t get_curr_second(void);
