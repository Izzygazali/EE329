#include "keypad.h"

/*
 * Program that displays the value of the key pressed on
 * the keypad and sends the lower three bits of that value
 * to the multicolor LED.
 */
void main(void)
{
    //Disable Watchdog Timer
    WDTCTL = WDTPW | WDTHOLD;

    //Set clock frequency for LCD and delay function.
    set_DCO(FREQ_48_MHz);

    //Initialize keypad and LCD.
    KEYPAD_INIT();
    LCD_init();

    //Print key to LCD, turn multicolor LED on.
    KEYPAD_TO_LCD_LED();
}
