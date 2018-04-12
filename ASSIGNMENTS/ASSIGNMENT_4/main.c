#include "msp.h"
#include "keypad.h"
#include "../ASSIGNMENT_3/LCD.h"
#include <stdio.h>

/* P2.4 -> ROW1
 * P2.5 -> ROW2
 * P2.6 -> ROW3
 * P2.7 -> ROW4
 * P3.5 -> COL1
 * P3.6 -> COL2
 * P3.7 -> COL3
 */

#define ROW1 BIT4
#define ROW2 BIT5
#define ROW3 BIT6
#define ROW4 BIT7

#define COL1 BIT5
#define COL2 BIT6
#define COL3 BIT7

void INIT_KEYPAD()
{
    //Set rows, P2, to input
    P2 -> SEL0 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P2 -> SEL1 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P2 -> REN  |=  (ROW1 + ROW2 + ROW3 + ROW4);
    P2 -> DIR  &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P2 -> OUT  &= ~(ROW1 + ROW2 + ROW3 + ROW4);

    //set cols, P3, to input
    P3 -> SEL0 &= ~(COL1 + COL2 + COL3);
    P3 -> SEL1 &= ~(COL1 + COL2 + COL3);
    P3 -> DIR  &= ~(COL1 + COL2 + COL3);
}

uint8_t GET_CHAR_KEYPAD()
{
    uint8_t col, row, key;
    P3 -> DIR  |= (COL1 + COL2 + COL3);
    P3 -> OUT  |= (COL1 + COL2 + COL3);
    __delay_cycles(50);
    row = P2 -> IN & (ROW1 + ROW2 + ROW3 + ROW4);
    if (row == 0) return 0xFF;
    else
    {
        for(col = 0; col < 3; col++)
        {
            P3 -> OUT  &= ~(COL1 + COL2 + COL3);
            P3 -> OUT  |= (COL1 << col);
            __delay_cycles(50);
            row = (P2 -> IN & (ROW1 + ROW2 + ROW3 + ROW4))>>4;
            if (row != 0) break;
        }
        P3 -> OUT  &= ~(COL1 + COL2 + COL3);
        P3 -> DIR  &= ~(COL1 + COL2 + COL3);
        if (row == 3) return 0xFF;
        if (row == 4) row = 3;
        if (row == 8) row = 4;
        key = 3*row + 46 + col;
        if (key == 58) key = '*';
        if (key == 59) key = '0';
        if (key == 60) key = '#';
        return key;
    }
}


void main(void)
{
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
            oldKey = key;
        }
        __delay_cycles(5000);
    }

}
