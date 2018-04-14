#include "keypad.h"

/*
 * Function that initializes the keypad. Rows are set
 * to ports 2.4-2.7. Columns are set to ports 3.5-3.7
 * Inputs: None
 * Return: None
 */
void KEYPAD_INIT(void)
{
    //set ports 2.4-2.7 as inputs for rows.
    P5 -> SEL0 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> SEL1 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> DIR  &= ~(ROW1 + ROW2 + ROW3 + ROW4);

    //Enable pull down resistor be setting REN high and
    //output port low.
    P5 -> REN  |= (ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> OUT  &= ~(ROW1 + ROW2 + ROW3 + ROW4);

    //set ports 3.5 -3.7 for columns
    //Initially the ports are set as inputs for safety
    P3 -> SEL0 &= ~(COL1 + COL2 + COL3);
    P3 -> SEL1 &= ~(COL1 + COL2 + COL3);
    P3 -> DIR  &= ~(COL1 + COL2 + COL3);
}

/*
 * Function that returns the value of the current key
 * pressed on the keypad. The key value for * is 10
 * and the key value for # 12.
 * Inputs: None
 * Return: uint8_t, unsigned 8-bit integer key value
 */
uint8_t KEYPAD_GET_KEY(void)
{
    //initialize variables
    uint8_t col, rows, key;

    //set ports 3.5 -3.7 as outputs and
    //drive columns high
    P3 -> DIR  |= (COL1 + COL2 + COL3);
    P3 -> OUT  |= (COL1 + COL2 + COL3);
    __delay_cycles(25);

    // read inputs from rows 1 - 4
    rows = P5 -> IN & (ROW1 + ROW2 + ROW3 + ROW4);

    //If all rows are low, no button press.
    if(rows == 0){return NO_KEY_PRESS;}

    //Step through columns to see which rows are on
    for(col = 0; col < 3; col++)
    {
        P3 -> OUT &= ~(COL1 + COL2 + COL3);
        P3 -> OUT |= (COL1 << col);
        __delay_cycles(25);
        rows = (P5 -> IN & (ROW1 + ROW2 + ROW3 + ROW4))>>4;
        if(rows != 0){break;}
    }

    //Clean up register. Set ports as inputs for safety
    P3 -> OUT &= ~(COL1 + COL2 + COL3);
    P3 -> DIR &= ~(COL1 + COL2 + COL3);

    //If number of columns is 3, that means no rows were
    //In the for loop, return no key pressed (-1).
    if(col == 3) {return NO_KEY_PRESS;}

    //If BIT2 of rows is high, set rows to row 3
    //If BIT3 of rows is high, set rows to row 4
    if (rows == 4 ){rows = 3;}
    if (rows == 8 ){rows = 4;}

    //formula for determining the values of the button pressed
    //10 is the * key, and 12 is the # key.
    key = 3*rows + col -2;
    if(key == 11){key = 0;}
    return key;
}
/*
 * Function that takes in the keypad key value and returns
 * the corresponding ASCII value of the key.
 * Inputs: uint8_t, unsigned 8-bit integer key value
 * Return: uint8_t, unsigned 8-bit integer ASCII value of key
 */
uint8_t KEYPAD_ASCII(uint8_t key)
{
    uint8_t asciiVal;
    //If key is *
    if (key == 10){asciiVal = 42;}

    //If key is #
    else if(key == 12){asciiVal = 35;}

    //For 0-9, 48 is added to the key value to get the correct
    //ASCII value.
    else{asciiVal = key + 48;}

    return asciiVal;
}

/*
 * Function that waits for a button press on the keypad and
 * outputs the key value to the LCD.
 * Inputs: None
 * Return: None
 */
void KEYPAD_TO_LCD()
{
    uint8_t key;
    while(1)
    {
        key = KEYPAD_GET_KEY();
        //If key press detected, output to LCD.
        if(key != NO_KEY_PRESS)
        {
            delay_ms(250, FREQ_48_MHz);
            LCD_Write_Char(KEYPAD_ASCII(key));
            break;
        }
    }
}

/*
 * Function that displays the value of the keypad button
 * pressed on the LCD and outputs the lower 3 bits of the
 * key value to the multicolor LED.
 * BIT0 => RED, BIT1 => GREEN, and BIT2 => BLUE
 * Inputs: None
 * Return: None
 */
void KEYPAD_TO_LCD_LED()
{
    //Variable definitions
    uint8_t key, oldKey;

    //Initialize the output ports for driving the LED
    P2 -> SEL0 &= ~(RED_LED + GREEN_LED + BLUE_LED);
    P2 -> SEL1 &= ~(RED_LED + GREEN_LED + BLUE_LED);
    P2 -> DIR  |= (RED_LED + GREEN_LED + BLUE_LED);

    while(1)
    {
        key = KEYPAD_GET_KEY();
        //If there is a key press, display the pressed key on
        //the LCD, and turn on the LED with the corresponding
        //color.
        if(key != NO_KEY_PRESS && key != oldKey)
        {
            Clear_LCD();
            Return_Home();
            LCD_Write_Char(KEYPAD_ASCII(key));
            P2->OUT = (key & (RED_LED + GREEN_LED + BLUE_LED));
            oldKey = key;
        }
        //If no key press detected clear the LCD and turn off
        //the LED,
        else if( key == NO_KEY_PRESS)
        {
            Clear_LCD();
            Return_Home();
            P2->OUT &= ~(RED_LED + GREEN_LED + BLUE_LED);
            oldKey = key;
        }
        __delay_cycles(5000);
    }
}
