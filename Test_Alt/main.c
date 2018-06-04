#include "msp.h"
#include "Altimeter.h"

int main(void)
{
    int i = 0;
    float temp = 0;
    float alt = 0;

    //Set DCO clock frequency and select as source for SMCLK
        CS -> KEY = CS_KEY_VAL;              //enable writing to clock systems
        CS -> CTL0 = 0;                      //clear control register 0
        CS -> CTL0 |= CS_CTL0_DCORSEL_3;     //DCO frequency set to 3 MHz
        CS -> CTL1 |= CS_CTL1_SELS__DCOCLK;  //Set DCO as the source for SMCLK
        CS -> KEY = 0;                       //disable writing to clock systems


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;             // Stop watchdog timer
    __enable_irq();                                         // Enable global interrupt

    Init_I2C(MPL3115A2_ADDRESS);
    Init_MPL3115A2();
    set_sea_pressure(101372);

    temp = get_temperature();

    alt = get_altitude();

    while(1);
}
