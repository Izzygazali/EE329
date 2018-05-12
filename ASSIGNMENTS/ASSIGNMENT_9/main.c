#include "msp.h"
#include "UART.h"
#include "ADC.h"

uint16_t binary_to_bcd(uint16_t binary_number)
{
   uint16_t bcd_number = 0;
   uint8_t  digit = 0;
   while (binary_number > 0) {
       bcd_number |= (binary_number % 10) << (digit++ << 2);
       binary_number /= 10;
   }
   return bcd_number;
}



void send_voltage_UART(uint16_t voltage_number)
{
    UART_write_string(CLEAR_LINE);
    UART_write_string(CURSOR_HOME);

    char voltage_string[] = {'0','.','0','0','0','V','\0'};

    voltage_string[0] = ((voltage_number & 0xF000) >> 12) + 48;
    voltage_string[2] = ((voltage_number & 0x0F00) >> 8) + 48;
    voltage_string[3] = ((voltage_number & 0x00F0) >> 4) + 48;
    voltage_string[4] = (voltage_number & 0x000F) + 48;
    UART_write_string(voltage_string);


    return;
}


void main(void){

    WDTCTL = WDTPW | WDTHOLD;
    UART_init();
    init_ADC();


    uint16_t converted_voltage = 0;
    uint16_t converted_bcd_voltage = 0;

    while(1)
    {
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
       __delay_cycles(20000);
       if (get_adc_val_and_flag() & adc_flag)
       {
           converted_voltage = (get_adc_val_and_flag() & ~adc_flag)*adc_conv_factor;
           if (converted_voltage <= 3300)
           {
               converted_bcd_voltage = binary_to_bcd(converted_voltage);
               send_voltage_UART(converted_bcd_voltage);
           }
           reset_adc_flag();
       }
    }
}



