#include "keypad_lock.h"

//Define States of the FSM
#define     LOCKED      0
#define     UNLOCKED    1
#define     CLEAR       2

//Clear key is set to he * button
#define     CLR_KEY     42

//Global variable, digit from keypad
int digit;

/*
 * Function that compares the entered key with the correct password.
 * INPUT:   entered_key = value to compare
 *          index = index of the correct password that key is compared to.
 * RETURN:  1 if key matches correct key
 *          0 otherwise.
 */
uint8_t CHECK_KEY(uint8_t entered_key, uint8_t index)
{
    if (CORRECT_KEY[index] == entered_key){return 1;}
    else{return 0;}
}


/*
 * Function that handles the logic for the LOCKED state and sets the
 * NS and repeat flag accordingly.
 * INPUT:   NONE
 * RETURN:  FSM's next state and repeat flag status.
 *          NS      = next state
 *          repeat  = 1 iterate through FSM again.
 *                    0 no iteration through FSM necessary.
 */
struct FSM_CONTROL LOCKED_LOGIC(void)
{
    //variable definitions
    static uint8_t num_digits_entered = 0;
    static uint8_t num_digits_correct = 0;
    uint8_t repeat = 0;
    uint8_t NS;

    //If no digits have been entered
    if (num_digits_entered == 0)
    {
        //Prompt user to enter a key
        LCD_CLR();
        LCD_HOME();
        WRITE_STR_LCD("LOCKED");
        SET_CUR_POS_LCD(0x40);
        WRITE_STR_LCD("ENTER KEY:");
    }

    //If a digit is entered
    if (digit != NO_KEY_PRESS && digit != CLR_KEY)
    {
        //display * in place of the entered digit
        WRITE_CHAR_LCD('*');
        //Check if the digit matches the correct key at that index, keep track
        //of the number of correct matches
        num_digits_correct += CHECK_KEY(digit, num_digits_entered);
        //keep track of the number of digits entered.
        num_digits_entered++;
    }

    //If the correct password is entered
    if (num_digits_entered > 3 && num_digits_correct == 4)
    {
        __delay_cycles(1000000);
        //Reset static variables and digit.
        num_digits_entered = 0;
        num_digits_correct = 0;
        digit = NO_KEY_PRESS;
        //Set NS to UNLOCKED and the FSM repeat flag to 1
        NS = UNLOCKED;
        repeat = 1;
    }

    //If the incorrect password is entered or the clear key is pressed.
    else if((num_digits_entered > 3) | (digit == CLR_KEY))
    {
        __delay_cycles(1000000);
        //Reset static variables and set NS to clear and FSM repeat flag to 1
        num_digits_entered = 0;
        num_digits_correct = 0;
        NS = CLEAR;
        repeat = 1;
    }

    //Catch all condition
    else{
        NS = LOCKED;
    }

    //Return the next state(NS) and the repeat flag(repeat).
    struct FSM_CONTROL ret = {NS, repeat};
    return ret;
}

/*
 * Function that handles the logic for the UNLOCKED state and sets the
 * NS and repeat flag accordingly.
 * INPUT:   NONE
 * RETURN:  FSM's next state and repeat flag status.
 *          NS      = next state
 *          repeat  = 1 iterate through FSM again.
 *                    0 no iteration through FSM necessary.
 */
struct FSM_CONTROL UNLOCKED_LOGIC(void)
{
    //variable definitions
    uint8_t repeat = 0;
    uint8_t NS;

    //Display HELLO WORLD! on the LCD
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("HELLO WORLD!");

    //If the clear key is pressed
    if(digit == CLR_KEY)
    {
        //Set NS to clear and FSM repeat flag to 1
        NS = CLEAR;
        repeat = 1;
    }
    //If the user doesn't press the clear key remain in this state
    else
    {
        NS = UNLOCKED;
    }

    //Return the next state(NS) and the repeat flag(repeat).
    struct FSM_CONTROL ret = {NS, repeat};
    return ret;
}

/*
 * Function that handles the logic for the CLEAR state and sets the
 * NS and repeat flag accordingly.
 * INPUT:   NONE
 * RETURN:  FSM's next state and repeat flag status.
 *          NS      = next state
 *          repeat  = 1 iterate through FSM again.
 *                    0 no iteration through FSM necessary.
 */
struct FSM_CONTROL CLEAR_LOGIC(void)
{
    uint8_t NS;
    //Set digit to NO_KEY_PRESS
    digit = NO_KEY_PRESS;

    //Clear the LCD and set the next state to LOCKED and the repeat flag to 1
    LCD_CLR();
    NS = LOCKED;
    uint8_t repeat = 1;

    //Return the next state(NS) and the repeat flag(repeat).
    struct FSM_CONTROL ret = {NS, repeat};
    return ret;
}



void LOCK_FSM()
{
    //Variable definitions.
    static uint8_t PS = LOCKED;
    uint8_t NS;
    uint8_t repeat = 1;
    struct FSM_CONTROL control;

    //While the repeat flag is enabled, iterate through the FSM and set
    //the present state and repeat flag accordingly
    while(repeat == 1)
    {
        repeat = 0;
        switch(PS)
        {
            case LOCKED:
                control = LOCKED_LOGIC();
                break;
            case UNLOCKED:
                control = UNLOCKED_LOGIC();
                break;
            case CLEAR:
                control = CLEAR_LOGIC();
                break;
            default:
                NS = LOCKED;
                break;
        }
        NS = control.NS;
        repeat = control.FLG_repeat;
        PS = NS;
    }
    return;
}

