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

struct NS_REPEAT {
    uint8_t NS;
    uint8_t repeat;
};

struct NS_REPEAT LOCKED_LOGIC(void)
{
    static uint8_t digit_num = 0;
    static uint8_t digit_check = 0;
    uint8_t repeat = 0;
    uint8_t NS;

    if (digit_num == 0){
        LCD_CLR();
        LCD_HOME();
        WRITE_STR_LCD("LOCKED");
        SET_CUR_POS_LCD(0x40);
        WRITE_STR_LCD("ENTER KEY:");
    }
    if (digit != 0xFF){
        WRITE_CHAR_LCD(digit);
        digit_check += CHECK_KEY(digit, digit_num);
        digit_num++;
    }
    if (digit_num > 3 && digit_check == 4){
      //  __delay_cycles(1000000);
        digit_num = 0;
        digit_check = 0;
        digit = 0xFF;
        NS = UNLOCKED;
        repeat = 1;
    }else if(digit_num > 3){
       // __delay_cycles(1000000);
        LCD_CLR();
        LCD_HOME();
        digit_num = 0;
        digit_check = 0;
        digit = 0xFF;
        NS = LOCKED;
        repeat = 1;
    }else{
        NS = LOCKED;
    }
    struct NS_REPEAT ret = {NS, repeat};
    return ret;
}

struct NS_REPEAT UNLOCKED_LOGIC(void)
{
    uint8_t repeat = 0;
    uint8_t NS;
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("HELLO WORLD!");
    if(digit == 42){
        NS = CLEAR;
        repeat = 1;
    }else{
        NS = UNLOCKED;
    }
    struct NS_REPEAT ret = {NS, repeat};
    return ret;
}

struct NS_REPEAT CLEAR_LOGIC(void)
{
    uint8_t repeat = 0;
    uint8_t NS;
    LCD_CLR();
    digit = 0xFF;
    NS = LOCKED;
    repeat = 1;
    struct NS_REPEAT ret = {NS, repeat};
    return ret;
}

void LOCK_FSM()
{
    static uint8_t PS = LOCKED;
    uint8_t NS;
    uint8_t repeat = 1;
    struct NS_REPEAT ret;
    while(repeat != 0){
        repeat = 0;
        switch(PS){
            case LOCKED:
                ret = LOCKED_LOGIC();
                NS = ret.NS;
                repeat = ret.repeat;
                break;
            case UNLOCKED:
                ret = UNLOCKED_LOGIC();
                NS = ret.NS;
                repeat = ret.repeat;
                break;
            case CLEAR:
                ret = CLEAR_LOGIC();
                NS = ret.NS;
                repeat = ret.repeat;
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
    INIT_KEYPAD();
    digit = 0xFF;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    __enable_irq();
    while(1);
}

void PORT5_IRQHandler(void)
{
    digit = GET_CHAR_KEYPAD();
    LOCK_FSM();
    P5 -> IFG  &= ~(ROW1 + ROW2 + ROW3 + ROW4);
}

