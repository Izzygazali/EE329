#include "msp.h"
#include "UART.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    init_GPS();
    while(1);
}

