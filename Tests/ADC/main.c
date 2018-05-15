#include "msp.h"
#include "dmm_functions.h"
#include "UART.h"
#include <stdio.h>

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
    init_AC_ADC();
    init_sample_timer(1000);
    P1->DIR |= BIT0;
    __enable_irq();
    __delay_cycles(10000);
    uint16_t test = get_sampled_rms();

    while(1);
}

/*
void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    //while(1);
    P1->DIR |= BIT0;
    init_ADC();
    SPI_INIT();
    init_freq_timer();
   // UART_init();
    __enable_irq();
    float wave_freq = 0;
    float low = 0;
    float high = 0;
    char high_val [50];



    while(1)
    {
        NVIC->ICER[0] = 1 << ((TA0_N_IRQn) & 31);
        //set_DC_offset();
        NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);

        __delay_cycles(30000);



        if (get_freq_flag()){
            __disable_irq();
            if (get_captured_freq() < 64000){
            wave_freq =  get_captured_freq();
            }
            reset_freq_flag();
            __enable_irq();
        }

        __delay_cycles(300000);


    }
}
*/


