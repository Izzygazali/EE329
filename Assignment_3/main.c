#include "msp.h"
#include "delay.h"
#include "LCD.h"

void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    set_DCO(FREQ_48_MHz);
    LCD_init();
    LCD_Write_String("Hello World!");
    delay_ms(2000, FREQ_48_MHz);
    Return_Home();
    Clear_LCD();
    delay_ms(2000, FREQ_48_MHz);
    LCD_Write_String("hello|nworld");

}
