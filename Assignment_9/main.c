#include "msp.h"
#include "UART.h"
#include "ADC.h"


uint32_t binary_to_BCD(int16_t input_num){
    uint32_t BCD_num = 0;
    uint8_t shift = 0;

    while(input_num > 0){
        BCD_num |= (input_num % 10) << (shift++ << 2);
        input_num /= 10;
    }
    return BCD_num;
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	UART_init();
	/*
	ADC_init();
	int32_t level;
	int i,j;
	uint8_t shift = 0;
	uint32_t temp;
	*/
	char outputString[] = {'H','E','L','L','O'} ;

	while(1){
    UART_write_string(CLEAR_LINE);
    UART_write_string(CURSOR_HOME);
	UART_write_string(outputString);
	UART_write_string(CLEAR_LINE);
	UART_write_string(CURSOR_HOME);
	}

/*
	while(1){
	    ADC14 -> CTL0 |= ADC14_CTL0_SC;
	    //__delay_cycles(10000);
	    if(get_sample_flag())
	    {
	        //NVIC -> ICER[0] = (1 << ADC14_IRQn);
	        level = get_sample();


	        level = binary_to_BCD(sample_to_voltage());

	        for(i = 0; i < 5; i++)
	        {
                temp = (level & (0xF0000>>shift))>>(12 - shift);
                shift += 4;

	            for(j = 0; j < 10; j++)
	            {
	                if(temp == j)
	                {
	                    outputString[i] = j + 48;
	                }
	            }
	        }


	    }
	}
*/
}
