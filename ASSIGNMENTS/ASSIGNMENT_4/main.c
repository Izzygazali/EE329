#include "msp.h"
#include "keypad.h"




void main(void)
{

    P2 -> SEL0 &= ~(LEDR + LEDG + LEDB);
    P2 -> SEL1 &= ~(LEDR + LEDG + LEDB);
    P2 -> DIR  |= (LEDR + LEDG + LEDB);

    int oldKey = 0;
    LCD_init();
    LCD_CLR();
    INIT_KEYPAD();
    WDTCTL = WDTPW | WDTHOLD;
    while(1){
        uint8_t key = GET_CHAR_KEYPAD();
        if (key != oldKey && key != 0xFF){
            LCD_CLR();
            LCD_HOME();
            WRITE_CHAR_LCD(key);
            P2->OUT = key & (LEDR + LEDG + LEDB);
            oldKey = key;
        }else if(key == 0xFF){
            LCD_CLR();
            LCD_HOME();
            P2->OUT &= ~(LEDR + LEDG + LEDB);
            oldKey = key;
        }
        __delay_cycles(5000);
    }

}
