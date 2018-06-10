/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 05/25/2018
 * Description: This main function implments the primary logic
 * which orchestrates the several library files such that the
 * gps logging/display system operates as described in the owner's
 * manual.
 */

//include necassary library files
#include "msp.h"
#include "gps.h"
#include "lcd_control.h"
#include "LCD.h"
#include "string_conv.h"
#include "Altimeter.h"
#include "sd.h"

//define states for the overall system FSM
enum state_type{
    //initalize system components
    start_system,
    //display data and wait for user to start logging
    idle,
    //perform operations to begin logging gps data
    start_log,
    //log gps data and wait for user to stop logging
    log,
    //perform final operations to stop logging
    end_log
};

//set initial state to start system components
enum state_type sys_state = start_system;
/*
 * State decoder for FSM to determine which states to transistion to
 * depending on user input from lcd_control.c
 * INPUTS   NONE
 * RETURN   NONE
 */
void sys_state_decode(void)
{
    //decode the next state based on the current state
    switch(sys_state)
    {
        //if system is idle and the lcd_state_flag indicates user has started logging
        //go to start log state
        case idle:
            if (get_lcd_flags() & lcd_state_flag)
                sys_state = start_log;
            break;
        //if system is logging and the lcd_state_flag indicates user has stopped logging
        //go to end log state
        case log:
            if ((get_lcd_flags() & lcd_state_flag) == 0)
                sys_state = end_log;
            break;
        default:
            //don't change state by default
            break;
    }
    return;
}
/*
 * State machine which implments overall logic for the gps logging/display
 * system
 * INPUTS   NONE
 * RETURN   NONE
 */
void sys_FSM(void)
{
    //switch based on current system state
    switch(sys_state)
    {
        case start_system:
            //enable interuppts globally
            __enable_irq();
            //intialize LCD module
            LCD_init();
            LCD_HOME();
            LCD_CLR();
            //intialize gps module
            init_GPS();
            //intilize interupt based buttons for user input
            init_buttons();
            //intialize i2c module and the MPL3115A2 module
            Init_I2C(MPL3115A2_ADDRESS);
            Init_MPL3115A2();
            //set sea pressure to 101372 Pa
            set_sea_pressure(101372);
            //intialize timer which controls LCD refresh rate
            LCD_update_timer();
            //indicate gps data is currently invalid
            WRITE_STR_LCD("Invalid GPS...");
            //wait until gps module has indicated that gps data is valid
            while((get_gps_flags() & data_valid_flag) == 0);
            //transition to idle state
            sys_state = idle;
        case idle:
            //call lcd decode and FSM to update display with new data
            lcd_state_decode();
            lcd_FSM();
            break;
        case start_log:
            //reset odometer on loop until module acknowledges it has reset
            while((get_gps_flags() & gps_ack_flag) == 0)
                reset_gps_odometer();
            //reset the old time of week to store time log starts
            reset_tow();
            //intialize log file on the sd card
            init_log_file();
            __delay_cycles(100*CYCLES);
            //write the XML header required for GPX formatted files
            write_header();
            //proceed to log state
            sys_state = log;
            break;
        case log:
            //update LCD with new information
            lcd_state_decode();
            lcd_FSM();
            //if the log flag is set save current gps data to log
            if (get_lcd_flags() & lcd_log_flag){
                //write new gps data to log file
                write_gps_coord();
                //reset log file to prevent excessive logging
                reset_lcd_flags(lcd_log_flag);
            }
            break;
        case end_log:
            //write final lines required for GPX formatted files
            end_log_file();
            __delay_cycles(100*CYCLES);
            //close the log file concluding write
            close_log_file();
            //return to idle state
            sys_state = idle;
            break;
        default:
            //catch all, start system
            sys_state = start_system;
    }
    return;
}
/*
 * main function which keeps the system running by polling the overall
 * system FSM
 * INPUTS   NONE
 * RETURN   NONE
 */
int main(void)
{
    //stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    //loop indefintiely
    while(1){
        //poll decoder and FSM for the overall system
        sys_state_decode();
        sys_FSM();
    }
}
