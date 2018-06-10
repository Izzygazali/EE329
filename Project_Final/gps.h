/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 05/22/2018
 * Description: This library file implements a FSM
 * for parsing gps data over UART using the "UBX"
 * data format. Additionally, the file implements
 * other functions used to change operating parameters
 * on the gps.
 *
 * Pin Assignments: P3.2 -> GPS TX/MCU RX
 *                  P3.3 -> GPS RX/MCU TX
 */

#include "msp.h"

//define pin numbers used for UART RX and TX
#define RX BIT2
#define TX BIT3

//define bits used as flags in GPS parser
//BIT 0 - flag is set when gps data is determined to be valid
//BIT 1 - flag is set when new gps data is processed
//BIT 2 - set when the gps acknoledges a transmission from
//        sent from the MCU
#define data_valid_flag BIT0
#define new_data_flag BIT1
#define gps_ack_flag BIT2

//Define functions implemented in gps.c
uint16_t get_gps_flags(void);
void reset_gps_flags(uint16_t flags);
void gps_state_decode(void);
void gps_FSM(void);
void init_GPS(void);
void reset_gps_odometer(void);
uint32_t get_curr_lat(void);
uint32_t get_curr_lon(void);
uint32_t get_curr_tow(void);
uint32_t get_diff_tow(void);
uint32_t get_curr_dist(void);
uint16_t get_curr_year(void);
uint8_t get_curr_month(void);
uint8_t get_curr_day(void);
uint8_t get_curr_hour(void);
uint8_t get_curr_minute(void);
uint8_t get_curr_second(void);
uint32_t get_curr_speed(void);
void reset_tow(void);
