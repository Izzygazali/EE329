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
    ADC14->CTL0 = ADC14_CTL0_SHT0__192 |
                  ADC14_CTL0_ON |
                  ADC14_CTL0_SHP;
    ADC14->CTL1 = ADC14_CTL1_RES_1;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
    ADC14->IER0 |= ADC14_IER0_IE0;
    return;
}

void init_sample_timer(void)
{
    P4->DIR |= BIT4;
    P4->SEL0 |= BIT4;
    P4->SEL1 &= ~BIT4;
    CS ->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    //Set Timer CLK to DCOCLK
    CS ->CTL0 |= CS_CTL0_DCORSEL_2;
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;
    //Disable writing to clock registers
    CS ->KEY = 0;
    TIMER_A0->CCTL[0] |=  TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCR[0] = 600;
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);


    return;
}



void ADC14_IRQHandler(void)
{
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0)
        if (adc_index < 21){
            adc_value[adc_index] = ADC14->MEM[0];
            adc_index++;
        }else{
            adc_index = 0;
        }
}

void TA0_0_IRQHandler(void)
{
    P1->OUT |= BIT0;
    if ((TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG))
    {
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    P1->OUT &= ~BIT0;
}


