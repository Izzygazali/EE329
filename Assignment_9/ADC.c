#include "ADC.h"

static uint8_t sample_flag = 0;
static uint16_t sample = 0;
static uint16_t voltage_level_mV = 0;


uint16_t get_sample(void){
    return sample;
}

uint8_t get_sample_flag(void){
    return sample_flag;
}

void set_sample_flag(uint8_t input_val){
    sample_flag = input_val;
}

uint8_t sample_to_voltage(void){
    voltage_level_mV = (sample * CONVERSION_FACTOR);
    return voltage_level_mV;
}

void ADC_init(void){
    //configure GPIO
    P5 -> SEL0 |= BIT4;
    P5 -> SEL1 |= BIT4;

    ADC14 -> CTL0 = ADC14_CTL0_SHT0_2   |       //sample for 16 cycles
                    ADC14_CTL0_ON       |       //power on
                    ADC14_CTL0_SHP;             //sample & hold pulse mode
    ADC14 -> CTL1 = ADC14_CTL1_RES_3;           //14-bit conversion
    ADC14 -> MCTL[0] = ADC14_MCTLN_INCH_1;      //A1 ADC input select
    ADC14 -> IER0 |= ADC14_IER0_IE0;            //interrupt enable
    ADC14 -> CTL0 |= ADC14_CTL0_ENC;


    NVIC -> ISER[0] = (1 << ADC14_IRQn);
    __enable_irq();
}

void ADC14_IRQHandler(void){
    if(ADC14 -> IFGR0 & ADC14_IFGR0_IFG0){
        sample = ADC14 -> MEM[0];
        sample_flag = 1;
    }
}
