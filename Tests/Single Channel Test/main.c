#include "msp.h"
#include "dmm_functions.h"

void main(void){

    WDTCTL = WDTPW | WDTHOLD;

    init_clock();
    init_ADC();

    while(1);


}



