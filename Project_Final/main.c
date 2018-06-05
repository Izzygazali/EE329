#include "msp.h"
#include "gps.h"
#include "lcd_control.h"
#include "LCD.h"
#include "string_conv.h"
#include "Altimeter.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
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
    while((get_gps_flags() & gps_ack_flag) == 0)
        reset_gps_odometer();



    while(1){
        set_lcd_flags(lcd_screen_flag);
        lcd_state_decode();
        lcd_FSM();
        __delay_cycles(24000000);
    }
}
