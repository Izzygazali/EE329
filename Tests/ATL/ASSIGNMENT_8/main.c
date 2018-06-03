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
    WRITE_DAC(4095);

    while(1){
        value_flag = get_value_flag();
        if (value_flag == 1){
            uart_value = get_uart_value();
            WRITE_DAC(uart_value);
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = 'y';
            reset_value_flag();
        }
    }
}



