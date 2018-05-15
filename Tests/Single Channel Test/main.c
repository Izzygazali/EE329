#include "msp.h"
#include "dmm_functions.h"

void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    P1->DIR |= BIT0;
    init_clock();
    init_AC_ADC();
    init_sample_timer();
    __enable_irq();

    while(1){
       // ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

        __delay_cycles(200000);
    }


}


