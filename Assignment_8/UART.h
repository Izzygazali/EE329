#include "msp.h"

#define ENTER_KEY 13

uint8_t get_value_flag(void);
uint16_t get_DAC_level(void);
void UART_init(void);
void EUSCIA0_IRQHandler(void);
