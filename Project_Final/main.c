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
  //  while((get_gps_flags() & gps_ack_flag) == 0)
 //       reset_gps_odometer();
    NVIC->ICER[0] = 1 << ((EUSCIA2_IRQn) & 31);\
    char * p;
    p = alt_to_string();
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);


    while(1){
        uint32_t lat = get_curr_lat();
        uint32_t lon = get_curr_lon();
        uint32_t tow = get_curr_tow();
        uint32_t dist = get_curr_dist();
        uint32_t diff = get_diff_tow();

        lcd_state_decode();
        lcd_FSM();
    }
}
