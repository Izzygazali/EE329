#include "msp.h"
#include "dmm_functions.h"



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

    enum state_type state;
    state = get_offset_DC;

    while(1)
    {
        switch(state)
        {
            case get_offset_DC:
                //DC offset function
                set_DC_offset();
                reset_dmm_flags();
                if(event == DC_offset_set)

                    state = get_wave_freq;
                break;
            case get_wave_freq:
                //get freq function
                if(event == wave_freq_set)
                    state = sample_wave;
                break;
            case sample_wave:
                //sample wave function
                if(event == wave_sampled)
                    state = perform_calculations;
                break;
            case perform_calculations:
                //calculations function
                if(event == calculations_done)
                    state = display_results;
                break;
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
    }
}


void main(void){
    WDTCTL = WDTPW | WDTHOLD;   //disable watchdog timer
    SPI_INIT();
    __enable_irq();
    init_clock();

    WRITE_DAC(4095);
    init_DC_ADC();


    while(1){
        DMM_STATE_DECODE();
        DMM_FSM();

    }

}



