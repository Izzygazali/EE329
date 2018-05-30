#include "msp.h"
#include "UART.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    init_GPS();
    reset_gps_odometer();
    while(1);
}

