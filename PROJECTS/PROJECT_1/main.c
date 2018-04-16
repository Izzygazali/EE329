#include "msp.h"
#include "delay.h"
#include "LCD.h"
#include "keypad.h"
#include <stdio.h>

/* States
 * 0 => LOCKED
 * 1 => INPUTTING PASS
 * 2 => WRONG PASS
 * 3 => RIGHT PASS
 */

#define LOCKED 0
#define UNLOCKED 1
#define CLEAR 2
#define CLR_KEY 0x1234

#define ROW1 BIT4
#define ROW2 BIT5
#define ROW3 BIT6
#define ROW4 BIT7

#define COL1 BIT5
#define COL2 BIT6
#define COL3 BIT7

#define LEDR BIT0
#define LEDG BIT1
#define LEDB BIT2

const uint8_t CORR_KEY[] = {49,50,51,52};


uint8_t CHECK_KEY(uint8_t digit, uint8_t digit_num)
{
    if (CORR_KEY
            [digit_num] == digit){
        return 1;
    }else{
        return 0;
    }
}


void LOCK_FSM()
{
    static uint8_t PS = LOCKED;
    static uint8_t digit_num = 0;
    static uint8_t digit_check = 0;
    uint8_t NS;
    _Bool repeat = 1;

    while(repeat != 0){
        repeat = 0;
        switch(PS){
            case LOCKED:
                if (digit != 0xFF){
                    WRITE_CHAR_LCD(digit);
                    digit_check += CHECK_KEY(digit, digit_num);
                    digit_num++;
                }
                if (digit_num > 3 && digit_check == 4){
                    digit_num = 0;
                    NS = UNLOCKED;
                    repeat = 1;
                }else if(digit_num > 3){
                    LCD_CLR();
                    LCD_HOME();
                    digit_num = 0;
                    NS = LOCKED;
                }else{
                    NS = LOCKED;
                }
                break;
            case UNLOCKED:
                NS = UNLOCKED;
                P1 ->OUT |= BIT0;

                break;
            case CLEAR:
                NS = LOCKED;
                break;
            default:
                NS = LOCKED;
                break;
        }
        PS = NS;
    }
    return;
}


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P1 ->DIR |= BIT0;
    P1 ->OUT &= ~BIT0;
    set_DCO(FREQ_3_MHz);
    LCD_init();
    LCD_CLR();
    INIT_KEYPAD();
   // int oldKey = 60;
    digit = 0xFF;
    __enable_irq();
    while(1);
}

void PORT5_IRQHandler(void)
{
    digit = GET_CHAR_KEYPAD();

    LOCK_FSM();

    __delay_cycles(100000);
    P5 -> IFG  &= ~(ROW1 + ROW2 + ROW3 + ROW4);
}

