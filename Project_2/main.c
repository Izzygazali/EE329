/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 5/1/2018
 * Description: Program that implements a function generator using the MSP432
 * and the MCP4921 DAC. The function generator is capable of generating square,
 * sawtooth and sine waves with variable frequencies between 100Hz and 500Hz in
 * 100Hz increments. The amplitude is set to 3.3 Vpp with 1.65 V DC offset. The
 * square wave has a variable duty cycle between 10% and 90% in increments of 10%.
 */

#include "msp.h"
#include "keypad.h"
#include "function_generator.h"
#include "LCD.h"

//define states of the FSM
#define SQUARE       0
#define SAWTOOTH     1
#define SINE         2
#define TRIANGLE     3

//Global Variables, initially set to 100 Hz square wave
uint8_t button_press = NO_KEY_PRESS;
uint8_t freq_flag = FREQ_100Hz;
uint8_t state_flag = SQUARE;


/*
 * Struct that defines two variables of type uint8_t, NS and FLG_repeat. The
 * struct allows functions to return FSM control variables at once.
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



/*
 * Function that determines the correct duty cycle for the square wave.
 * The '0' key sets the duty cycle to 50%, the '*' key decreases the
 * duty cycle by 10% to a minimum of 10% duty cycle, and the '#' key
 * increases the duty cycle by 10% to a maximum of 90%.
 * INPUT:   NONE
 * RETURN:  uint8_t duty = square wave duty cycle.
 */
uint8_t duty_cycle(){
    //Duty cycle initially set to 50%
    static uint8_t duty = 50;

    if(button_press == '0')
        duty = 50;
    else if(button_press == '*' && duty > 10)
        duty -= 10;
    else if(button_press == '#' && duty < 90)
        duty += 10;
    return duty;
}

/*
 * Function that determines the next state of the FSM and whether a repeat
 * is required or not.
 * INPUT:   uint8_t current_state =  FSMs present state
 * RETURN:  struct control_FSM ret = struct containing the NS and repeat
 *                                   flag status
 */
struct control_FSM check_NS(uint8_t current_state)
{
    //variable definitions
    uint8_t NS, repeat;

    //Next state is set based on which key is pressed on the keypad.
    //Next state is set to the current state if none of the keys designated
    //for wave type is pressed.
    if (button_press == '6')
        NS = TRIANGLE;
    else if (button_press == '7')
        NS = SQUARE;
    else if(button_press == '8')
        NS = SAWTOOTH;
    else if(button_press == '9')
        NS = SINE;
    else
        NS = current_state;

    //The repeat flag is set high when there is a state change.
    if (NS == current_state)
        repeat = 0;
    else
        repeat = 1;

    //return next state and repeat flag status.
    struct control_FSM ret = {NS, repeat};
    return ret;
}

