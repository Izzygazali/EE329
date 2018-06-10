/*
 * Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 05/30/2018
 */

#include "lcd_control.h"

//BIT0 - "redraw" screen flag
//BIT1 - enter key flag
//BIT2 - start log flag
//BIT3 - log flag
uint16_t lcd_flags = 0x0000;

//define FSM events
enum event_type
{
    up_pressed,
    down_pressed,
    enter_pressed,
    nothing_pressed
};
enum event_type event = nothing_pressed;

//define FSM states
enum state_type
{
    hiking_display,
    data_1,
    data_2,
    data_3,
    data_4
};
enum state_type state = hiking_display;

/*
 * Function that returns the current status of the LCD flags
 * INPUTS       NONE
 * RETURN       uint16_t lcd_flags = 2-byte word containing the LCD flags
 */
uint16_t get_lcd_flags(void)
{
    return lcd_flags;
}

/*
 * Function that sets the LCD flags
 * INPUTS       uint16_t flags = 2-byte word containing the value to set the LCD flags to
 * RETURN       NONE
 */
void set_lcd_flags(uint16_t flags)
{
    lcd_flags |= flags;
    return;
}

/*
 * Function that resets the LCD flags
 * INPUTS       uint16_t flags = 2-byte word containing the flag to reset
 * RETURN       NONE
 */
void reset_lcd_flags(uint16_t flags)
{
    lcd_flags &= ~flags;
    return;
}
/*
 * Function that determines the FSM's next state based on the current event
 * INPUTS       NONE
 * RETURN       NONE
 */
void lcd_state_decode(void)
{
    switch(event)
    {
        //if the up button is pressed and the prior key pressed is not enter,
        //switch between screen displays. Otherwise trigger LCD state flag.
        case up_pressed:
            if ((lcd_flags & lcd_enter_flag) == 0)
            {
                if (state == 0)
                    state = data_4;
                else
                    state--;
            }
            else
                lcd_flags |= lcd_state_flag;
            break;

        //if the down button is pressed and the prior key pressed is not enter,
        //switch between screen displays. Otherwise trigger LCD state flag.
        case down_pressed:
            if ((lcd_flags & lcd_enter_flag) == 0)
            {
                if (state == 4)
                    state = hiking_display;
                else
                    state++;
            }
            else
                lcd_flags &= ~lcd_state_flag;
            break;

        //if the enter button is pressed and turn cursor on otherwise
        //turn cursor off
        case enter_pressed:
            lcd_flags ^= lcd_enter_flag;
            if ((lcd_flags & lcd_enter_flag) == 0)
            {
                lcd_flags |= lcd_screen_flag;
                LCD_COMMAND(TURN_CURSOR_ON);
            }
            else
                LCD_COMMAND(TURN_CURSOR_OFF);
            break;

        case nothing_pressed:
            break;
    }
    event = nothing_pressed;
    return;
}


/*
 * Function that implements the LCD finite state machine.
 * INPUTS       NONE
 * RETURN       NONE
 */
