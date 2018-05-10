#include "msp.h"
#include "dmm_functions.h"

void main(void){

    WDTCTL = WDTPW | WDTHOLD;

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
        __delay_cycles(100);
        NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
        __delay_cycles(100000);
        NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);


        wave_freq = get_captured_freq();
        low = get_low_voltage();
        high = get_high_voltage();

        reset_freq_flag();



    }
}

