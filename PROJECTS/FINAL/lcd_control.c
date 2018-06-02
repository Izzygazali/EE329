#include "LCD.h"
#include "lcd_control.h"

//bit 0 - "redraw" screen flag
//bit 1 - enter key flag
uint16_t lcd_flags = 0x0001;
//0 -> run; 1 -> pause; 2 -> stop

uint8_t run_state = 0;
char *timer_str = "12:33";
char *pace_str = "12:22";
char *dist_str = "999.99";


enum event_type{
    up_pressed,
    down_pressed,
    enter_pressed,
    nothing_pressed
};
enum event_type event = nothing_pressed;

enum state_type{
    hiking_display,
    data_1,
    data_2,
    data_3,
    data_4
};

enum state_type state = hiking_display;

void lcd_state_decode(void)
{
    switch(event){
        case up_pressed:
            if ((lcd_flags & lcd_enter_flag) == 0){
                if (state == 0)
                    state = data_4;
                else
                    state--;
                lcd_flags |= lcd_screen_flag;
            }else{
                if (run_state == 2)
                    run_state = 0;
                else
                    run_state++;
            }
            break;
        case down_pressed:
            if ((lcd_flags & lcd_enter_flag) == 0){
                if (state == 4)
                    state = hiking_display;
                else
                    state++;
                lcd_flags |= lcd_screen_flag;
            }else{
                if (run_state == 0)
                    run_state = 2;
                else
                    run_state--;
            }
            break;
        case enter_pressed:
            lcd_flags ^= lcd_enter_flag;
            if ((lcd_flags & lcd_enter_flag) == 0){
                lcd_flags |= lcd_screen_flag;
                LCD_COMMAND(0x0C);
            }else{
                LCD_COMMAND(0x0F);
            }
            break;
        case nothing_pressed:
            break;
    }
    event = nothing_pressed;
    return;
}
void lcd_FSM(void)
{


    switch(state)
    {
        case hiking_display:
            if (lcd_flags & lcd_screen_flag){
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD(timer_str);
                SET_CUR_POS_LCD(0x08);
                WRITE_STR_LCD(pace_str);
                WRITE_STR_LCD("/Km");
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD(dist_str);
                WRITE_STR_LCD("Km");
                SET_CUR_POS_LCD(0x4B);
                WRITE_STR_LCD("R P S");
                switch(run_state)
                {
                case 0:
                    SET_CUR_POS_LCD(0x4A);
                break;
                case 1:
                    SET_CUR_POS_LCD(0x4C);
                break;
                case 2:
                    SET_CUR_POS_LCD(0x4E);
                break;
                }
                WRITE_STR_LCD("*");
                lcd_flags &= ~lcd_screen_flag;
            }
            if (lcd_flags & lcd_enter_flag){
                switch(run_state)
                {
                    case 0:
                        SET_CUR_POS_LCD(0x4A);
                        break;
                    case 1:
                        SET_CUR_POS_LCD(0x4C);
                        break;
                    case 2:
                        SET_CUR_POS_LCD(0x4E);
                        break;
                }
            }
            break;
        case data_1:
            if (lcd_flags & lcd_screen_flag){
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Date: ");
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD("Time: ");
                lcd_flags &= ~lcd_screen_flag;
            }

            break;
        case data_2:
            if (lcd_flags & lcd_screen_flag){
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Lat: ");
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD("Lon: ");
                lcd_flags &= ~lcd_screen_flag;
            }
            break;
        case data_3:
            if (lcd_flags & lcd_screen_flag){
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Spd: ");
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD("Alt: ");
                lcd_flags &= ~lcd_screen_flag;
            }
        case data_4:
            if (lcd_flags & lcd_screen_flag){
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Temp: ");
                lcd_flags &= ~lcd_screen_flag;
            }
            break;
        default:
            state = hiking_display;
            break;
    }
    return;
}

void init_buttons(void)
{
    P5->DIR &= ~(button_up | button_down | button_enter);
    P5->IE |= (button_up | button_down | button_enter);            // Enable interrupts for P1.1 and P1.4
    P5->REN |= (button_up | button_down | button_enter);           // Add pull up resistor.
    P5->OUT |= (button_up | button_down | button_enter);
    P5->IES |= (button_up | button_down | button_enter);           // Select high to low edge Interrupt
    P5->IFG &= ~(button_up | button_down | button_enter);          // Clear Interrupt flag.

    __enable_irq();
    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);
    return;
}

void PORT5_IRQHandler(void)
{
    if(P5->IFG & button_up){
        event = up_pressed;
    }
    else if(P5->IFG & button_down){
        event = down_pressed;
    }
    else if(P5->IFG & button_enter){
        event = enter_pressed;
    }
    P5->IFG &= ~(button_up | button_down | button_enter);
}
