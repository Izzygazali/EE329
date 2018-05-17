#include "msp.h"
#include "DAC.h"
#include <stdlib.h>
#include <math.h>

//inputs defined
#define PEAK_RESET BIT0
#define VALLEY_RESET BIT1
#define PEAK BIT2
#define VALLEY BIT5


#define INPUT_FREQ BIT5
#define INPUT_WAVE BIT4

//flag bits defined
#define dc_offset_flag BIT0
#define wave_freq_flag BIT1
#define sampling_done_flag BIT2
#define calc_done_flag BIT3
#define results_displayed_flag BIT4

#define volt_conv_factor 0.201416
//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for All Parts------------------------------------------
//-------------------------------------------------------------------------------------------------
uint16_t get_dmm_flags(void);
void reset_dmm_flags(void);
void set_dmm_flags(uint16_t flags);
void init_clock(void);
//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for DC Offset------------------------------------------
//-------------------------------------------------------------------------------------------------
uint16_t get_DC_offset(void);
uint16_t get_high_voltage(void);
uint16_t get_low_voltage(void);
void init_DC_ADC(void);
void set_DC_offset(void);
//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for Frequency of Wave----------------------------------
//-------------------------------------------------------------------------------------------------
uint16_t get_captured_freq(void);
void init_freq_timer(void);
//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for Sampling of Wave-----------------------------------
//-------------------------------------------------------------------------------------------------
void reset_ADC_index(void);
void init_AC_ADC(void);

void init_sample_timer(uint16_t freq);
//-------------------------------------------------------------------------------------------------
//-----------------------------------Functions for Calculations------------------------------------
//-------------------------------------------------------------------------------------------------
void calc_sampled_rms(void);
uint16_t get_sampled_rms(void);
uint16_t get_sampled_DC(void);
void calc_sampled_DC(void);
uint16_t get_max(void);
uint16_t get_max(void);
void calc_max_min(void);


