 #include "dmm_functions.h"
#include <math.h>


volatile uint16_t high_value;
volatile uint16_t low_value;
volatile uint16_t captureValues[2];
volatile uint16_t dmm_flags = 0;
volatile uint16_t adc_value[200];


uint8_t state = 1;

float get_high_voltage(void)
{
    return high_value*0.08;
}

float get_low_voltage(void)
{
    return low_value*0.08;
}

uint16_t get_captured_freq(void)
{
    return (captureValues[1] - captureValues[0]);
}

uint8_t get_freq_flag(void)
{
    return (dmm_flags & FREQ_FLAG);
}

void reset_freq_flag(void)
{
    dmm_flags &= ~FREQ_FLAG;
    return;
}

void init_clock(void)
{
    CS ->KEY = CS_KEY_VAL;
    CS ->CLKEN |= CS_CLKEN_ACLK_EN | CS_CLKEN_REFOFSEL;
    CS->CTL1 |= CS_CTL1_SELA__REFOCLK | CS_CTL1_DIVA_2;
    CS ->KEY = 0;
    return;
}

void init_freq_timer(void)
{
    P2->SEL0 |= INPUT_FREQ;
    P2->SEL1 &= ~INPUT_FREQ;
    P2->DIR  &= ~INPUT_FREQ;
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__CONTINUOUS | TIMER_A_CTL_CLR;
    TIMER_A0->CCTL[2] |= TIMER_A_CCTLN_CAP | TIMER_A_CCTLN_CM_1 | TIMER_A_CCTLN_CCIS_0 | TIMER_A_CCTLN_CCIE | TIMER_A_CCTLN_SCS;
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
    return;
}

void init_DC_ADC(void)
{
    P6->SEL0 |= (PEAK + VALLEY);
    P6->SEL1 |= (PEAK + VALLEY);
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    ADC14->CTL0 = ADC14_CTL0_SHT0_0 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1;
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_14;
    ADC14->IER0 |= ADC14_IER0_IE0;
    ADC14->IER0 |= ADC14_IER0_IE1;
    return;
}

void init_AC_ADC(void)
{
    P5->SEL0 |= AIN;
    P5->SEL1 |= AIN;
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    ADC14->CTL0 = ADC14_CTL0_SHT0_0 |
                  ADC14_CTL0_ON |
                  ADC14_CTL0_SHP;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
    ADC14->IER0 |= ADC14_IER0_IE0;
    return;
}

void init_sample_timer(uint16_t freq)
{
    uint16_t CCR0_count;
    CCR0_count = 6000000/(100*freq);

    CS ->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS ->CTL0 |= CS_CTL0_DCORSEL_4;
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
    CS ->KEY = 0;

    TIMER_A0->CTL = 0;

    if (freq >= 500){
        TIMER_A0->CTL |= TIMER_A_CTL_ID_1;
        CCR0_count *= 2;
    }
    else{
        TIMER_A0->CTL |= TIMER_A_CTL_ID_2;
    }

    TIMER_A0->CCTL[0] |=  TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCR[0] = CCR0_count;
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    return;
}



void set_DC_offset(void)
{
    uint16_t dc_offset;
    P5->DIR |= PEAK;
    P5->OUT &= ~PEAK;
    P5->DIR &= ~PEAK;
    P5->DIR |= VALLEY;
    P5->OUT |= VALLEY;
    P5->DIR &= ~VALLEY;
   //__delay_cycles(3000);
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
   // __delay_cycles(30000);
    dc_offset = (high_value+low_value) >> 1;
    WRITE_DAC(dc_offset);
    return;
}

void ADC14_IRQHandler(void)
{
    P1->OUT |= BIT0;
    static volatile uint16_t adc_index = 0;
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0 & state == 0)
        high_value = ADC14->MEM[0];
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1)
        low_value = ADC14->MEM[1];
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0 & state == 1)
    {
        if (adc_index < 200){
          adc_value[adc_index] = ADC14->MEM[0];
          adc_index++;
        }else{
            __disable_irq();
          //adc_index = 0;
        }
    }
    P1->OUT &= ~BIT0;
}

void TA0_N_IRQHandler(void)
{
    static volatile uint32_t captureCount = 0;
    if ((TIMER_A0->CCTL[2] & TIMER_A_CCTLN_CCIFG))
    {
        captureValues[captureCount] = TIMER_A0->CCR[2];
        captureCount++;
        if (captureCount >= 2)
        {
            dmm_flags |= 0x01;
            captureCount = 0;
        }
    }

    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
}

void TA0_0_IRQHandler(void)
{
    if ((TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG))
    {
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}


float get_sampled_rms(void)
{
    __disable_irq();

    int i;
    float rms = 0;
    uint32_t temp = 0;

    for (i = 0; i < 200; i++){
        temp = adc_value[i];
        rms += temp*temp;
    }
    uint16_t n = sizeof(adc_value)/sizeof(adc_value[0]);
    rms /= n;
    rms = sqrt(rms);
    __enable_irq();
    return rms;
}
