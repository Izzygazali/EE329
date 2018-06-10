/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/15/2018
 * Description: A program that implements an electronic lock system. The user
 *              is prompted for a key. If the key matches the correct
 *              password, "HELLO WORLD!" is displayed on the LCD. Otherwise,
 *              the prompt is displayed again. The keypad and FSM used in this
 *              system are both interrupt based.
 */

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

    //Prompt user to enter a key
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("LOCKED");
    SET_CUR_POS_LCD(0x40);
    WRITE_STR_LCD("ENTER KEY:");

    //Put the MCU to sleep to save power between interrupts
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk; // Do not wake up on exit from Int
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    //Ensure that SLEEPDEEP occurs immediately
    __DSB();
    __sleep();

}

//ISR handler for port 5 (keypad ISR)
void PORT5_IRQHandler(void)
{
    //If an interrupt is detected from the specified ports, get the digit
    //pressed and call the finite state machine.
    //digit is defined as a global variable to enable sharing between
    //multiple files.
    if(P5 ->IFG & (ROW1 + ROW2 + ROW3 + ROW4))
    {
        digit = GET_CHAR_KEYPAD();
        LOCK_FSM();
        P5 -> IFG  &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    }
}

