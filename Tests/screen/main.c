#include "msp.h"
#include "LCD.h"

//bit 0 - "redraw" screen flag
//bit 1 - "redraw" data flag
uint16_t lcd_flags = 0;

enum event_type{
    up_pressed,
    down_pressed,
    enter_pressed
};
enum event_type event;

enum state_type{
    hiking_display,
    data_display,
    location_display,
    settings_display
};

void lcd_FSM(void)
{
    static enum state_type state = hiking_display;
    switch(state)
    {
        case hiking_display:
            break;

    }
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    LCD_init();
    LCD_HOME();
    LCD_CLR();


    while(1);
}

