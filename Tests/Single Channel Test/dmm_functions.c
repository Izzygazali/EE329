 #include "dmm_functions.h"

volatile uint16_t adc_value[101];
volatile uint16_t adc_index = 0;
volatile uint32_t adc_sum = 0;

void init_clock(void)
{
    CS ->KEY = CS_KEY_VAL;
    CS ->CLKEN |= CS_CLKEN_ACLK_EN;
    CS ->KEY = 0;
    return;
}

void init_ADC(void)
{
    P5->SEL0 |= AIN;
    P5->SEL1 |= AIN;
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    __enable_irq();
    ADC14->CTL0 = ADC14_CTL0_SHT0__192 |
                  ADC14_CTL0_ON |
                  ADC14_CTL0_SHP |
                  ADC14_CTL0_CONSEQ_2 |
                  ADC14_CTL0_MSC |
                  ADC14_CTL0_DIV__8;
    ADC14->CTL1 = ADC14_CTL1_RES_1;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;
    ADC14->IER0 |= ADC14_IER0_IE0;
    return;
}




void ADC14_IRQHandler(void)
{
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0)
        if (adc_index < 100){
            adc_value[adc_index] = ADC14->MEM[0];
            adc_index++;
        }else{
            adc_index = 0;
        }
}



