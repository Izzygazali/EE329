#include "msp.h"
#include "LCD.h"
#include "../ASSIGNMENT_2/delay.h"

void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    set_DCO(FREQ_48_MHz);
    LCD_init();
    LCD_CLR();
    WRITE_STR_LCD("Tyler is cooler");
    SET_CUR_POS_LCD(0x40);
    WRITE_STR_LCD("than Izzy! :D");


    P1 ->SEL0 &= ~BIT0;
    P1 ->SEL1 &= ~BIT1;
    P1 ->DIR |= BIT0;

    while (1){
       P1 ->OUT ^= BIT0;
       delay_ms(500, FREQ_48_MHz);
    }

}
