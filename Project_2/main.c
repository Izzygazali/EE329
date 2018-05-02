/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date:
 * Description:
 */

#include "keypad.h"
#include "DAC.h"


//Define States of the FSM
#define     SQUARE          0
#define     SAWTOOTH         1
#define     SINE            2
#define     NO_KEY_PRESS    0xFF

//Global Variables
int button_press;
uint8_t freq_flag;
uint8_t state_flag;

struct control_FSM {
    uint8_t NS;
    uint8_t FLG_repeat;
};


uint8_t duty_cycle(){
    static uint8_t duty = 50;
    if(button_press == '0')
        duty = 50;
    else if(button_press == '#' && duty > 10)
        duty -= 10;
    else if(button_press == '*' && duty < 90)
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
      state_flag = SQUARE;
      TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;
      period = 60000/freq_flag;
      TIMER_A0->CCR[0] = period;
      TIMER_A0->CCR[1] = period*duty_cycle()/100;
      TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
      return;
}

void FUNCTION_GENERATOR_FSM()
{

    //Variable definitions.
    static uint8_t PS  = SQUARE;
    struct control_FSM control;
    uint8_t NS;
    uint16_t period;
    uint8_t repeat = 1;

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
                P2 -> OUT |= BIT1;
                P2 -> OUT &= ~(BIT0 |BIT2);

                state_flag = SAWTOOTH;

                control = check_NS(SAWTOOTH);
                TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;
                TIMER_A0->CCR[0] = 74;
                TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
                break;
            case SINE:
                P2 -> OUT |= BIT2;
                P2 -> OUT &= ~(BIT0 |BIT1);

                state_flag = SINE;

                control = check_NS(SINE);


                TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;
                TIMER_A0->CCR[0] = 74;
                TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
                break;
            default:
                NS = SQUARE;
                repeat = 0;
                break;
        }
        if(button_press >= 49 && button_press <= 53){
            freq_flag = button_press - 48;
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
    //Initialize keypad and set digit value to NO_KEY_PRESS
    INIT_KEYPAD();
    button_press = NO_KEY_PRESS;

    freq_flag = FREQ_100Hz;
    state_flag = SQUARE;

    P2 -> DIR |= (BIT0 | BIT1 | BIT2);

    //Enable Interrupts
   __enable_irq();

    while(1){

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

void TA0_0_IRQHandler(void)
{
    P1->OUT |= BIT0;
    //Variable for the "sample" we are currently on
    static int16_t step = 0;
    static int16_t max_step = 199;
    //Variable for the amplitude of that sample
    static uint16_t level = 0;
    //flag to keep track of pos/neg half cycle
    static uint8_t pos_neg_flag = 0x00;

    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
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
                level = 4095;
                break;
        }
        //write this amplitude to the DAC
        WRITE_DAC(level);
        //check if we are at the top of the triangle wave
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
        if (step >= max_step){
            //if we are at the top of the wave count down
            step = 0;
            pos_neg_flag ^= 0x01;
        }
    }

    P1->OUT &= ~BIT0;

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
      if (state_flag == SQUARE)
          WRITE_DAC(0);
    }
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}
