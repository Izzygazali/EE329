#include "msp.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

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

void NYBBLE(){
    P4 -> OUT |= EN;
    delay_us(50, FREQ_48_MHz);
    P4 -> OUT &= ~EN;
    return;
}



void LCD_COMMAND(unsigned char COMMAND){
    P4 -> OUT &= ~(RS|RW);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (COMMAND & Clear_Lower_Byte);
    NYBBLE();
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (COMMAND & 0x0F)<<4;
    NYBBLE();
    delay_ms(2, FREQ_48_MHz);
    return;
}

void LCD_DATA(unsigned char data)
{
      P4 -> OUT |= RS | (~RW);
      P4 ->OUT &= Clear_Upper_Byte;
      P4 -> OUT |= (data & Clear_Lower_Byte);
      NYBBLE();
      P4 ->OUT &= Clear_Upper_Byte;
      P4 -> OUT |= (data & 0x0F)<<4;
      NYBBLE();
      delay_us(50, FREQ_48_MHz);
      return;
}

void LCD_init(void){
    P4 -> SEL0 &= ~(0xFE);
    P4 -> SEL1 &= ~(0xFE);
    P4 -> DIR |=   (0xFE);
    P4 -> OUT &=  ~(0xFE);
    delay_ms(500,FREQ_48_MHz);

    P4 ->OUT &= Clear_Upper_Byte;
    P4 ->OUT |= 0x30;

    NYBBLE();
    delay_ms(5,FREQ_48_MHz);
    NYBBLE();
    delay_ms(5,FREQ_48_MHz);
    NYBBLE();
    delay_ms(5,FREQ_48_MHz);

    P4 ->OUT &= Clear_Upper_Byte;
    P4 ->OUT |= 0x20;
    NYBBLE();
    delay_ms(5,FREQ_48_MHz);

    LCD_COMMAND(0x28);
    delay_ms(5,FREQ_48_MHz);
    LCD_COMMAND(0x08);
    delay_ms(5,FREQ_48_MHz);
    LCD_COMMAND(0x01);
    delay_ms(5,FREQ_48_MHz);
    LCD_COMMAND(0x06);
    delay_ms(5,FREQ_48_MHz);
    LCD_COMMAND(0x0F);
    return;
}

void WRITE_CHAR_LCD(unsigned char letter)
{
    LCD_DATA(letter);
    return;
}

void LCD_CLR()
{
    LCD_COMMAND(0x01);
}

void LCD_HOME()
{
    LCD_COMMAND(0x02);
}
void SET_CUR_POS_LCD(unsigned char address)
{
    LCD_COMMAND(0x80 | address);
}

void WRITE_STR_LCD(char word[])
{
    int letterCnt = 0;
    while(word[letterCnt]!=0)
    {
        WRITE_CHAR_LCD(word[letterCnt]);
        letterCnt++;
    }
    return;
}

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
