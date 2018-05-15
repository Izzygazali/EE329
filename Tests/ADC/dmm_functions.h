#include "msp.h"
#include "DAC.h"
#include <stdlib.h>

#define PEAK BIT0
#define VALLEY BIT1
#define INPUT_FREQ BIT5
#define FREQ_FLAG 0x01
#define AIN BIT4
void init_freq_timer(void);
void init_DC_ADC(void);
void init_AC_ADC(void);
void init_sample_timer(uint16_t CCR0);

void set_DC_offset(void);
float get_high_voltage(void);
float get_low_voltage(void);
uint16_t get_captured_freq(void);
uint8_t get_freq_flag(void);
void reset_freq_flag(void);
float get_sampled_rms(void);












