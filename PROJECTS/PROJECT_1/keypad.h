/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/16/2018
 * Description: This library file implements useful functions for setting up
 *              and utilizing the COM-086553. The functions are documented
 *              prior to their source code.
 * LCD Pin Assignments:     P4.1 -> RS
 *                          P4.2 -> R/W
 *                          P4.3 -> EN
 *                          P4.4 -> DB4
 *                          P4.5 -> DB5
 *                          P4.6 -> DB6
 *                          P4.7 -> DB7
 * Keypad Pin Assignments:  P5.4 -> ROW1
 *                          P5.5 -> ROW2
 *                          P5.6 -> ROW3
 *                          P5.7 -> ROW4
 *                          P3.5 -> COL1
 *                          P3.6 -> COL2
 *                          P3.7 -> COL3
 */
#include "msp.h"
#include "LCD.h"

//define some useful constants used in keypad.c
#define ROW1 BIT4
#define ROW2 BIT5
#define ROW3 BIT6
#define ROW4 BIT7
#define COL1 BIT5
#define COL2 BIT6
#define COL3 BIT7
#define NO_KEY_PRESS 0xFF

//function prototypes
void INIT_KEYPAD(void);
uint8_t GET_CHAR_KEYPAD(void);



