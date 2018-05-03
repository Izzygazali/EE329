/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date:
 * Description:
 */

#include "keypad.h"
#include "DAC.h"
#include "LCD.h"

//Global Variables
uint8_t button_press;
uint8_t freq_flag = 1;
uint8_t state_flag;


/*
 * Struct that defines two variables of type uint8_t, NS and FLG_repeat. The
 * struct allows the logic functions to return FSM control variables at once.
 * NS           => next state of the FSM
 * FLG_repeat   => flag to indicate whether the program needs to iterate
 *                 through the FSM again. This flag is used to free the MCU for
 *                 other tasks when no state change is required.
 *                 1 iterate through FSM
 *                 0 return to main. (free to perform other tasks)
 */
struct control_FSM {
    uint8_t NS;
    uint8_t FLG_repeat;
};



uint8_t duty_cycle(){
    static uint8_t duty = 50;
    if(button_press == '0')
        duty = 50;
    else if(button_press == '*' && duty > 10)
        duty -= 10;
    else if(button_press == '#' && duty < 90)
        duty += 10;
    return duty;
}

struct control_FSM check_NS(uint8_t current_state)
{
    uint8_t NS, repeat;
    //----
    if (button_press == '7')
        NS = SQUARE;
    else if(button_press == '8')
        NS = SAWTOOTH;
    else if(button_press == '9')
        NS = SINE;
    else
        NS = current_state;
    //----
    if (NS == current_state)
        repeat = 0;
    else
        repeat = 1;
    struct control_FSM ret = {NS, repeat};
    return ret;
}

void square_logic(void)
{
    uint8_t duty = duty_cycle();
    uint16_t period_count;
    state_flag = SQUARE;

    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Square Wave ");
    SET_CUR_POS_LCD(0x40);
    WRITE_CHAR_LCD(freq_flag + 48);
    WRITE_STR_LCD("00Hz, ");
    WRITE_CHAR_LCD((duty/10) + 48);
    WRITE_STR_LCD("0% Duty");

    TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;
    period_count = 60000/freq_flag;
    TIMER_A0->CCR[0] = period_count;
    TIMER_A0->CCR[1] = period_count*duty/100;
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    return;
}

void sawtooth_logic(void)
{
    state_flag = SAWTOOTH;

    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Sawtooth Wave ");
    SET_CUR_POS_LCD(0x40);
    WRITE_CHAR_LCD(freq_flag + 48);
    WRITE_STR_LCD("00 Hz");

    TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;
    TIMER_A0->CCR[0] = 74;
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    return;
}
void sine_logic(void)
{
    state_flag = SINE;

    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Sine Wave ");
    SET_CUR_POS_LCD(0x40);
    WRITE_CHAR_LCD(freq_flag + 48);
    WRITE_STR_LCD("00 Hz");

    TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;
    TIMER_A0->CCR[0] = 74;
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    return;
}


void FUNCTION_GENERATOR_FSM()
{
    //Variable definitions.
    struct control_FSM control;
    static uint8_t PS  = SQUARE;
    uint8_t NS;
    uint8_t repeat = 1;

    if(button_press >= 49 && button_press <= 53)
        freq_flag = button_press - 48;

    while(repeat)
    {
        repeat = 0;
        switch(PS)
        {
            case SQUARE:
                square_logic();
                control = check_NS(SQUARE);
                break;
            case SAWTOOTH:
                sawtooth_logic();
                control = check_NS(SAWTOOTH);
                break;
            case SINE:
                sine_logic();
                control = check_NS(SINE);
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
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    set_DCO(FREQ_24_MHz);
    SPI_INIT();
    INIT_TIMER();
    LCD_init();
    //Initialize keypad and set digit value to NO_KEY_PRESS
    INIT_KEYPAD();
    button_press = NO_KEY_PRESS;

    freq_flag = FREQ_100Hz;
    state_flag = SQUARE;
    //Enable interrupts globally
   __enable_irq();
    while(1);
}



//ISR handler for port 5 (keypad ISR)
void PORT5_IRQHandler(void)
{
    //If an interrupt is detected from the specified ports, get the digit
    //pressed and call the finite state machine.
    if(P5 ->IFG & (ROW1 + ROW2 + ROW3 + ROW4))
    {
        button_press = GET_CHAR_KEYPAD();
        FUNCTION_GENERATOR_FSM();
        P5 -> IFG  &= ~(ROW1 + ROW2 + ROW3 + ROW4);
    }
}

void TA0_0_IRQHandler(void)
{
    //Variable for the "sample" we are currently on
    static int16_t step = 0;
    //Variable for the sample number we will count up to
    static int16_t max_step;
    //Variable for the amplitude of that sample
    static uint16_t level = 0;
    //flag to keep track of pos/neg half cycle
    static uint8_t pos_neg_flag = 0x00;

    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        //Select the correct Level function for the waveform
        //currently being generated
        switch (state_flag)
        {
            case SINE:
                if (pos_neg_flag == 0x01)
                    level = 2047-samples[step];
                else
                    level = 2047+samples[step];
                max_step = 399;
                break;
            case SAWTOOTH:
                level = (int)(step*10.25);
                max_step = 799;
                break;
            case SQUARE:
                level = 0;
                break;
        }
        //write this amplitude to the DAC
        WRITE_DAC(level);
        //Change the frequency of the wave by varying the
        //steps per interrupt from Timer_A
        switch (freq_flag)
        {
            case FREQ_100Hz:
                step+=1;
                break;
            case FREQ_200Hz:
                step+=2;
                break;
            case FREQ_300Hz:
                step+=3;
                break;
            case FREQ_400Hz:
                step+=4;
                break;
            case FREQ_500Hz:
                step+=5;
                break;
        }
        //At the maximum step count reset the step count
        //and flip the pos_neg half-cycle flag for the Sine wave.
        if (step >= max_step){
            step = 0;
            pos_neg_flag ^= 0x01;
        }
    }
    //reset interrupt flag for TIMER_A0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

void TA0_N_IRQHandler(void)
{
    //Only care about interrupt 1 from Timer_A
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
      //Only do something if we are currently generating the square wave
      if (state_flag == SQUARE)
          //set the output low for part of the square wave
          WRITE_DAC(4095);
    }
    //reset interrupt flag for TIMER_A1
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}
