#include "dmm_functions.h"

//Definitions for DMM_FLAG:
//BIT 0 -> DC Offset Captured
//BIT 1 -> Wave Freq Captured
//BIT 2 -> Sampling Done
//BIT 3 -> Calculation Done
//BIT 4 -> Results displayed
//BIT 5 -> DC flag
volatile uint16_t dmm_flags = 0;

//variables for DC Offset
uint16_t dc_offset;
uint16_t dc_offset_avg;
volatile uint16_t high_value;
volatile uint16_t low_value;

//variables for frequency determination
volatile uint16_t captured_freq = 0;
uint32_t freq_conv_factor = 0;

//variables for sampling wave
volatile uint16_t adc_value[500];
volatile uint16_t adc_index = 0;

//variables for calculations
float rms = 0;
float DC = 0;
//intialize min at "max" value
uint16_t max = 0;
//intialize min at "max" value
uint16_t min = 16384;

//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for All Parts------------------------------------------
//-------------------------------------------------------------------------------------------------
/*
 * Function that returns current dmm_flags for use in FSM logic.
 * INPUT:   NONE
 * RETURN:  uint16_t dmm_flags = current dmm flags.
 */
uint16_t get_dmm_flags(void)
{
    //return flags for use in FSM
    return dmm_flags;
}

/*
 * Function that resets dmm_flags for use in FSM reset logic.
 * INPUT:   uint16_t flags = dmm flags to reset.
 * RETURN:  NONE
 */
void reset_dmm_flags(uint16_t flags)
{
    //reset flags for DMM
    dmm_flags &= ~flags;
}

/*
 * Function that sets dmm_flags for use in FSM reset logic.
 * INPUT:   uint16_t flags = dmm flags to set.
 * RETURN:  NONE
 */
void set_dmm_flags(uint16_t flags)
{
    //set flags for the DMM
    dmm_flags |= flags;
}

/*
 * Function that intializes clocks used for the ADC and TIMER As
 * INPUT:   NONE
 * RETURN:  NONE
 */
void init_clock(void)
{
    //unclock CS register
    CS ->KEY = CS_KEY_VAL;
    //clear CS register
    CS->CTL0 = 0;
    //set DCO to 24MHz
    CS ->CTL0 |= CS_CTL0_DCORSEL_4;
    //set REFO at 128kHz and SMCLK, MCLK to 24MHz
    CS ->CLKEN |= CS_CLKEN_ACLK_EN | CS_CLKEN_REFOFSEL;
    //divide ACLK by four for 32kHz clock
    CS->CTL1 |= CS_CTL1_SELA__REFOCLK | CS_CTL1_DIVA_2 |
                CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
    //lock CS register
    CS ->KEY = 0;
    return;
}

//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for DC Offset------------------------------------------
//-------------------------------------------------------------------------------------------------
/*
 * Function that intializes ADC for determining the DC offset from external hardware
 * INPUT:   NONE
 * RETURN:  NONE
 */
void init_DC_ADC(void)
{
    //enable ADC inputs for measuring peak and valley voltages from circuit
    P5->SEL0 |= (PEAK + VALLEY);
    P5->SEL1 |= (PEAK + VALLEY);
    //enable interrupts for ADC
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    //setup relevant parameters for ADC
    ADC14->CTL0 = ADC14_CTL0_SHT0_0 |
                  ADC14_CTL0_ON |
                  ADC14_CTL0_SHP| ADC14_CTL0_CONSEQ_3;
    //set resolution to 14bit
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    //enable input channels for peak and valley pin
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_0;
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_3 | ADC14_MCTLN_EOS;
    //enable interrupts for both peak and valley pins
    ADC14->IER0 |= ADC14_IER0_IE1;
    return;
}

