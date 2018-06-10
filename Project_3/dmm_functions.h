#include "msp.h"
#include "DAC.h"
#include <stdlib.h>
#include <math.h>

//Define Values for Inputs for DMM
#define PEAK_RESET BIT0
#define VALLEY_RESET BIT1
#define PEAK BIT2
#define VALLEY BIT5
#define INPUT_FREQ BIT5
#define INPUT_WAVE BIT4

//Bits of the DMM flag variable defined
#define dc_offset_flag BIT0
#define wave_freq_flag BIT1
#define sampling_done_flag BIT2
#define calc_done_flag BIT3
#define results_displayed_flag BIT4
#define dc_flag_set BIT5

//define conversion factor between ADC values and volts for display
#define volt_conv_factor 0.201416
//-------------------------------------------------------------------------------------------------
//--------------------------------Functions for All Parts------------------------------------------
//-------------------------------------------------------------------------------------------------
uint16_t get_dmm_flags(void);
void reset_dmm_flags(uint16_t flags);
void set_dmm_flags(uint16_t flags);
void init_clock(void);
void set_freq_fast(void);
void set_freq_slow(void);
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
void set_freq_conversion(uint32_t input_conv_factor);
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
uint16_t get_min(void);
void calc_max_min(void);


