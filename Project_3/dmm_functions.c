 #include "dmm_functions.h"


//Variables for all parts
//BIT 0 -> DC Offset Captured
//BIT 1 -> Wave Freq Captured
//BIT 2 -> Sampling Done
//BIT 3 -> Calculation Done
//BIT 4 -> Results displayed
volatile uint16_t dmm_flags = 0;

//variables for DC Offset
uint16_t dc_offset;
volatile uint16_t high_value;
volatile uint16_t low_value;

//variables for frequency determination
volatile uint16_t captureValues[2];

//variables for sampling wave
volatile uint16_t adc_value[200];
static volatile uint16_t adc_index = 0;
//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for All Parts------------------------------------------
//-------------------------------------------------------------------------------------------------
uint16_t get_dmm_flags(void)
{
    //return flags for use in FSM
    return dmm_flags;
}

void reset_dmm_flags(void)
{
    //reset flags for DMM
    dmm_flags = 0;
}

void set_dmm_flags(uint16_t flags)
{
    dmm_flags |= flags;
}

void init_clock(void)
{
    CS ->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS ->CTL0 |= CS_CTL0_DCORSEL_4;
    CS ->CLKEN |= CS_CLKEN_ACLK_EN | CS_CLKEN_REFOFSEL;
    CS->CTL1 |= CS_CTL1_SELA__REFOCLK | CS_CTL1_DIVA_2 |
                CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
    CS ->KEY = 0;
    return;
}
//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for DC Offset------------------------------------------
//-------------------------------------------------------------------------------------------------
uint16_t get_DC_offset(void)
{
    //return DC offset value in volts
    return dc_offset*0.8;
}
uint16_t get_high_voltage(void)
{
    //return DC high value in volts
    return high_value*0.8;
}

uint16_t get_low_voltage(void)
{
    //return DC low value in volts
    return low_value*0.8;
}

void init_DC_ADC(void)
{
    //enable ADC inputs for measuring peak and valley voltages from circuit
    P6->SEL0 |= (PEAK + VALLEY);
    P6->SEL1 |= (PEAK + VALLEY);
    //enable interrupts for ADC
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    //setup relevant parameters for ADC
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_3;
    //set resolution to 12bit (same as DAC)
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    //enable input channels for peak and valley pin
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_15;
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_14 | ADC14_MCTLN_EOS;
    //enable interrupts for both peak and valley pins
    ADC14->IER0 |= ADC14_IER0_IE1;
    return;
}

void set_DC_offset(void)
{
    __disable_irq();
    //Reset the capacitors in the circuit by "shorting" them
    P5->DIR |= PEAK;
    P5->DIR |= VALLEY;
    P5->OUT &= ~PEAK;
    P5->OUT |= VALLEY;
    //Set pins to input to "disconnect" them from the circuit
    P5->DIR &= ~PEAK;
    P5->DIR &= ~VALLEY;
    __enable_irq();
    //delay for capacitors to charge/discharge to extreme voltages

    int i = 0;
    static uint16_t count = 0;
    static uint32_t dc_acm = 0;

    while(i < 10000){
        //sample the voltages to determine DC offset
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        //delay for samples to be captured
        //calculate DC offset
        dc_offset = (high_value+low_value) >> 1;
        dc_acm += dc_offset;
        count++;
        i++;
    }
    uint32_t dc_avg = dc_acm/(count);
    //write DC offset to DAC for use in circuit
     WRITE_DAC(dc_avg);
    //dmm_flags |= dc_offset_flag;
    return;
}


//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for Frequency of Wave----------------------------------
//-------------------------------------------------------------------------------------------------
uint16_t get_captured_freq(void)
{
    //return the frequency of the input analog wave
    return 32000/(captureValues[1] - captureValues[0]);
}

