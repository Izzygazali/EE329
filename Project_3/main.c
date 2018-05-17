#include "msp.h"
#include "dmm_functions.h"
uint16_t freq = 0;
uint16_t rms = 0;

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


void DMM_STATE_DECODE(void)
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



void DMM_FSM(void)
{

    static enum state_type state = get_offset_DC;
        switch(state)
        {
            case get_offset_DC:
                if(event == DC_offset_set){
                    //NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);
                    state = get_wave_freq;
                    break;
                }
                //DC offset function
                init_DC_ADC();
                set_DC_offset();
                break;
            case get_wave_freq:
                if(event == wave_freq_set && freq != 0){
                    ADC14->IER0 &= ~ADC14_IER0_IE1;
                    NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);
                    state = sample_wave;
                    break;
                }
                __delay_cycles(24000000);
                freq = get_captured_freq();
                break;
            case sample_wave:
                if(event == wave_sampled){
                    state = perform_calculations;
                    break;
                }
                //sample wave function
                reset_ADC_index();
                init_AC_ADC();
                init_sample_timer(freq);
                while((get_dmm_flags() & sampling_done_flag) == 0);
                break;
            case perform_calculations:
                if(event == calculations_done){
                    state = display_results;
                    break;
                }
                //calculations function
                rms=get_sampled_rms();
            case display_results:
                //display results function
                if(event == results_displayed)
                    state = reset_state;
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


void main(void){
    //set ADC input for analog wave to be sampled

    WDTCTL = WDTPW | WDTHOLD;   //disable watchdog timer
    SPI_INIT();
    __enable_irq();
    init_clock();
    P1->DIR |= BIT0;
    WRITE_DAC(4095);

    init_freq_timer();


    while(1){
        DMM_STATE_DECODE();
        DMM_FSM();

    }

}



