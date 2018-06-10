#include "ADC.h"

uint16_t adc_val_and_flag;

uint16_t get_adc_val_and_flag(void)
{
    return adc_val_and_flag;
}

void reset_adc_flag(void)
{
    adc_val_and_flag &= ~adc_flag;
    return;
}

void init_ADC(void)
{
    P5->SEL0 |= ANALOG_IN;
    P5->SEL1 |= ANALOG_IN;

    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    __enable_irq();

    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
    ADC14->IER0 |= ADC14_IER0_IE0;
    return;
}

void ADC14_IRQHandler(void)
{
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0)
        adc_val_and_flag =  (ADC14->MEM[0] | adc_flag);
}
