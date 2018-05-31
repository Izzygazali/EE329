#include "msp.h"
#include "UART.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    init_GPS();
    while((get_gps_flags() & gps_ack_flag) == 0)
        reset_gps_odometer();
    while(1){
        uint32_t lat = get_curr_lat();
        uint32_t lon = get_curr_lon();
        uint32_t tow = get_curr_tow();
        uint32_t dist = get_curr_dist();
    }
}

