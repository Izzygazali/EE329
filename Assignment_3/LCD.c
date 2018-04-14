#include "msp.h"
#include "LCD.h"

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

void LCD_Write_Char(unsigned char Letter){
    P4 -> OUT |= RS;
    P4 -> OUT &= ~(RW);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (Letter & Clear_Lower_Byte);
    Nybble(Letter);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= ((Letter & Clear_Upper_Byte)<<4);
    Nybble(Letter);
    return;
}


void LCD_Command(unsigned char Command){
    P4 -> OUT &= ~(RS|RW);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (Command & Clear_Lower_Byte);
    Nybble(Command);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= ((Command & Clear_Upper_Byte)<<4);
    Nybble(Command);
    return;
}

void LCD_Write_String(unsigned char inputString[]){
    int i = 0;
    while(inputString[i]!=0){
        if((inputString[i] == '|') && (inputString[i+1] == 'n')){
            LCD_Command(Second_Line);
            i += 2;
        }
        LCD_Write_Char(inputString[i]);
        i++;
    }
    return;
}

void Clear_LCD(){
    LCD_Command(Clear_LCD_Command);
    return;
}

void Return_Home(){
    LCD_Command(Return_Home_Command);
    return;
}

void Set_Cursor_Address(unsigned char Address){
    LCD_Command(BIT7 | Address);
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
