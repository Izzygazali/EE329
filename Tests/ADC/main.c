#include "msp.h"
#include "DAC.h"

#define PEAK BIT0
#define VALLEY BIT1

uint16_t high_value;
uint16_t low_value;

void init_ADC(void)
{
    P6->SEL0 |= (PEAK + VALLEY);
    P6->SEL1 |= (PEAK + VALLEY);

    __enable_irq();
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);

    ADC14->CTL0 = ADC14_CTL0_SHT0_0 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1;
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_14;
    ADC14->IER0 |= ADC14_IER0_IE0;
    ADC14->IER0 |= ADC14_IER0_IE1;


    return;
}

void get_DC_offset(void)
{
    uint16_t dc_offset;
    P5->DIR |= PEAK;
    P5->OUT &= ~PEAK;
    P5->DIR &= ~PEAK;
    P5->DIR |= VALLEY;
    P5->OUT |= VALLEY;
    P5->DIR &= ~VALLEY;
    __delay_cycles(1000);
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    __delay_cycles(1000);
    dc_offset = (high_value+low_value) >> 1;
    WRITE_DAC(dc_offset);
    return;
}



void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    init_ADC();
    SPI_INIT();
    P1->OUT &= ~BIT0;
    P1->DIR |= BIT0;
    get_DC_offset();
    while(1);

}

void ADC14_IRQHandler(void) {
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0)
        high_value = ADC14->MEM[0];
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1)
        low_value = ADC14->MEM[1];
}
