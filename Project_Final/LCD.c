/*
 * Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/07/2018
 */
#include "LCD.h"

/*
 * Function toggles EN to trigger LCD to read
 * in data or command.
 */
void NYBBLE(){
    P4 -> OUT |= EN;
    delay_us(50, FREQ_12_MHz);
    P4 -> OUT &= ~EN;
    return;
}

/*
 * Function performs operations need to send a
 * command to the LCD
 * Inputs: command = 8 bit command character to send to LCD
 */
void LCD_COMMAND(unsigned char command){
    // Set RS and RW to zero
    P4 -> OUT &= ~(RS|RW);
    // Put 4-bit half-command into the correct place in P4
    // by first clearing it then "oring" with relevant part
    // of the command
    P4 ->OUT &= Clear_Upper_Byte;
    P4 -> OUT |= (command & Clear_Lower_Byte);
    // Toggle Enable
    NYBBLE();
    // Put 4-bit half-command into the correct place in P4
    // by first clearing it then "oring" with relevant part
    // of the command
    P4 ->OUT &= Clear_Upper_Byte;
    //Clear top nibble and shift lower nibble to upper nibble
    P4 -> OUT |= (command & 0x0F)<<4;
    // Toggle Enable
    NYBBLE();
    delay_ms(2, FREQ_12_MHz);
    return;
}

/*
 * Function utilizes LCD_COMMAND to send command that
 * clears the LCD.
 */
void LCD_CLR()
{
    LCD_COMMAND(Clear_LCD_Command);
}

/*
 * Function utilizes LCD_COMMAND to send command that
 * sets the cursor to "home", the top left of the LCD.
 */
void LCD_HOME()
{
    LCD_COMMAND(Return_Home_Command);
}

/*
 * Function utilizes LCD_COMMAND to send command that
 * sets the cursor position to a desired address.
 * Inputs: address = address in memory you want to set
 * DDRAM counter to, effectively changing cursor position.
 */
void SET_CUR_POS_LCD(unsigned char address)
{
    // 0x80 sets the correct bit to use DDRAM counter
    // set then address sets the counter
    LCD_COMMAND(0x80 | address);
}

/* Function performs operations need to send data
 * to the LCD
 * Inputs: data = 8 bit data character to send to LCD */
void LCD_DATA(unsigned char data)
{
      // Set RS to one and RW to zero
      P4 -> OUT |= RS | (~RW);
      // Rest is same as LCD_COMMAND
      P4 ->OUT &= Clear_Upper_Byte;
      P4 -> OUT |= (data & Clear_Lower_Byte);
      NYBBLE();
      P4 ->OUT &= Clear_Upper_Byte;
      P4 -> OUT |= (data & 0x0F)<<4;
      NYBBLE();
      delay_us(50, FREQ_12_MHz);
      return;
}


/*
 * Function writes a character to the LCD.
 * Inputs: letter = 8 bit letter character to send to LCD
 */
void WRITE_CHAR_LCD(unsigned char letter)
{
    //Call data write function with letter
    LCD_DATA(letter);
    return;
}

/*
 * Function writes a String to the LCD.
 * Inputs: letter = 8 bit word character array to send to LCD
 */
void WRITE_STR_LCD(char word[])
{
    uint16_t letterCnt = 0;
    // take array of characters and write one at a time
    // with LCD_CHAR_LCD. "|n" is defined as "newline"
    // and will move the cursor to the second line of
    // the LCD
    while(word[letterCnt]!=0)
    {
        if((word[letterCnt] == '|') && (word[letterCnt+1] == 'n')){
            LCD_COMMAND(Second_Line);
            letterCnt += 2;
        }
        WRITE_CHAR_LCD(word[letterCnt]);
        letterCnt++;
    }
    return;
}

/*
 * Function performs the operations at initial
 * boot up of the LCD to make the LCD operationalble in 4-bit mode.
 * Inputs: letter = 8 bit word character array to send to LCD
 */
void LCD_init(void){
    // set pins for driving LCD as output and initialize to zero
    P4 -> SEL0 &= ~(0xFE);
    P4 -> SEL1 &= ~(0xFE);
    P4 -> DIR |=   (0xFE);
    P4 -> OUT &=  ~(0xFE);
    delay_ms(500,FREQ_12_MHz);
    // Send command to "wake-up" display three times
    P4 ->OUT &= Clear_Upper_Byte;
    P4 ->OUT |= 0x30;
    NYBBLE();
    delay_ms(5,FREQ_12_MHz);
    NYBBLE();
    delay_ms(5,FREQ_12_MHz);
    NYBBLE();
    delay_ms(5,FREQ_12_MHz);
    // Set LCD to 4-bit mode
    P4 ->OUT &= Clear_Upper_Byte;
    P4 ->OUT |= 0x20;
    NYBBLE();
    delay_ms(5,FREQ_12_MHz);
    //Turn display on, disable cursor, and no blink
    LCD_COMMAND(0x0C);
    delay_us(100,FREQ_12_MHz);
    //Clear display
    LCD_COMMAND(0x01);
    delay_ms(2,FREQ_12_MHz);
    // Set entry mode to "right"
    LCD_COMMAND(0x06);
    return;
}
