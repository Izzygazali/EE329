/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date:
 * Description:
 */

#include "msp.h"


//Define States of the FSM
#define     SQUARE          0
#define     SAWTOOH         1
#define     SINE            2
#define     NO_KEY_PRESS    0xFF

//Global Variables
int button_press, freq_flag, state_flag;


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

uint8_t check_NS(uint8_t current_state)
{
    if(button_press == '7'){
        NS = SQUARE;
    }

    else if(button_press == '8'){
        NS = SAWTOOTH;
    }

    else  if(button_press == '9'){
        NS = SINE;
    }
    else{
        NS = current_state
    }
    return NS;
}


void LOCK_FSM()
{

    //Variable definitions.
    static uint8_t PS  = SQUARE;
    uint8_t NS;
    uint16_t period;

    while(1)
    {
        switch(PS)
        {
            case SQUARE:
                state_flag = SQUARE;

                NS = check_NS(SQUARE);

                if(button_press >= 49 && button_press <= 53){
                    freq_flag = button_press - 48;
                }

                period = (int)(60000/freq_flag);

                TIMER_A0->CCR[0] = period;
                TIMER_A0->CCR[1] = (int)(check_duty()/100)*period;

                break;
            case SAWTOOTH:
                state_flag = SAWTOOTH;

                NS = check_NS(SAWTOOTH);

                if(button_press >= 49 && button_press <= 53){
                    freq_flag = button_press - 48;
                }

                break;
            case SINE:
                state_flag = SINE;

                NS = check_NS(SINE);

                if(button_press >= 49 && button_press <= 53){
                    freq_flag = button_press - 48;
                }
                break;
            default:
                NS = SQUARE;
                break;
        }
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

    //Enable Interrupts
    __enable_irq();

    while(1);


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
