#include "msp.h"
#include "gps.h"
#include "lcd_control.h"
#include "LCD.h"
#include "string_conv.h"
#include "Altimeter.h"
#include "sd.h"


enum state_type{
    start_system,
    idle,
    start_log,
    log,
    end_log
};

enum state_type sys_state = start_system;


void sys_state_decode(void)
{
    switch(sys_state)
    {
        case idle:
            if (get_lcd_flags() & lcd_state_flag)
                sys_state = start_log;
            break;
        case log:
            if ((get_lcd_flags() & lcd_state_flag) == 0)
                sys_state = end_log;
            break;
        default:
            break;
    }
    return;
}

void sys_FSM(void)
{
    switch(sys_state)
    {
        case start_system:
            LCD_init();
            init_GPS();
            init_buttons();
            LCD_HOME();
            LCD_CLR();
            Init_I2C(MPL3115A2_ADDRESS);
            Init_MPL3115A2();
            set_sea_pressure(101172);
            WRITE_STR_LCD("Invalid GPS...");
            while((get_gps_flags() & data_valid_flag) == 0);
            sys_state = idle;
        case idle:
            set_lcd_flags(lcd_screen_flag);
            lcd_state_decode();
            lcd_FSM();
            __delay_cycles(24000000);
            break;
        case start_log:
            while((get_gps_flags() & gps_ack_flag) == 0)
                reset_gps_odometer();
            reset_tow();
            init_log_file();
            write_header();
            sys_state = log;
            break;
        case log:
            set_lcd_flags(lcd_screen_flag);
            lcd_state_decode();
            lcd_FSM();
            __delay_cycles(24000000);
            write_gps_coord();
            break;
        case end_log:
            end_log_file();
            close_log_file();
            sys_state = idle;
            break;
        default:
            sys_state = start_system;
    }
    return;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    init_log_file();
    write_header();
    end_log_file();
    close_log_file();
    while(1);
    while(1){
        sys_state_decode();
        sys_FSM();
    }
}