void lcd_FSM(void)
{
    switch(state)
    {
        //initial display: contains timer, distance traveled, pace, and
        //start/stop logging selection
        case hiking_display:
            if (lcd_flags & lcd_screen_flag)
            {
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD(tow_to_string());
                SET_CUR_POS_LCD(0x08);
                WRITE_STR_LCD(pace_to_string());
                WRITE_STR_LCD("/Km");
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD(dist_to_string());
                WRITE_STR_LCD("Km");
                SET_CUR_POS_LCD(0x4D);
                WRITE_STR_LCD("S E");

                //if lcd_state_flag is set, a '*' is written next to the 'S' on
                //the display to indicated logging is on. Otherwise, a '*' is
                //written next to the 'E' to indicate end of logging
                if (lcd_flags & lcd_state_flag)
                    SET_CUR_POS_LCD(0x4C);
                else
                    SET_CUR_POS_LCD(0x4E);

                WRITE_STR_LCD("*");
                lcd_flags &= ~lcd_screen_flag;
            }

            //set cursor position after write
            if (lcd_flags & lcd_enter_flag)
            {
                if (lcd_flags & lcd_state_flag)
                     SET_CUR_POS_LCD(0x4C);
                 else
                     SET_CUR_POS_LCD(0x4E);
            }
            break;

        //data_1 screen: contains Date and time
        case data_1:
            if (lcd_flags & lcd_screen_flag)
            {
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Date: ");
                WRITE_STR_LCD(date_to_string());
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD("Time: ");
                WRITE_STR_LCD(time_to_string());
                lcd_flags &= ~lcd_screen_flag;
            }
            break;

        //data_2 screen: contains Latitude and Longitude
        case data_2:
            if (lcd_flags & lcd_screen_flag)
            {
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Lat: ");
                WRITE_STR_LCD(latitude_to_string());
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD("Lon: ");
                WRITE_STR_LCD(longitude_to_string());
                lcd_flags &= ~lcd_screen_flag;
            }
            break;

        //data_3 screen: contains speed in Km/hr and altitude in meters.
        case data_3:
            if (lcd_flags & lcd_screen_flag)
            {
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Spd: ");
                WRITE_STR_LCD(speed_to_string());
                WRITE_STR_LCD("Km/hr");
                SET_CUR_POS_LCD(0x40);
                WRITE_STR_LCD("Alt: ");
                WRITE_STR_LCD(alt_to_string());
                WRITE_STR_LCD("m");
                lcd_flags &= ~lcd_screen_flag;
            }
            break;

        //data_4 screen: contains temperature in degrees celsius.
        case data_4:
            if (lcd_flags & lcd_screen_flag)
            {
                LCD_HOME();
                LCD_CLR();
                WRITE_STR_LCD("Temp: ");
                WRITE_STR_LCD(temp_to_string());
                WRITE_STR_LCD(" degC");
                lcd_flags &= ~lcd_screen_flag;
            }
            break;
        default:
            state = hiking_display;
            break;
    }
    return;
}

/*
 * Function that initialized the GPIO ports to interrupt when the up,down, or enter
 * buttons are pressed.
 *
 * UP       ->  P5.4
 * DOWN     ->  P5.5
 * ENTER    ->  P5.6
 *
 * INPUTS       NONE
 * RETURN       NONE
 */
void init_buttons(void)
{
    // GPIO setting
    P5->DIR &= ~(button_up | button_down | button_enter);
    P5->IE |= (button_up | button_down | button_enter);     // Enable interrupts for P1.1 and P1.4
    P5->REN |= (button_up | button_down | button_enter);    // Add pull up resistor.
    P5->OUT |= (button_up | button_down | button_enter);
    P5->IES |= (button_up | button_down | button_enter);    // Select high to low edge Interrupt
    P5->IFG &= ~(button_up | button_down | button_enter);   // Clear Interrupt flag.

    //enable Port 5 interrupts
    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);
    return;
}

/*
 * Function that initialized TIMERA0 in up mode using ACLK to interrupt every 1s. TIMERA0
 * is used to specify the LCD update rate.
 * INPUTS       NONE
 * RETURN       NONE
 */
void LCD_update_timer(void)
{
    // TACCR0 interrupt enabled
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    //Set CCR0 value that generate an interrupt every 2s
    TIMER_A0->CCR[0] = CCR0_COUNT;

    //ACLK, up mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__UP;

    // Enable interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    return;
}

// Port 5 interrupt service routine, used to set event when a button is pressed
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

// Timer A0 interrupt service routine, used as a timer for updating display
void TA0_0_IRQHandler(void)
{
    //LCD update flag is set every 1s
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG){
        lcd_flags |= lcd_screen_flag;
        lcd_flags |= lcd_log_flag;
    }

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

