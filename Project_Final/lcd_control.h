/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 05/30/2018
 * Description: This library file implements useful functions
 *              for controlling what it is displayed on the LCD.
 *              The functions are documented prior to their source code.
 *
 * Pin Assignments: P5.4 -> tactile button -> down
 *                  P5.5 -> tactile button -> up
 *                  P5.6 -> tactile button -> enter
 */

#include "msp.h"
#include "LCD.h"
#include "gps.h"
#include "string_conv.h"


//useful definitions
#define lcd_screen_flag     BIT0
#define lcd_enter_flag      BIT1
#define lcd_state_flag      BIT2
#define lcd_log_flag        BIT3

#define TURN_CURSOR_ON      0x0C
#define TURN_CURSOR_OFF     0x0F

#define button_up           BIT5
#define button_down         BIT4
#define button_enter        BIT6
#define CCR0_COUNT          32768

//function prototypes,
void lcd_state_decode(void);
void lcd_FSM(void);
void init_buttons(void);
void set_lcd_flags(uint16_t flags);
void LCD_update_timer(void);
void reset_lcd_flags(uint16_t flags);
uint16_t get_lcd_flags(void);


