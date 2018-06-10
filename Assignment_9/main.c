/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/9/2018
 * Description: main file implementing the DC voltage measurement
 *              and UART output of the voltage.
 */
#include "UART.h"
#include "ADC.h"

/*
 * Function that converts a binary number into its BCD form.
 * The number must fit in 4 base ten digits.
 * INPUTS:      uint16_t binary_number = binary number to convert
 * RETURN:      bcd_number = BDC representation of the input.
 */
uint16_t binary_to_bcd(uint16_t binary_number)
{
   //initialize variables for use in conversion.
   uint16_t bcd_number = 0;
   uint8_t  digit = 0;
   //algorithm which converts binary to bcd.
   while (binary_number > 0) {
       bcd_number |= (binary_number % 10) << (digit++ << 2);
       binary_number /= 10;
   }
   //return bcd number
   return bcd_number;
}



void send_voltage_UART(uint16_t voltage_number)
{
    //Clear and set the cursor position in the UART terminal
    UART_write_string(CLEAR_LINE);
    UART_write_string(CURSOR_HOME);

    //initialize character array to store the output string in
    char voltage_string[] = {'0','.','0','0','0','V','\0'};
    //fill in the "digits" of the string from the BCD number determined.
    voltage_string[0] = ((voltage_number & 0xF000) >> 12) + 48;
    voltage_string[2] = ((voltage_number & 0x0F00) >> 8) + 48;
    voltage_string[3] = ((voltage_number & 0x00F0) >> 4) + 48;
    voltage_string[4] = (voltage_number & 0x000F) + 48;
    //write the character array to UART
    UART_write_string(voltage_string);
    return;
}

void main(void){
    //stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    //Initialize the UART communication and the ADC.
    UART_init();
    init_ADC();
    //declare and initialize useful variables for converting
    //ADC values to display via UART.
    uint16_t converted_voltage = 0;
    uint16_t converted_bcd_voltage = 0;

    //infinite loop to continue checking voltage.
    while(1)
    {
        //start conversion to get ADC value
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        //delay a bit to allow ADC value to be determined
       __delay_cycles(20000);
       //if a the adc_flag is set (Bit 15 of adc_val_and_flag from ADC.c)
       if (get_adc_val_and_flag() & adc_flag)
       {
           //multiply the adc value by a conversion factor to get voltage
           converted_voltage = (get_adc_val_and_flag() & ~adc_flag)*adc_conv_factor;
           //if the converted voltage is in a valid range display it
           if (converted_voltage <= 3300)
           {
               //convert adc value in to a BCD representation
               converted_bcd_voltage = binary_to_bcd(converted_voltage);
               //convert and send the BCD representation over UART
               send_voltage_UART(converted_bcd_voltage);
           }
           //reset the ADC flag
           reset_adc_flag();
       }
    }
}
