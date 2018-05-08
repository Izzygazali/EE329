#include "UART.h"
#include "DAC.h"

void main(void){
    //disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    SPI_init();
    DAC_init();
    UART_init();
    __enable_irq();

    UART_cursor_home();
    UART_clear_console();
    UART_write_string("Enter DAC Level:\n\r");
    while(1)
    {
        if(get_value_flag() ==1){
            NVIC -> ICER[0] = 1 <<(EUSCIA0_IRQn & 31);
            DAC_data(get_DAC_level());
            NVIC -> ISER[0] = 1 <<(EUSCIA0_IRQn & 31);
        }
    }
}


