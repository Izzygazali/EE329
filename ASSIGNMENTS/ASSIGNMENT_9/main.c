#include "msp.h"
#include "UART.h"

#define Ain BIT0
#define adc_flag_val 0x8000

//Bottom 14 bits are value from ADC
//Top bit is the flag for completion of conversion
uint16_t adc_val_and_flag;


void init_ADC(void)
{
    P6->SEL0 |= Ain;
    P6->SEL1 |= Ain;
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    __enable_irq();
    ADC14->CTL0 = ADC14_CTL0_SHT0_0 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;
    ADC14->IER0 |= ADC14_IER0_IE0;

    return;
}

char* convert_number_to_string(uint16_t input_number)
{
    char output_string[] = {0xFF,0xFF,0xFF,0xFF,0xFF};
    uint8_t temp_place_number;

    temp_place_number = (input_number % 100) >> 4;
    return (output_string);

}

void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    UART_init();
    init_ADC();
    convert_number_to_string(833);
  /*  uint16_t capture_adc_val = 0;

    while(1)
    {
        if (adc_val_and_flag & adc_flag_val){
            capture_adc_val = (adc_val_and_flag & ~adc_flag_val);

        }
    }*/
}

void ADC14_IRQHandler(void)
{

    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0){
        adc_val_and_flag |= (0x3FFF & ADC14->MEM[0]);
        adc_val_and_flag |= adc_flag_val;
    }
}

