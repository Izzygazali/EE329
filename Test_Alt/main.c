#include "msp.h"
#include "Altimeter.h"

int main(void)
{
    int i = 0;
    float temperature[5], altitude[5];
    float temp = 0;
    float alt = 0;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;             // Stop watchdog timer
    __enable_irq();                                         // Enable global interrupt

    Init_I2C(MPL3115A2_ADDRESS);
    Init_MPL3115A2();
    set_sea_pressure(101372);

    for(i = 0; i < 5; i++){
        temperature[i] = get_temperature();
        __delay_cycles(1500000);
    }

    for(i = 0; i < 5; i++){
        temp += temperature[i];
    }
    temp /= 5.0;

    for(i = 0; i < 5; i++){
        altitude[i] = get_altitude();
        __delay_cycles(1500000);
    }

    for(i = 0; i < 5; i++){
        alt += altitude[i];
    }
    alt /= 5.0;

    //float altitude = get_altitude();

    while(1);
}