void init_freq_timer(void)
{
    //initialize pin used for capture from circuit
    P2->SEL0 |= INPUT_FREQ;
    P2->SEL1 &= ~INPUT_FREQ;
    P2->DIR  &= ~INPUT_FREQ;
    //Setup mode and clock source for the capture timer
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__ACLK |
                     TIMER_A_CTL_MC__CONTINUOUS |
                     TIMER_A_CTL_CLR;
    //setup capture mode parameters
    TIMER_A0->CCTL[2] |= TIMER_A_CCTLN_CAP | TIMER_A_CCTLN_CM_1 |
                         TIMER_A_CCTLN_CCIS_0 | TIMER_A_CCTLN_CCIE |
                         TIMER_A_CCTLN_SCS;
    //enable interrupts for capture mode on (2)
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
    return;
}


//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for Sampling of Wave-----------------------------------
//-------------------------------------------------------------------------------------------------
void reset_ADC_index(void)
{
    adc_index = 0;
    return;
}
void init_AC_ADC(void)
{
    //set ADC input for analog wave to be sampled
    P5->SEL0 |= INPUT_WAVE;
    P5->SEL1 |= INPUT_WAVE;
    //enable interrupts on ADC
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    //set parameters for ADC (single channel)
    ADC14->CTL0 = ADC14_CTL0_SHT0_0 |
                  ADC14_CTL0_ON |
                  ADC14_CTL0_SHP;
    //set resolution to 14 bit
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    //set input channel
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
    //enable interrupts on MEMO[0]
    ADC14->IER0 |= ADC14_IER0_IE0;
    return;
}

void init_sample_timer(uint16_t freq)
{
    //determine count for 6MHz clock timer
    //(assuming wave is less than 500Hz)
    uint16_t CCR0_count;
    CCR0_count = 6000000/(100*freq);
    //clear TIMER CTL register
    TIMER_A0->CTL = 0;
    if (freq >= 500){
        //if frequency is "high" set timer to 12MHz and double CCR count
        TIMER_A0->CTL |= TIMER_A_CTL_ID_1;
        CCR0_count *= 2;
    }else{
        //if frequency is "low" set clk to 6MHz
        TIMER_A0->CTL |= TIMER_A_CTL_ID_2;
    }
    //enable interrupts on timer A and rest flag
    TIMER_A0->CCTL[0] |=  TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    //set CCR count for desired sampling rate
    TIMER_A0->CCR[0] = CCR0_count;
    //set clock source and up mode for timer A
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;
    //enable interrupts on Timer A0
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    return;
}
//-------------------------------------------------------------------------------------------------
//-----------------------------------Functions for Calculations------------------------------------
//-------------------------------------------------------------------------------------------------
float get_sampled_rms(void)
{
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
    return rms;
}

//-------------------------------------------------------------------------------------------------
//--------------------------------------Interrupt Handlers-----------------------------------------
//-------------------------------------------------------------------------------------------------
void ADC14_IRQHandler(void)
{
    //if we are in the DC offset determination state
    if ((ADC14->IFGR0 & ADC14_IFGR0_IFG0)){ //& ~(dmm_flags & dc_offset_flag)){
        //save high dc value for dc offset calculation
        high_value = ADC14->MEM[0];
    }
    //else if we are in sampling state
    else if (dmm_flags & dc_offset_flag)
    {
        //if array isn't full
        if (adc_index < 200){
            //add samples to array
            adc_value[adc_index] = ADC14->MEM[0];
            //increment index
            adc_index++;
        }else{
            //if array is full write over old data

            adc_index = 0;
        }
    }
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1)
        //save low dc value for dc offset calculation
        low_value = ADC14->MEM[1];
}

void TA0_N_IRQHandler(void)
{
    //variable for "counting" frequency of input wave
    static volatile uint32_t captureCount = 0;
    if ((TIMER_A0->CCTL[2] & TIMER_A_CCTLN_CCIFG))
    {
        //save value for 2 points
        captureValues[captureCount] = TIMER_A0->CCR[2];
        captureCount++;
        //if 2 points have been collected
        if (captureCount >= 2)
        {
            //set frequency found flag
            dmm_flags |= wave_freq_flag;
            //reset the capture count for next time
            captureCount = 0;
        }
    }
    //reset timer flag
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
}

void TA0_0_IRQHandler(void)
{
    //if sampling timer has caused this interrupt
    if ((TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG))
    {
        //sample the wave
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    }
    //reset timer flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}



