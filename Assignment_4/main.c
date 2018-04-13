#include "msp.h"
#include "..\Assignment_3\LCD.h"
#include "..\Assignment_2\delay.h"
#include "..\Assignment_2\delay.c"
#include "..\Assignment_3\LCD.c"

#define ROW1 BIT4
#define ROW2 BIT5
#define ROW3 BIT6
#define ROW4 BIT7

#define COL1 BIT5
#define COL2 BIT6
#define COL3 BIT7
#define NO_KEY_PRESS 0xFF

void keypad_Init()
{
    //set ports 2.4-2.7 as inputs for rows.
    P2 -> SEL0 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P2 -> SEL1 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P2 -> DIR  &= ~(ROW1 + ROW2 + ROW3 + ROW4);

    //Enable pull down resistor be setting REN high and
    //output port low.
    P2 -> REN  |= (ROW1 + ROW2 + ROW3 + ROW4);
    P2 -> OUT  &= ~(ROW1 + ROW2 + ROW3 + ROW4);

    //set ports 3.5 -3.7 for columns
    //Initially the ports are set as inputs for safety
    P3 -> SEL0 &= ~(COL1 + COL2 + COL3);
    P3 -> SEL1 &= ~(COL1 + COL2 + COL3);
    P3 -> DIR  &= ~(COL1 + COL2 + COL3);
}


uint8_t keypad_getchar(void)
{
    //initialize variables
    uint8_t col, rows, key;

    //set ports 3.5 -3.7 as outputs and
    //drive columns high
    P3 -> DIR  |= (COL1 + COL2 + COL3);
    P3 -> OUT  |= (COL1 + COL2 + COL3);
    __delay_cycles(25);

    // read inputs from rows 1 - 4
    rows = P2 -> IN & (ROW1 + ROW2 + ROW3 + ROW4);

    //If all rows are low, no button press.
    if(rows == 0){return NO_KEY_PRESS;}

    //Step through columns to see which rows are on
    for(col = 0; col < 3; col++)
    {
        P3 -> OUT &= ~(COL1 + COL2 + COL3);
        P3 -> OUT |= (COL1 << col);
        __delay_cycles(25);
        rows = (P2 -> IN & (ROW1 + ROW2 + ROW3 + ROW4))>>4;
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

uint8_t keypad_ASCII(uint8_t key){
    uint8_t asciiVal;
    //If key is *
    if (key == 10){
        asciiVal = 42;
    }
    //If key is #
    else if(key == 12){
        asciiVal = 35;
    }
    //For 0-9, 48 is added to the key value to get the correct
    //ASCII value.
    else{
        asciiVal = key + 48;
    }
    return asciiVal;
}

void keypad_To_LCD()
{
    uint8_t key;
    while(1)
    {
        key = keypad_getchar();
        if(key != NO_KEY_PRESS)
        {
            delay_ms(250, FREQ_48_MHz);
            LCD_Write_Char(keypad_ASCII(key));
            break;
        }
    }
}

void main(void)
{
    //Disable Watchdog Timer
    WDTCTL = WDTPW | WDTHOLD;
    set_DCO(FREQ_48_MHz);
    keypad_Init();
    LCD_init();
    int i;
    for(i=0; i<30; i++){
        keypad_To_LCD();
        if(i == 15){
            LCD_newLine();
        }
    }
}
