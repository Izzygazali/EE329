/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 5/10/2018
 * Description: Program that implements a digital multimeter using the MSP432
 * ,MCP4921 DAC, and some other supporting hardware. The DMM is capable of 
 * measuring DC and other periodic waveforms between 0V and 3.3V. A more
 * detailed explanation of this project is included in the report.
 */

#include "msp.h"
#include "dmm_functions.h"
#include "UART.h"

//define states for program flow
enum state_type{
    reset_state,
    get_offset_DC,
    get_wave_freq,
    sample_wave,
    perform_calculations,
    display_results
};


//define events to control program flow
enum event_type{
    reset,
    DC_offset_set,
    wave_freq_set,
    wave_sampled,
    calculations_done,
    results_displayed
};

enum event_type event;

/*
 * Function that determines the current even based on flags
 * from dmm_functions.
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void DMM_state_decode(void)
{
    uint16_t curr_flags = get_dmm_flags() & ~dc_flag_set;
    switch(curr_flags)
    {
        case dc_offset_flag:
            event = DC_offset_set;
            break;
        case dc_offset_flag | wave_freq_flag:
            event = wave_freq_set;
            break;
        case dc_offset_flag | wave_freq_flag | sampling_done_flag:
            event = wave_sampled;
            break;
        case dc_offset_flag | wave_freq_flag | sampling_done_flag | calc_done_flag:
            event = calculations_done;
            break;
        case dc_offset_flag | wave_freq_flag | sampling_done_flag | calc_done_flag | results_displayed_flag:
            event = results_displayed;
            break;
        default:
            event = results_displayed;
            break;
    }
    return;
}

/*
 * Function that implments the logic for the dc offset
 * setting state.
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void dc_offset_logic(void)
{
    //setup ADC in Sequence of Channels Single Conversion for use
    //to get max and min from peak/valley circuit
    init_DC_ADC();
    //get the max and min value, average them, and set the DAC for
    //use in the "frequency generator"
    set_DC_offset();
    //disable the interrupts for "DC ADC"
    ADC14->IER0 &= ~ADC14_IER0_IE1;
    return;
}

/*
 * Function that implments the logic for determing
 * the frequency of the input wave (or if it is DC)
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void wave_freq_logic(void)
{
    //define variable for frequency timeout 
    uint32_t timeout_count = 0;
    //setup the timer for accurate low frequencies intially
    set_freq_slow();
    //set the frequency conversion for low frequencies
    set_freq_conversion(32000);
    //start capture-mode timer to get frequency
    init_freq_timer();
    //wait until flag indicates frequency has been determined
    while((get_dmm_flags() & wave_freq_flag) == 0){
        //increment timeout count.
        timeout_count++;
        //if timeout count reaches this the wave is assumed to be DC
        if (timeout_count > 5000000)
        {
            //set the DC flag and the freq found flag to make FSM move on
            set_dmm_flags(dc_flag_set | wave_freq_flag);
            //disable interrupts for frequency timer
            NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);
            return;
        }
    }
    //disable interrupts for frequency timer
    NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);
    //if frequency is "high frequency" then get it again this a faster timer
    if (get_captured_freq() > 100){
        //reset wave freq flag
        reset_dmm_flags(wave_freq_flag);
        //setup the timer for accurate high frequencies
        set_freq_fast();
        //set frequency conversion for high frequencies 
        set_freq_conversion(756400);
        //start timer
        init_freq_timer();
        //wait for flag to indicate frequency was found
        while((get_dmm_flags() & wave_freq_flag) == 0);
        //disable interrupts for frequency timer
        NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);
    }
    return;
}

/*
 * Function that implments the logic for sampling the
 * input waveform for performing calculations.
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void sample_wave_logic(void)
{
    //reset the index for the array holding sample values
    reset_ADC_index();
    //setup ADC in Single Channel Single Conversion for use
    //to sample input wave
    init_AC_ADC();
    //start timer with CCR0 count dependent on the frequency of the
    //input wave to get reasonable number of samples for all frequencies.
    init_sample_timer(get_captured_freq());
    //wait for flag to indicate sampling is complete
    while((get_dmm_flags() & sampling_done_flag) == 0);
    //disable sample timer and ADC interrupts
    ADC14->IER0 &= ~ADC14_IER0_IE3;
    NVIC->ICER[0] = 1 << ((TA0_0_IRQn) & 31);
    return;
}

/*
 * Function that implments the logic for performing the
 * calculations to get RMS, DC, AVERAGE, and MAX/MIN
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void perform_calculations_logic(void)
{
    //determine rms of input samples
    calc_sampled_rms();
    //determine DC average of input samples
    calc_sampled_DC();
    //determine maxmim and minimum of input samples
    calc_max_min();
    return;
}
 
/*
 * Function that implments the logic for displaying
 * the update data via a UART interface.
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void display_results_logic(void)
{
    //if the DC flag is set the display to "DC" mode
    if (get_dmm_flags() & dc_flag_set)
    {
        //0xFFFF corresponds to "----" in UART meaning the number doesn't apply to DC mode
        update_display(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, get_sampled_DC());
        //set flag indicating results have been displayed
        set_dmm_flags(results_displayed_flag);
        //delay ~1 second to slow down update rate
        __delay_cycles(24000000);
        return;
    }
    //update all relevant numbers on the UART display
    update_display(get_captured_freq(), get_max(), get_min(), get_sampled_rms(), get_sampled_DC());
    //set flag indicating results have been displayed
    set_dmm_flags(results_displayed_flag);
    //delay ~1 second to slow down update rate
    __delay_cycles(24000000);
    return;
}

/*
 * Function that implments the primary logic loop which
 * coordinates all functions to produce working digital 
 * multimeter.
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void DMM_FSM(void)
{
    //define local varible to contain the current state.
    static enum state_type state = get_offset_DC;
    switch(state)
    {
        case get_offset_DC:
            if(event == DC_offset_set){
                state = get_wave_freq;
                break;
            }
            //DC offset function
            dc_offset_logic();
            break;
        case get_wave_freq:
            if(event == wave_freq_set){
                state = sample_wave;
                break;
            }
            wave_freq_logic();
            break;
        case sample_wave:
            if(event == wave_sampled){
                state = perform_calculations;
                break;
            }
            //sample wave function
            sample_wave_logic();
            break;
        case perform_calculations:
            if(event == calculations_done){
                state = display_results;
                break;
            }
            //calculations function
            perform_calculations_logic();
            break;
        case display_results:
            if(event == results_displayed){
                state = reset_state;
                break;
            }
            //display results function
            display_results_logic();
            break;
        case reset_state:
            //reset variables
            reset_dmm_flags(0xFF);
            state = get_offset_DC;
            event = reset;
            break;
        default:
            state = reset_state;
            break;
    }
    return;
}

/*
 * Function that initializes various components
 * at startup for the digital multimeter.
 * INPUT:   NONE
 * RETURN:  NONE.
 */
void init_DMM(void)
{
    //set up LED as output to indicate when sampling is occuring
    P1->DIR |= BIT0;
    //enable interrupts globally
    __enable_irq();
    //startup needed clocks for various functions
    init_clock();
    //start up SPI interface for DAC
    SPI_INIT();
    //start up UART interface for displaying data
    UART_init();
    //initialize console for displaying data
    initialize_console();
    return;
}

void main(void)
{
    //disable watchdog timer
    WDTCTL = WDTPW | WDTHOLD;   
    //intialize components for DMM
    init_DMM();
    //repeat the state decoder and FSM to perform function of DMM
    while(1)
    {
        DMM_state_decode();
        DMM_FSM();
    }
}
