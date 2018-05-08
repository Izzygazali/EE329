/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/7/2018
 * Description: Library file containing useful functions for setting DAC level
 *              through UART communication
 */
#include "msp.h"

#define ENTER_KEY 13

//function prototypes.
uint8_t get_value_flag(void);
uint16_t get_DAC_level(void);
void set_value_flag(uint8_t num);
void EUSCIA0_IRQHandler(void);
void UART_init(void);
void UART_write_string(unsigned char inputString[]);


