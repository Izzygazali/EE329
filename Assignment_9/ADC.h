#include "msp.h"

#define ANALOG_IN BIT4
#define adc_flag 0x8000
#define adc_conv_factor 0.201416

void init_ADC(void);
void reset_adc_flag(void);
uint16_t get_adc_val_and_flag(void);
