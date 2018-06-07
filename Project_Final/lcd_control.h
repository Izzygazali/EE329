#include "msp.h"

//useful defintition
#define lcd_screen_flag     BIT0
#define lcd_enter_flag      BIT1
#define lcd_state_flag      BIT2
#define lcd_log_flag        BIT3

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
uint16_t get_lcd_flags(void);
void reset_lcd_flags(uint16_t flags);

