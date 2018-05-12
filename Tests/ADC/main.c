#include "msp.h"
#include "dmm_functions.h"

void running_avg_freq(uint16_t current_freq)
{
    static uint8_t freq_index = 1;
    static float accumulated_freq = 0;
    static float   avg_freq = 0;

    accumulated_freq += current_freq;
    avg_freq = accumulated_freq/freq_index;

    freq_index++;
    if (freq_index > 5){
        freq_index = 1;
        accumulated_freq = 0;
    }
    return;
}



void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    //while(1);
    P1->DIR |= BIT0;
    init_ADC();
    SPI_INIT();
    init_freq_timer();
    __enable_irq();
    float wave_freq = 0;
    float low = 0;
    float high = 0;



    while(1)
    {

        set_DC_offset();






        if (get_freq_flag()){
            __disable_irq();
            if (get_captured_freq() < 64000 && get_captured_freq() > 60){
                running_avg_freq(get_captured_freq());
            }
            low = get_low_voltage();
            high = get_high_voltage();
            reset_freq_flag();
            __enable_irq();

        }

        __delay_cycles(10000);


    }
}


