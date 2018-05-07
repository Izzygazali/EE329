#include "msp.h"
#include "UART.h"
#include "DAC.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    init_UART();
    SPI_INIT();
    uint8_t value_flag = 0;
    uint16_t uart_value = 0;
    while(1){
        value_flag = get_value_flag();
        uart_value = get_uart_value();
        if (value_flag == 1){
            WRITE_DAC(uart_value);
            reset_value_flag();
        }
    }
}



