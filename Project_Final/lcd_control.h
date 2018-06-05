#include "msp.h"

#define lcd_screen_flag BIT0
#define lcd_enter_flag BIT1
#define lcd_state_flag BIT2

#define button_up BIT4
#define button_down BIT5
#define button_enter BIT6

void lcd_state_decode(void);
void lcd_FSM(void);
void init_buttons(void);
void set_lcd_flags(uint16_t flags);
uint16_t get_lcd_flags(void);
