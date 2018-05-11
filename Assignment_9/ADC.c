#include "ADC.h"


void ADC_init(void){
    //configure GPIO
    P5 -> SEL0 |= BIT4;
    P5 -> SEL1 |= BIT4;

    ADC14 -> CTL0


}
