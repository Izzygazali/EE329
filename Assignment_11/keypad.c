/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/16/2018
 */
#include "keypad.h"

//variable definitions
uint8_t flags = 0;          //set when a input is entered on the keypad
uint8_t input = 0;          //value of the key pressed on keypad

/*
 * Function that returns the value of the input entered on the keypad
 * Inputs       NONE
 * Return       uint8_t input
 */
uint8_t get_input(void)
{
    return input;
}

/*
 * Function that returns the value of the flags variable
 * Inputs       NONE
 * Return       uint8_t flags
 */
uint8_t get_flags(void)
{
    return flags;
}

/*
 * Function that sets the value of the flags variable
 * Inputs       uint8_t num = value to set flags to
 * Return       NONE
 */
void set_flags(uint8_t num)
{
    flags = num;
    return;
}


/*
 * Function that initializes the keypad. Rows are set
 * to ports 5.4-5.7. Columns are set to ports 3.5-3.7
 * Inputs: None
 * Return: None
 */
void INIT_KEYPAD(void)
{
    //set ports 3.5 -3.7 for columns and drive columns high
    P3 -> SEL0 &= ~(COL1 + COL2 + COL3);
    P3 -> SEL1 &= ~(COL1 + COL2 + COL3);
    P3 ->DIR  |=  (COL1 + COL2 + COL3);
    P3 ->OUT  |=  (COL1 + COL2 + COL3);

    //set ports 5.4-5.7 as inputs for rows and enable pull down resistors
    P5 -> SEL0 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> SEL1 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> REN  |=  (ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> OUT  &=  ~(ROW1 + ROW2 + ROW3 + ROW4);

    //enable port 5 interrupts, interrupt on low to high transitions
    P5 -> IE   |=  (ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> IES  &=  ~(ROW1 + ROW2 + ROW3 + ROW4);
    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);

    //clear interrupt flags
    P5 -> IFG  &=  ~(ROW1 + ROW2 + ROW3 + ROW4);
    return;
}

/*
 * Function that returns the ASCII value of the current key
 * pressed on the keypad.
 * Inputs: None
 * Return: uint8_t, ASCII value of the key pressed
 */
uint8_t GET_CHAR_KEYPAD(void)
{
    //initialize variables
    uint8_t col, row, key;

    //set ports 3.5 -3.7 as outputs and
    //drive columns high
    P3 -> DIR  |= (COL1 + COL2 + COL3);
    P3 -> OUT  |= (COL1 + COL2 + COL3);
    __delay_cycles(50);

    // read inputs from rows 1 - 4
    row = P5 -> IN & (ROW1 + ROW2 + ROW3 + ROW4);

    //If all rows are low, no button press.
    if (row == 0) {return NO_KEY_PRESS;}

    //Step through columns to see which rows are on
    for(col = 0; col < 3; col++)
    {
        P3 -> OUT  &= ~(COL1 + COL2 + COL3);
        P3 -> OUT  |= (COL1 << col);
        __delay_cycles(50);
        row = (P5 -> IN & (ROW1 + ROW2 + ROW3 + ROW4))>>4;
        if (row != 0) break;
    }
    //Clean up. Make sure columns are still driven high to enable
    //interrupt detection
    P3 -> OUT  |= (COL1 + COL2 + COL3);
    P3 -> DIR  |= (COL1 + COL2 + COL3);

    //If number of columns is 3, that means no rows were in the
    //for loop, return no key pressed (-1).
    if (row == 3) {return NO_KEY_PRESS;}

    //If BIT2 of rows is high, set rows to row 3
    //If BIT3 of rows is high, set rows to row 4
    if (row == 4) row = 3;
    if (row == 8) row = 4;

    //formula for determining the ASCII values of 0-9
    key = 3*row + 46 + col;

    if (key == 58) {key = '*';}
    if (key == 59) {key = '0';}
    if (key == 60) {key = '#';}
    return key;
}


//Port 5 interrupt service routine
void PORT5_IRQHandler(void)
{
    if(P5->IFG & (ROW1 + ROW2 + ROW3 + ROW4))
    {
        input = GET_CHAR_KEYPAD();
        flags = 1;
    }
    P5->IFG &= ~(ROW1 + ROW2 + ROW3 + ROW4);
}