/*
 * Function resets external peak/valley detector and determines the DC_offset.
 * Then the DC offset is written to the DAC for the comparator.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void set_DC_offset(void)
{
    //Reset the capacitors in the circuit by "shorting" them
    P5->DIR |= PEAK_RESET;
    P5->DIR |= VALLEY_RESET;
    P5->OUT &= ~PEAK_RESET;
    P5->OUT |= VALLEY_RESET;
    //Set pins to input to "disconnect" them from the circuit
    P5->DIR &= ~PEAK_RESET;
    P5->DIR &= ~VALLEY_RESET;

    uint16_t avg_index = 0;
    uint32_t dc_offset_acc = 0;

    while(avg_index < 100){
        //sample the voltages to determine DC offset
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        //delay for samples to be captured
        __delay_cycles(10);
        //calculate DC offset
        dc_offset = (high_value+low_value) >> 1;
        //sum DC offset for averaging
        dc_offset_acc += dc_offset;
        avg_index++;
    }
    //determine average of measured DC offsets
    dc_offset_avg =  dc_offset_acc/avg_index;
    //write DC offset to DAC for use in circuit
    //shifted by to since DAC accepts 12 bit number
    WRITE_DAC(dc_offset_avg >> 2);
    //set flag indicating dc_offset has been set
    dmm_flags |= dc_offset_flag;
    return;
}


//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for Frequency of Wave----------------------------------
//-------------------------------------------------------------------------------------------------

/*
 * Function sets the factor that converts counter "counts" into frequency
 * values.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void set_freq_conversion(uint32_t input_conv_factor)
{
    freq_conv_factor = input_conv_factor;
    return;
}

/*
 * Function returns the frequency of the input wave in Hz.
 * INPUT:   NONE
 * RETURN:  uint16_t frequency in Hz
 */
uint16_t get_captured_freq(void)
{
    //return the frequency of the input analog wave
    return freq_conv_factor/captured_freq;
}

/*
 * Function that sets relevant parameters for timing "fast" signals.
 * Another words over 100Hz.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void set_freq_fast(void)
{
    //clear timer A0 settings
    TIMER_A0->CTL = 0;
    TIMER_A0->EX0 = 0;
    //set parameters for a 750kHz clock sourced from SMCLK
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK |
                     TIMER_A_CTL_MC__CONTINUOUS |
                     TIMER_A_CTL_CLR |
                     TIMER_A_CTL_ID__4;
    TIMER_A0->EX0 |= TIMER_A_EX0_IDEX__8;
    return;
}

/*
 * Function that sets relevant parameters for timing "slow" signals.
 * Another words under 100Hz.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void set_freq_slow(void)
{
    TIMER_A0->CTL = 0;
    TIMER_A0->EX0 = 0;
     //set parameters for a 32kHz clock sourced from ACLK
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__ACLK |
                     TIMER_A_CTL_MC__CONTINUOUS |
                     TIMER_A_CTL_CLR;
    return;
}

/*
 * Function that sets and starts the capture mode timer which
 * determines the frequency from the square wave produced by our
 * external hardware.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void init_freq_timer(void)
{
    //initialize pin used for capture from circuit
    P2->SEL0 |= INPUT_FREQ;
    P2->SEL1 &= ~INPUT_FREQ;
    P2->DIR  &= ~INPUT_FREQ;

    //setup capture mode parameters
    TIMER_A0->CCTL[2] |= TIMER_A_CCTLN_CAP | TIMER_A_CCTLN_CM_1 |
                         TIMER_A_CCTLN_CCIS_0 | TIMER_A_CCTLN_CCIE |
                         TIMER_A_CCTLN_SCS;
    //enable interrupts for capture mode on (2)
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
    return;
}

//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for Sampling of Wave-----------------------------------
//-------------------------------------------------------------------------------------------------

/*
 * Function that resets the ADC index to overwrite old samples.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void reset_ADC_index(void)
{
    adc_index = 0;
    return;
}

/*
 * Function that intializes ADC for determining the samples of the input wave.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void init_AC_ADC(void)
{
    //clear previous ADC settings
    ADC14->CTL0 = 0;
    ADC14->CTL1 = 0;
    ADC14->MCTL[0] = 0;
    ADC14->MCTL[1] = 0;
    //enable interrupts on ADC
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    //set parameters for ADC (single channel single conversion)
    ADC14->CTL0 = ADC14_CTL0_SHT0_0 |
                  ADC14_CTL0_ON |
                  ADC14_CTL0_SHP;
    //set resolution to 14 bit
    ADC14->CTL1 = ADC14_CTL1_RES_3 | (3 << ADC14_CTL1_CSTARTADD_OFS);
    //set input channel
    ADC14->MCTL[3] |= ADC14_MCTLN_INCH_1 | ADC14_MCTLN_EOS;
    //enable interrupts on MEMO[0]
    ADC14->IER0 |= ADC14_IER0_IE3;
    return;
}

/*
 * Function that intializes timer which sets sample rate for a period given the
 * input waves frequency.
 * INPUT:   uint16_t freq = frequency of input wave.
 * RETURN:  NONE
 */
