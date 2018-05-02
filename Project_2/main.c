/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date:
 * Description:
 */

#include "keypad.h"


//Define States of the FSM
#define     SQUARE          0
#define     SAWTOOTH         1
#define     SINE            2
#define     NO_KEY_PRESS    0xFF

//Global Variables
int button_press;
uint8_t freq_flag = 1;
uint8_t state_flag = 0;

struct control_FSM {
    uint8_t NS;
    uint8_t FLG_repeat;
};


uint8_t check_duty(){
    static uint8_t duty = 50;
    static uint8_t oldkey = 0xFF;

    if(button_press == oldkey){
        return duty;
    }
    if(button_press == '0' ){
        duty = 50;
    }
    else if(button_press == '#' && duty >= 10){
        duty -= 10;
    }
    else if(button_press == '*' && duty <= 90){
        duty += 10;
    }

    oldkey = button_press;
    return duty;
}

struct control_FSM check_NS(uint8_t current_state)
{
    uint8_t NS, repeat;
    if(button_press == '7' && current_state != SQUARE){
        NS = SQUARE;
        repeat = 1;
    }

    else if(button_press == '8' && current_state != SAWTOOTH){
        NS = SAWTOOTH;
        repeat = 1;
    }

    else  if(button_press == '9' && current_state != SINE){
        NS = SINE;
        repeat = 1;
    }
    else{
        NS = current_state;
        repeat = 0;
    }

    struct control_FSM ret = {NS, repeat};
    return ret;
}


void FUNCTION_GENERATOR_FSM()
{

    //Variable definitions.
    static uint8_t PS  = SQUARE;
    uint8_t NS;
    uint8_t repeat = 1;
    uint16_t period;
    struct control_FSM control;

    while(repeat == 1)
    {
        repeat = 0;
        switch(PS)
        {
            case SQUARE:
                state_flag = SQUARE;
                P2 -> OUT |= BIT0;
                P2 -> OUT &= ~(BIT1 |BIT2);

                control = check_NS(SQUARE);

                if(button_press >= 49 && button_press <= 53){
                    freq_flag = button_press - 48;
                }

                period = (int)(60000/freq_flag);

                TIMER_A0->CCR[0] = period;
                TIMER_A0->CCR[1] = (int)(check_duty()/100)*period;

                break;
            case SAWTOOTH:
                P2 -> OUT |= BIT1;
                P2 -> OUT &= ~(BIT0 |BIT2);

                state_flag = SAWTOOTH;

                control = check_NS(SAWTOOTH);

                if(button_press >= 49 && button_press <= 53){
                    freq_flag = button_press - 48;
                }

                break;
            case SINE:
                P2 -> OUT |= BIT2;
                P2 -> OUT &= ~(BIT0 |BIT1);

                state_flag = SINE;

                control = check_NS(SINE);

                if(button_press >= 49 && button_press <= 53){
                    freq_flag = button_press - 48;
                }
                break;
            default:
                NS = SQUARE;
                repeat = 0;
                break;
        }

        NS = control.NS;
        repeat = control.FLG_repeat;
        PS = NS;
    }
    return;
}


void main(void)
{
    //Disable watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    //Initialize keypad and set digit value to NO_KEY_PRESS
    INIT_KEYPAD();
    button_press = NO_KEY_PRESS;

    P2 -> DIR |= (BIT0 | BIT1 | BIT2);

    //Enable Interrupts
 //   __enable_irq();

    while(1){
        button_press = GET_CHAR_KEYPAD();
               FUNCTION_GENERATOR_FSM();
    }


}

//ISR handler for port 5 (keypad ISR)
void PORT5_IRQHandler(void)
{
    //If an interrupt is detected from the specified ports, get the digit
    //pressed and call the finite state machine.
    //digit is defined as a global variable to enable sharing between
    //multiple files.
    if(P5 ->IFG & (ROW1 + ROW2 + ROW3 + ROW4))
    {
        button_press = GET_CHAR_KEYPAD();
        FUNCTION_GENERATOR_FSM();
        P5 -> IFG  &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    }
}
