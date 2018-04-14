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
/*
int i;
for(i=0; i<30; i++){
    KEYPAD_TO_LCD();
    if(i == 15){
        LCD_newLine();
    }
}
*/


/*
P2 -> SEL0 &= ~(BIT0 + BIT1 + BIT2);
P2 -> SEL1 &= ~(BIT0 + BIT1 + BIT2);
P2 -> DIR  |= (BIT0 + BIT1 + BIT2);
uint8_t key;
while(1){
    key = KEYPAD_GET_KEY();
    if (key != NO_KEY_PRESS)
    {
        P2->OUT = (key & (BIT0 + BIT1 + BIT2));
        Clear_LCD();
        KEYPAD_TO_LCD();
    }
}
*/
