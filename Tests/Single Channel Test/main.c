#include "msp.h"
#include "dmm_functions.h"

void main(void){

    WDTCTL = WDTPW | WDTHOLD;

    init_clock();
    init_ADC();
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;

    while(1){
        __delay_cycles(200000);
    }


}



