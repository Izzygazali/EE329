/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/07/2018 */
#include "LCD.h"

/* Function toggles EN to trigger LCD to read
 * in data or command. */
void NYBBLE(){
    P4 -> OUT |= EN;
    delay_us(50, FREQ_48_MHz);
    P4 -> OUT &= ~EN;
    return;
}

/* Function performs operations need to send a
 * command to the LCD
 * Inputs: command = 8 bit command character to send to LCD */
void LCD_COMMAND(unsigned char command){
    P4 -> OUT &= ~(RS|RW);
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (command & Clear_Lower_Byte);
    NYBBLE();
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (command & 0x0F)<<4;
    NYBBLE();
    delay_ms(2, FREQ_48_MHz);
    return;
}

/* Function utilizes LCD_COMMAND to send command that
 * clears the LCD.
 */
void LCD_CLR()
{
    LCD_COMMAND(0x01);
}

/* Function utilizes LCD_COMMAND to send command that
 * sets the cursor to "home", the top left of the LCD.
 */
void LCD_HOME()
{
    LCD_COMMAND(0x02);
}

/* Function utilizes LCD_COMMAND to send command that
 * sets the cursor position to a desired address.
 * Inputs: address = address in memory you want to set
 * DDRAM counter to, effectively changing cursor position.
 */
void SET_CUR_POS_LCD(unsigned char address)
{
    LCD_COMMAND(0x80 | address);
}

/* Function performs operations need to send data
 * to the LCD
 * Inputs: data = 8 bit data character to send to LCD */
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


/* Function writes a character to the LCD.
 * Inputs: letter = 8 bit letter character to send to LCD*/
void WRITE_CHAR_LCD(unsigned char letter)
{
    LCD_DATA(letter);
    return;
}

/* Function writes a String to the LCD.
 * Inputs: letter = 8 bit word character array to send to LCD*/
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

/* Function performs the operations at initial
 * boot up of the LCD to make the LCD operationalble in 4-bit mode.
 * Inputs: letter = 8 bit word character array to send to LCD*/
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





