#include "msp.h"
#include "dmm_functions.h"
#include "UART.h"

uint16_t freq = 0;
uint16_t rmsTEST = 0;

//define states
enum state_type{
    reset_state,
    get_offset_DC,
    get_wave_freq,
    sample_wave,
    perform_calculations,
    display_results
};


//define events
enum event_type{
    reset,
    DC_offset_set,
    wave_freq_set,
    wave_sampled,
    calculations_done,
    results_displayed
};

enum event_type event;


void DMM_state_decode(void)
{
    uint16_t curr_flags = get_dmm_flags();
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


void dc_offset_logic(void)
{
    init_DC_ADC();
    set_DC_offset();
    ADC14->IER0 &= ~ADC14_IER0_IE1;
    return;
}

void wave_freq_logic(void)
{
    init_freq_timer();
    while((get_dmm_flags() & wave_freq_flag) == 0);
    NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);
    freq = get_captured_freq();
    return;
}

void sample_wave_logic(void)
{
    reset_ADC_index();
    init_AC_ADC();
    init_sample_timer(get_captured_freq());
    while((get_dmm_flags() & sampling_done_flag) == 0);
    ADC14->IER0 &= ~ADC14_IER0_IE3;
    NVIC->ICER[0] = 1 << ((TA0_0_IRQn) & 31);
    return;
}

void perform_calculations_logic(void)
{
    calc_sampled_rms();
    calc_sampled_DC();
    calc_max_min();
    return;
}

void DMM_FSM(void)
{
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
            update_display(get_captured_freq(), get_max(), get_min(), get_sampled_rms(), get_sampled_DC());
            set_dmm_flags(results_displayed_flag);
            __delay_cycles(24000000);
            break;
        case reset_state:
            //reset variables
            reset_dmm_flags();
            state = get_offset_DC;
            event = reset;
            break;
        default:
            state = reset_state;
            break;
    }
    return;
}

void init_DMM(void)
{
    P1->DIR |= BIT0;
    __enable_irq();
    init_clock();
    SPI_INIT();
    UART_init();
    initialize_console();
    return;
}


void main(void)
{
    //set ADC input for analog wave to be sampled

    WDTCTL = WDTPW | WDTHOLD;   //disable watchdog timer
    init_DMM();
    while(1)
    {
        DMM_state_decode();
        DMM_FSM();
    }
}



