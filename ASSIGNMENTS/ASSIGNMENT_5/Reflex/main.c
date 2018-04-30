#include "msp.h"
#include "LCD.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CCR0_COUNT 3000
uint32_t wraps = 0;

void INIT_BUTTONS()
{
    P1   ->DIR &=  ~(BIT1+BIT4);
    P1   ->REN |=  BIT1 + BIT4;
    P1   ->OUT |= (BIT1+BIT4);
    P1   ->IE  |=  BIT1 + BIT4;
    P1   ->IES |=  (BIT1 + BIT4);
    NVIC ->ISER[1] = 1 << ((PORT1_IRQn) & 31);
    P1   ->IFG  &=  ~(BIT1+BIT4);
    return;
}

void INIT_TIMER()
{
       CS ->KEY = CS_KEY_VAL;
       CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;
       CS ->KEY = 0;
       TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
       TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
       TIMER_A0->CCR[0] = CCR0_COUNT;
       TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;
       NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
       return;
}


void WRITE_INT_LCD(uint32_t number)
{
    char int_str[20];
    sprintf(int_str, "%d", number);
    WRITE_STR_LCD(int_str);
    return;
}



int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    INIT_BUTTONS();
    INIT_TIMER();
    set_DCO(1);
    LCD_init();
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Reflex Game");
    __delay_cycles(10000000);
    LCD_CLR();
    LCD_HOME();
    __enable_irq();
    while(1);
}


void PORT1_IRQHandler(void)
{
    static uint8_t press_flag = 0;
    if (P1->IFG & BIT1){
        LCD_CLR();
        wraps = 0;
        WRITE_STR_LCD("Pressed!");
        press_flag = 1;
    }
    if (P1->IFG & BIT4 & (press_flag << 4)){
        LCD_CLR();
        LCD_HOME();
        WRITE_INT_LCD(wraps);
        WRITE_STR_LCD("ms");
        press_flag = 0;
    }
    P1 -> IFG &= ~(BIT1+BIT4);
}
EUSCI_B_CTLW0_CKPL
void TA0_0_IRQHandler(void)
{
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        wraps++;
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}





