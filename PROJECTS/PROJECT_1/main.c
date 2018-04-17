#include "keypad_lock.h"

void main(void)
{
    //Disable watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    //Set clock frequency and initialize LCD
    set_DCO(FREQ_3_MHz);
    LCD_init();

    //Initialize keypad and set digit value to NO_KEY_PRESS
    INIT_KEYPAD();
    digit = NO_KEY_PRESS;

    //Enable Interrupts
    __enable_irq();

    //GPIO settings for LED
    P1 ->SEL0 &= ~BIT0;
    P1 ->SEL0 &= ~BIT0;
    P1 ->DIR |= BIT0;

    //LED is initially turned off
    P1 ->OUT &= ~BIT0;

    //Prompt user to enter a key
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("LOCKED");
    SET_CUR_POS_LCD(0x40);
    WRITE_STR_LCD("ENTER KEY:");

    //Blink LED
    while(1)
    {
        __delay_cycles(1000000);
        P1 ->OUT ^= BIT0;
    }
}

//ISR handler for port 5
void PORT5_IRQHandler(void)
{
    if(P5 ->IFG & (ROW1 + ROW2 + ROW3 + ROW4))
    {
        digit = GET_CHAR_KEYPAD();
        LOCK_FSM();
        P5 -> IFG  &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    }
}

