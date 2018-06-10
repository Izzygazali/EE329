#include "msp.h"
#include "delay.h"

#define RS  BIT1
#define RW  BIT2
#define EN  BIT3
#define Clear_LCD_Command 0x01
#define Return_Home_Command 0x02
#define Clear_Upper_Byte 0x0F
#define Clear_Lower_Byte 0xF0
#define Mode_4bit 0x30
#define Second_Line 0xC0


void Nybble(unsigned char Command);
void LCD_Write_Char(unsigned char Letter);
void LCD_Command(unsigned char Command);
void LCD_newLine();
void LCD_Write_String(unsigned char inputString[]);
void Clear_LCD();
void Return_Home();
void Set_Cursor_Address(unsigned char Address);
void LCD_init(void);
