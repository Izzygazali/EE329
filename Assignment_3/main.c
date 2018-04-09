#include "msp.h"
#include "delay.h"
#include <stdio.h>

/*
 * RS   4.1
 * R/W  4.2
 * EN   4.3
 *
 * DB4  4.4
 * DB5  4.5
 * DB6  4.6
 * DB7  4.7
 *
 */

#define RS  BIT1
#define RW  BIT2
#define EN  BIT3
#define Clear_Upper_Byte 0x0F
#define Clear_Lower_Byte 0xF0
#define Mode_4bit 0x30


void Nybble(unsigned char Command){
    P4 -> OUT |= EN;
    if (Command < 0x04){
        delay_ms(2,FREQ_48_MHz);
    }
    else{
        delay_us(50,FREQ_48_MHz);
    }
    P4 -> OUT &= ~EN;
    return;
}


void LCD_Command(unsigned char Command){
    P4 -> OUT &= ~(RS|RW);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (Command & Clear_Lower_Byte);
    Nybble(Command);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= ((Command & 0x0F)<<4);
    Nybble(Command);
    return;
}

void LCD_init(void){
    P4 -> SEL0 &= BIT0;
    P4 -> SEL1 &= BIT0;
    P4 -> DIR |= ~BIT0;
    P4 -> OUT &= 0x00;
    delay_ms(50,FREQ_48_MHz);

    LCD_Command(0x30);
    delay_us(100,FREQ_48_MHz);
    LCD_Command(0x20);
    delay_us(100,FREQ_48_MHz);
    LCD_Command(0x20);
    delay_us(100,FREQ_48_MHz);
    LCD_Command(0x0F);
    delay_us(100,FREQ_48_MHz);
    LCD_Command(0x01);
    delay_ms(2,FREQ_48_MHz);
    LCD_Command(0x06);
    return;
}



void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    set_DCO(FREQ_48_MHz);
    LCD_init();

    P1 ->SEL0 &= ~BIT0;
    P1 ->SEL1 &= ~BIT1;
    P1 ->DIR |= BIT0;

    while (1){
       P1 ->OUT ^= BIT0;
       delay_ms(500, FREQ_48_MHz);
    }

}
