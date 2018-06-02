#include "msp.h"
#include "gps.h"
#include "lcd_control.h"
#include "LCD.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    LCD_init();
    init_buttons();
    LCD_HOME();
    LCD_CLR();
    WRITE_STR_LCD("Invalid GPS...");
    init_GPS();
    while((get_gps_flags() & data_valid_flag) == 0);
    while((get_gps_flags() & gps_ack_flag) == 0)
        reset_gps_odometer();
    while(1){
        uint32_t lat = get_curr_lat();
        uint32_t lon = get_curr_lon();
        uint32_t tow = get_curr_tow();
        uint32_t dist = get_curr_dist();
        lcd_state_decode();
        lcd_FSM();
    }
}

