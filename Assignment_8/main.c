/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/7/2018
 * Description: Program that takes in a number from the user and outputs
 *              that value to the DAC
 */
#include "UART.h"
#include "DAC.h"

void main(void){
    //disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    //Initialize SPI, DAC, and UART
    SPI_init();
    DAC_init();
    UART_init();

    //enable interrupts globally
    __enable_irq();

    //prompt for input
    UART_write_string("Enter DAC Level:\n\r");
    while(1)
    {
        //value_flag = 1 indicates a valid DAC_level
        if(get_value_flag() ==1)
        {
            NVIC -> ICER[0] = 1 <<(EUSCIA0_IRQn & 31);      //disable interrupts on UART
            DAC_data(get_DAC_level());                      //write level to DAC
            set_value_flag(0);                              //reset value_flag
            NVIC -> ISER[0] = 1 <<(EUSCIA0_IRQn & 31);      //enable interrupts on UART

            //prompt for input
            UART_write_string("\n\rEnter DAC Level:\n\r");

        }
    }
}


