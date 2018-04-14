#include "msp.h"
#include "LCD.h"

/* P2.4 -> ROW1
 * P2.5 -> ROW2
 * P2.6 -> ROW3
 * P2.7 -> ROW4
 * P3.5 -> COL1
 * P3.6 -> COL2
 * P3.7 -> COL3
 */
#define LEDR BIT0
#define LEDG BIT1
#define LEDB BIT2

#define ROW1 BIT4
#define ROW2 BIT5
#define ROW3 BIT6
#define ROW4 BIT7

#define COL1 BIT5
#define COL2 BIT6
#define COL3 BIT7

uint8_t GET_CHAR_KEYPAD(void);
void INIT_KEYPAD(void);

