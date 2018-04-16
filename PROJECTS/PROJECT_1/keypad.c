#include "keypad.h"
#include <stdio.h>

int digit;

void INIT_KEYPAD(void)
{
    P3 -> SEL0 &= ~(COL1 + COL2 + COL3);
    P3 -> SEL1 &= ~(COL1 + COL2 + COL3);
    P3 ->DIR  |=  (COL1 + COL2 + COL3);
    P3 ->OUT  |=  (COL1 + COL2 + COL3);
    P5 -> SEL0 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> SEL1 &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> IE   |=  (ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> REN  |=  (ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> OUT  &=  ~(ROW1 + ROW2 + ROW3 + ROW4);

    P5 -> IES  &=  ~(ROW1 + ROW2 + ROW3 + ROW4);
    P5 -> IFG  &=  ~(ROW1 + ROW2 + ROW3 + ROW4);
    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);
    return;
}

uint8_t GET_CHAR_KEYPAD(void)
{
    uint8_t col, row, key;
    P3 -> DIR  |= (COL1 + COL2 + COL3);
    P3 -> OUT  |= (COL1 + COL2 + COL3);
    __delay_cycles(50);
    row = P5 -> IN & (ROW1 + ROW2 + ROW3 + ROW4);
    if (row == 0) return 0xFF;
    else
    {
        for(col = 0; col < 3; col++)
        {
            P3 -> OUT  &= ~(COL1 + COL2 + COL3);
            P3 -> OUT  |= (COL1 << col);
            __delay_cycles(50);
            row = (P5 -> IN & (ROW1 + ROW2 + ROW3 + ROW4))>>4;
            if (row != 0) break;
        }
        P3 -> OUT  |= (COL1 + COL2 + COL3);
        P3 -> DIR  |= (COL1 + COL2 + COL3);
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

