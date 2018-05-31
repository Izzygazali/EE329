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
};

enum state_type state = hiking_display;

void lcd_state_decode(void)
{
    switch(event){
        case up_pressed:
            switch(state){
                case hiking_display:

            }
    }
    return;
}
void lcd_FSM(void)
{
    switch(state)
    {
        case hiking_display:
            break;
        case data_display:
            break;
        case location_display:
            break;
        default:
            state = hiking_display;
            break;
    }
    return;
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    LCD_init();
    LCD_HOME();
    LCD_CLR();


    while(1);
}

