#include "msp.h"

#define lcd_screen_flag BIT0
#define lcd_enter_flag BIT1

#define button_up BIT4
#define button_down BIT5
#define button_enter BIT6

void lcd_state_decode(void);
void lcd_FSM(void);
void init_buttons(void);
