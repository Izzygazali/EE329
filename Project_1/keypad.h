#include "msp.h"
#include "LCD.h"


#define RED_LED      BIT0
#define GREEN_LED    BIT1
#define BLUE_LED     BIT2
#define ROW1         BIT4
#define ROW2         BIT5
#define ROW3         BIT6
#define ROW4         BIT7
#define COL1         BIT5
#define COL2         BIT6
#define COL3         BIT7
#define NO_KEY_PRESS 0xFF


uint8_t KEYPAD_GET_KEY(void);
uint8_t KEYPAD_ASCII(uint8_t key);
void KEYPAD_INIT(void);
void KEYPAD_TO_LCD();
void KEYPAD_TO_LCD_LED();
