#include "msp.h"
#include "UART.h"

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	UART_init();
	UART_write_string("Hello World");
	__delay_cycles(100000);
	UART_write_string(CLEAR_LINE);
	UART_write_string(CURSOR_HOME);
    __delay_cycles(100000);
    UART_write_string("hello");
    __delay_cycles(100000);
    UART_write_string(CLEAR_LINE);
    UART_write_string(CURSOR_HOME);
    UART_write_string("Hi");
    __delay_cycles(100000);
    UART_write_string(CLEAR_LINE);
    UART_write_string(CURSOR_HOME);
}