void init_sample_timer(uint16_t freq)
{
    //determine count for 6MHz clock timer
    //(assuming wave is less than 500Hz)
    uint16_t CCR0_count;

    if (get_dmm_flags() & dc_flag_set){
        //if DC signal set count for 1ms sampling time
        CCR0_count = 6000;
    }else{
        CCR0_count = 6000000/(100*freq);
    }

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
/*
 * Function that returns the RMS value of the input wave.
 * INPUT:   NONE
 * RETURN:  uint16_t rms in volts
 */
uint16_t get_sampled_rms(void)
{
    return rms*volt_conv_factor;
}
/*
 * Function that returns the DC Average value of the input wave.
 * INPUT:   NONE
 * RETURN:  uint16_t DC in volts
 */
uint16_t get_sampled_DC(void)
{
    return DC*volt_conv_factor;
}

/*
 * Function that returns the maximum value of the input wave.
 * INPUT:   NONE
 * RETURN:  uint16_t max in volts
 */
uint16_t get_max(void)
{
    return max*volt_conv_factor;
}

/*
 * Function that returns the minimum value of the input wave.
 * INPUT:   NONE
 * RETURN:  uint16_t min in volts
 */
uint16_t get_min(void)
{
    return min*volt_conv_factor;
}

/*
 * Function that determines minimum and maximum of input wave samples.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void calc_max_min(void)
{
     //define variables relevant for determing max and min
     uint16_t max_min_index;
     max = 0;
     min = 16384;

     //shift through sample array
     for (max_min_index = 0; max_min_index < 500; max_min_index++){
         //find minimum value in array
         if (adc_value[max_min_index] < min)
             min = adc_value[max_min_index];
         //find maximum value in array
         if (adc_value[max_min_index] > max)
             max = adc_value[max_min_index];
     }
     return;
}

/*
 * Function that determines DC average of input wave samples.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void calc_sampled_DC(void)
{
    uint16_t DC_index;
    DC = 0;
    //sum elements in the input wave samples.
    for (DC_index = 0; DC_index < 500; DC_index++){
        DC += adc_value[DC_index];
    }
    //determine number of samples in sample array
    uint16_t sample_num = sizeof(adc_value)/sizeof(adc_value[0]);
    //average for the DC value of the input wave
    DC /= sample_num;
    return;
}

/*
 * Function that determines RMS of input wave samples.
 * INPUT:   NONE
 * RETURN:  NONE
 */
void calc_sampled_rms(void)
{
    uint16_t rms_index;
    uint32_t inter_rms = 0;
    rms = 0;
    //sum the square of the input wave samples
    for (rms_index = 0; rms_index < 500; rms_index++){
        inter_rms = adc_value[rms_index];
        rms += inter_rms*inter_rms;
    }
    //determine the number of samples in the array
    uint16_t sample_num = sizeof(adc_value)/sizeof(adc_value[0]);
    //average
    rms /= sample_num;
    //take sqrt to get the rms value
    rms = sqrt(rms);
    //set flag indicating calculation are done
    dmm_flags |= calc_done_flag;
    return;
}

//-------------------------------------------------------------------------------------------------
//--------------------------------------Interrupt Handlers-----------------------------------------
//-------------------------------------------------------------------------------------------------
void ADC14_IRQHandler(void)
{
    //if we are in the DC offset determination state
    if ((ADC14->IFGR0 & ADC14_IFGR0_IFG0))
        //save high dc value for dc offset calculation
        high_value = ADC14->MEM[0];
    //else if we are in sampling state
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1)
        //save low dc value for dc offset calculation
        low_value = ADC14->MEM[1];
    if ((ADC14->IFGR0 & ADC14_IFGR0_IFG3)){

        //if array isn't full
        if (adc_index < 500){
            //add samples to array
            adc_value[adc_index] = ADC14->MEM[3];
            //increment index
            adc_index++;
        }else{
            //if array is full write over old data
            dmm_flags |= sampling_done_flag;
            adc_index = 0;
        }
    }
}

void TA0_N_IRQHandler(void)
{
    //variable for "counting" frequency of input wave
    static volatile uint32_t captureCount = 0;
    static volatile uint16_t captureValues[2] = {0,0};

    if ((TIMER_A0->CCTL[2] & TIMER_A_CCTLN_CCIFG))
    {
        //save value for 2 points
        captureValues[captureCount] = TIMER_A0->CCR[2];
        captureCount++;
        //if 2 points have been collected
        if (captureCount >= 2)
        {
            //set frequency found flag
            if ((captureValues[1] - captureValues[0]) > 0)
            {
                dmm_flags |= wave_freq_flag;
                captured_freq = (captureValues[1] - captureValues[0]);
                captureValues[0] = 0;
                captureValues[1] = 0;
            }
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