/*
 * Function that handles the logic for the square wave state. Period, duty cycle,
 * and output to LCD are handled in this function.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void square_logic(void)
{
    //Variable definitions
    uint16_t period_count;
    uint8_t duty = duty_cycle();                //Get current duty cycle

    //Set state flag to square wave state
    state_flag = SQUARE;

    //Display wave type, frequency and duty cycle on LCD.
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Square Wave ");
    SET_CUR_POS_LCD(Second_Line);
    WRITE_CHAR_LCD(freq_flag + 48);
    WRITE_STR_LCD("00Hz, ");
    WRITE_CHAR_LCD((duty/10) + 48);
    WRITE_STR_LCD("0% Duty");

    //Set CCR0 and CCR1 counts for corresponding period and duty cycle
    TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;      //stop timer
    period_count = 60000/freq_flag;             //determine period
    TIMER_A0->CCR[0] = period_count;            //set CCR0 count for 1 period
    TIMER_A0->CCR[1] = period_count*duty/100;   //set CCR1 count for correct duty cycle
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;        //start timer
    return;
}

/*
 * Function that handles the logic for the sawtooth wave state.
 * The sampling rate and output to LCD are handled in this function.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void sawtooth_logic(void)
{
    //Set state flag to sawtooth wave state
    state_flag = SAWTOOTH;

    //Display wave type and frequency on LCD
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Sawtooth Wave ");
    SET_CUR_POS_LCD(Second_Line);
    WRITE_CHAR_LCD(freq_flag + 48);
    WRITE_STR_LCD("00 Hz");

    //Set CCR0 count for the correct sampling rate
    TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;      //stop timer
    TIMER_A0->CCR[0] = 74;                      //set CCR0 count
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;        //start timer
    return;
}

/*
 * Function that handles the logic for the triangle wave state.
 * The sampling rate and output to LCD are handled in this function.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void triangle_logic(void)
{
    //Set state flag to sawtooth wave state
    state_flag = TRIANGLE;

    //Display wave type and frequency on LCD
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Triangle Wave ");
    SET_CUR_POS_LCD(Second_Line);
    WRITE_CHAR_LCD(freq_flag + 48);
    WRITE_STR_LCD("00 Hz");

    //Set CCR0 count for the correct sampling rate
    TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;      //stop timer
    TIMER_A0->CCR[0] = 74;                      //set CCR0 count
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;        //start timer
    return;
}

/*
 * Function that handles the logic for the sine wave state.
 * The sampling rate and output to LCD are handled in this function.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void sine_logic(void)
{
    //Set state flag to sine wave state
    state_flag = SINE;

    //Display wave type and frequency on LCD
    LCD_CLR();
    LCD_HOME();
    WRITE_STR_LCD("Sine Wave ");
    SET_CUR_POS_LCD(Second_Line);
    WRITE_CHAR_LCD(freq_flag + 48);
    WRITE_STR_LCD("00 Hz");

    //Set CCR0 count for the correct sampling rate
    TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;      //stop timer
    TIMER_A0->CCR[0] = 74;                      //set CCR0 count
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;        //start timer
    return;
}

/*
 * Function that sets the frequency flag for the generated signals. Buttons
 * 1-5 determine the frequency, with 1 representing 100Hz, 2 representing 200Hz ...
 * INPUT:   NONE
 * RETURN:  NONE
 */
void set_frequency(void){
    if(button_press >= 49 && button_press <= 53)
        freq_flag = button_press - 48;
    return;
}


/*
 * Function that behaves as a finite state machine for the function generator. The
 * FSM has three states, square, sawtooth, and sine.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void FUNCTION_GENERATOR_FSM(void)
{
    //Variable definitions.
    struct control_FSM control;
    uint8_t NS;

    static uint8_t PS  = SQUARE;                //initial state, square
    uint8_t repeat = 1;                         //repeat flag initially set to 1
    set_frequency();                            //set frequency flag

    //While the repeat flag is enabled, iterate through the FSM and set
    //the present state and repeat flag accordingly
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
            case TRIANGLE:
                triangle_logic();
                control = check_NS(TRIANGLE);
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

    //Set DCO frequency to 24MHz
    set_DCO(FREQ_24_MHz);

    //Initialize SPI, TimerA, LCD, and keypad.
    SPI_INIT();
    INIT_TIMER();
    LCD_init();
    INIT_KEYPAD();

    //Enable interrupts globally
    __enable_irq();

   //Put the MCU to sleep to save power between interrupts
   SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk; // Do not wake up on exit from Int
   SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
   //Ensure that SLEEPDEEP occurs immediately
   __DSB();
   __sleep();

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


//TimerA0 ISR handler for interrupts generated at CCR0
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
                max_step = 399;
                break;
            case SQUARE:
                level = 0;
                break;
            case TRIANGLE:
                if (pos_neg_flag == 0x01)
                    level = (int)(step*20.5);
                else
                    level = 4095-(int)(step*20.5);
                max_step = 199;
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
    //reset interrupt flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

}

//TimerA0 ISR handler for interrupts generated at CCR1
void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG)
    {
      //Only do something if we are currently generating the square wave
      if (state_flag == SQUARE)
          //set the output high
          WRITE_DAC(4095);
    }
    //reset interrupt flag
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}
