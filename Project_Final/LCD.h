/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/07/2018
 * Description: This library file implements useful functions
 *              for setting up and utilizing the NHD-0216HZ
 *              LCD in four-bit mode. The functions are documented
 *              prior to their source code.
 * Pin Assignments: P4.1 -> RS
 *                  P4.2 -> R/W
 *                  P4.3 -> EN
 *                  P4.4 -> DB4
 *                  P4.5 -> DB5
 *                  P4.6 -> DB6
 *                  P4.7 -> DB7
 */
#include "msp.h"
#include "delay.h"

//define some useful constants used in LCD.c
#define RS  BIT1
#define RW  BIT2
#define EN  BIT3
#define Clear_Upper_Byte 0x0F
#define Clear_Lower_Byte 0xF0
#define Clear_LCD_Command 0x01
#define Return_Home_Command 0x02
#define Second_Line 0xC0

//define functions implemented in LCD.c
void NYBBLE();
void LCD_COMMAND(unsigned char command);
void LCD_CLR();
void LCD_HOME();
void SET_CUR_POS_LCD(unsigned char address);
void LCD_DATA(unsigned char data);
void WRITE_CHAR_LCD(unsigned char letter);
void WRITE_STR_LCD(char word[]);
void LCD_init(void);
