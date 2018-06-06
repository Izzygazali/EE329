#include "msp.h"

#define lcd_screen_flag     BIT0
#define lcd_enter_flag      BIT1
#define lcd_state_flag      BIT2

#define button_up           BIT4
#define button_down         BIT5
#define button_enter        BIT6
#define CCR0_COUNT          32768

void lcd_state_decode(void);
void lcd_FSM(void);
void init_buttons(void);
void set_lcd_flags(uint16_t flags);
void LCD_update_timer(void);
uint16_t get_lcd_flags(void);

