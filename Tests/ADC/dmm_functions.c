 #include "dmm_functions.h"

volatile uint16_t high_value;
volatile uint16_t low_value;
volatile uint16_t captureValues[2];
volatile uint16_t dmm_flags = 0;

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
    return 64000/(captureValues[1] - captureValues[0]);
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

void init_freq_timer(void)
{
    CS->KEY = CS_KEY_VAL;
    CS->CLKEN |= CS_CLKEN_REFOFSEL;
    CS->CTL1 |= CS_CTL1_SELA__REFOCLK | CS_CTL1_DIVA_1;
    CS->KEY = 0;
    P2->SEL0 |= INPUT_FREQ;
    P2->SEL1 &= ~INPUT_FREQ;
    P2->DIR  &= ~INPUT_FREQ;
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__CONTINUOUS | TIMER_A_CTL_CLR;
    TIMER_A0->CCTL[2] |= TIMER_A_CCTLN_CAP | TIMER_A_CCTLN_CM_1 | TIMER_A_CCTLN_CCIS_0 | TIMER_A_CCTLN_CCIE | TIMER_A_CCTLN_SCS;
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
    __enable_irq();
    return;
}

void init_ADC(void)
{
    P6->SEL0 |= (PEAK + VALLEY);
    P6->SEL1 |= (PEAK + VALLEY);
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    __enable_irq();
    ADC14->CTL0 = ADC14_CTL0_SHT0_0 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1;
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_14;
    ADC14->IER0 |= ADC14_IER0_IE0;
    ADC14->IER0 |= ADC14_IER0_IE1;

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
   __delay_cycles(3000000);
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    __delay_cycles(200);
    dc_offset = (high_value+low_value) >> 1;
    WRITE_DAC(dc_offset);
    return;
}

void ADC14_IRQHandler(void)
{
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0)
        high_value = ADC14->MEM[0];
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1)
        low_value = ADC14->MEM[1];
}

void TA0_N_IRQHandler(void)
{
    static volatile uint16_t captureCount = 0;
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
