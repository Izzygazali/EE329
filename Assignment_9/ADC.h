#include "msp.h"

#define CONVERSION_FACTOR 0.201416015625

void ADC_init(void);
uint16_t get_sample(void);
uint8_t get_sample_flag(void);
void set_sample_flag(uint8_t input_val);
uint8_t sample_to_voltage(void);
void ADC14_IRQHandler(void);
