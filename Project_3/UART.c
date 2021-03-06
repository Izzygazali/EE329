#include "UART.h"
#include "dmm_functions.h"

/*
 * Function that sends a string to the console
 * INPUTS:      unsigned char inputString[] = string to write to console
 * RETURN:      NONE
 */
void UART_write_string(char inputString[]){
    int i = 0;
    while(inputString[i] != '\0')
    {
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0->TXBUF = inputString[i];
        i++;
    }
    return;
}

/*
 * Function that checks if the values from dmm_functions are likely valid
 * INPUTS:      uint16_t FREQ, uint16_t MAX, uint16_t MIN, uint16_t RMS, uint16_t DC
 * RETURN:      NONE
 */
void check_valid(uint16_t FREQ, uint16_t MAX, uint16_t MIN, uint16_t RMS, uint16_t DC)
{
    //set the cursor to the home position
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_VALID);
    //if any values are outside of valid ranges set the invalid dot red
    //otherwise it is green
    if(FREQ > 1000 && FREQ != 0XFFFF)
        UART_write_string(COLOR_BACKGROUND_RED);
    else if(MAX > 3300 && MAX != 0XFFFF)
        UART_write_string(COLOR_BACKGROUND_RED);
    else if(MIN > 3300 && MIN != 0XFFFF)
        UART_write_string(COLOR_BACKGROUND_RED);
    else if(RMS > 3300 && RMS != 0XFFFF)
        UART_write_string(COLOR_BACKGROUND_RED);
    else if(DC > 3300)
        UART_write_string(COLOR_BACKGROUND_RED);
    else
        UART_write_string(COLOR_BACKGROUND_GREEN);
    UART_write_string(" ");
    UART_write_string(COLOR_BACKGROUND_BLACK);
    return;
}

/*
 * Function that handles UART initialization. UART is initialized to
 * use P1.2 for receiving and P1.3 for transmitting.
 * INPUTS:      NONE
 * RETURN:      NONE
 */
void UART_init(void){
    //GPIO settings: P1.2 RX, P1.3 TX
    P1 -> SEL0 |=  (BIT2 | BIT3);
    P1 -> SEL1 &= ~(BIT2 | BIT3);

    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;               //hold in reset state
    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK       |  //set SMCLK as source
                         EUSCI_A_CTLW0_SWRST;               //Remain eUSCI in reset

    EUSCI_A0 -> BRW = 13;                                   //baud rate = 13 => 24MHz/115200/16
    EUSCI_A0 -> MCTLW = (0    << EUSCI_A_MCTLW_BRF_OFS)   | //First modulation stage select
                        (0x49 << EUSCI_A_MCTLW_BRS_OFS)   | //Second modulation stage select
                         EUSCI_A_MCTLW_OS16;                //Oversampling
    EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;              //reset released for operation

    EUSCI_A0 -> IE |= EUSCI_A_IE_RXIE;                      //receive interrupt enable
    EUSCI_A0 -> IFG &= ~EUSCI_A_IFG_RXIFG;                  //clear receive interrupt flag
    NVIC -> ISER[0] = 1 <<(EUSCIA0_IRQn & 31);              //interrupt vector enable
    return;
}

/*
 * Function that initializes the console for the Digital Mulitimeter
 * INPUTS:      NONE
 * RETURN:      NONE
 */
void initialize_console(void)
{
    UART_write_string(CLEAR_SCREEN);
    UART_write_string(CURSOR_HOME);
    UART_write_string(COLOR_BACKGROUND_BLACK);
    UART_write_string(COLOR_FOREGROUND_WHITE);
    UART_write_string("______________________________________________________________________\n\r");
    UART_write_string("|************************ EE329 Project 3 ****************************|\n\r");
    UART_write_string("|********************** Digital Mulitmeter ***************************|\n\r");
    UART_write_string("|_____________________________________________________________________|\n\r");
    UART_write_string("|Frequency :       Hz                                          Valid  |\n\r");
    UART_write_string("|Vp-p      :       V                                                  |\n\r");
    UART_write_string("|Max. Val  :       V                                                  |\n\r");
    UART_write_string("|Min. Val  :       V                                                  |\n\r");
    UART_write_string("|RMS       : 3.300 V  |-----------------------------------|           |\n\r");
    UART_write_string("|DC        : 3.300 V  |-----------------------------------|           |\n\r");
    UART_write_string("|                     :    :    :    :    :    :    :    :            |\n\r");
    UART_write_string("|                     0.0  0.5  1.0  1.5  2.0  2.5  3.0  3.5          |\n\r");
    UART_write_string("|_____________________________________________________________________|\n\r");
    return;
}

/*
 * Function that converts a binary number to a BCD number
 * INPUTS:      uint16_t binary_number = binary number to be converted
 * RETURN:      uint16_t bcd_number = BCD representation of the input binary number
 */
uint16_t binary_to_bcd(uint16_t binary_number)
{
    //variable initialization.
    uint16_t bcd_number = 0;
    uint8_t digit = 0;

    //algorithm which converts binary to BCD.
    while (binary_number > 0)
    {
        bcd_number |= (binary_number % 10) << (digit++ << 2);
        binary_number /= 10;
    }
    return bcd_number;
}

/*
 * Function that prints a given voltage onto the console. The voltage is printed in the
 * following format: 3.300
 * INPUTS:      uint16_t voltage = voltage to be printed to console, multiplied by 1000
 * RETURN:      NONE
 */
void voltage_to_console(uint16_t voltage)
{
    //variable initialization
    char voltage_string[] = {'0','.','0','0','0','\0'};
    uint16_t bcd_num = 0;
    uint16_t temp = 0;
    uint8_t shift = 12;
    uint8_t i;

    //convert input voltage to a BCD number
    bcd_num = binary_to_bcd(voltage);

    //algorithm for copying the BCD voltage into a character array with 3 decimal places
    for(i = 0; i < 5;i++)
    {
        if(i != 1)
        {
            temp = (bcd_num & (0xF000 >> (12 - shift))) >> shift;
            voltage_string[i] = temp + 48;
            shift -= 4;
        }
    }
    //print voltage on console
    if (voltage == 0xFFFF)
    {
        voltage_string[0] = '-';
        voltage_string[1] = '-';
        voltage_string[2] = '-';
        voltage_string[3] = '-';
        voltage_string[4] = '-';
        voltage_string[5] = '\0';
    }
    UART_write_string(voltage_string);
    return;
}

/*
 * Function that prints a given frequency onto the console.
 * INPUTS:      uint16_t frequency = frequency to be printed to console
 * RETURN:      NONE
 */
void freq_to_console(uint16_t frequency)
{
    //variable initialization
    char frequency_string[] = {'0','0','0','0','\0'};
    uint16_t bcd_num = 0;
    uint16_t temp = 0;
    uint8_t shift = 12;
    uint8_t i;

    //convert input frequency to a BCD number
    bcd_num = binary_to_bcd(frequency);

    //algorithm for copying the BCD frequency to character array
    for(i = 0; i < 4;i++)
    {
        temp = (bcd_num & (0xF000 >> (12 - shift))) >> shift;
        frequency_string[i] = temp + 48;
        shift -= 4;
    }

    //The following code segment replaces all leading zero's with the space character ' '
    //For example: 1000 => 1000, 0100 => 100, 0010 => 10, and 0001 => 1
    if(frequency < 1000 && frequency >= 100)
    {
        frequency_string[0] = ' ';
    }
    else if(frequency < 100 && frequency >= 10)
    {
        frequency_string[0] = ' ';
        frequency_string[1] = ' ';
    }
    else if(frequency < 10)
    {
        frequency_string[0] = ' ';
        frequency_string[1] = ' ';
        frequency_string[2] = ' ';
    }

    if (frequency == 0xFFFF)
    {
        frequency_string[0] = '-';
        frequency_string[1] = '-';
        frequency_string[2] = '-';
        frequency_string[3] = '-';
        frequency_string[4] = '\0';
    }
    //print frequency on console
    UART_write_string(frequency_string);
    return;
}

/*
 * Function that prints bars, represented by a red ']' character, on the display. The
 * number of bars displayed corresponds to the input voltage, where each bar is equivalent
 * of 0.1 V
 * INPUTS:      uint16_t voltage = voltage level that bars will represent, mulitplied by 1000
 * RETURN:      NONE
 */
void set_voltage_bars(uint16_t voltage)
{
    //variable initialization
    uint8_t num = voltage/100;
    uint8_t bar = 0;
    uint8_t dash = 0;

        //character color set to RED
    if (voltage == 0xFFFF)
    {
        num = 0;
    }

    if(num < 35)
    {
        UART_write_string(COLOR_FOREGROUND_RED);
        while(bar < num)
        {
            UART_write_string("]");                 //print bars
            bar++;
            dash++;
        }
        UART_write_string(COLOR_FOREGROUND_WHITE);
        while(dash < 35)
        {
            UART_write_string("-");
            dash++;
        }
    }
    UART_write_string(COLOR_FOREGROUND_WHITE);      //reset character color to white
    return;
}

/*
 * Function that updates the measurements displayed on the console.
 * INPUTS:      uint16_t FREQ = new frequency to display
 *              uint16_t MAX  = new MAX value to display, multiplied by 1000
 *              uint16_t MIN  = new MIN value to display, multiplied by 1000
 *              uint16_t RMS  = new RMS value to display, multiplied by 1000
 *              uint16_t DC   = new DC value to display, multiplied by 1000
 * RETURN:      NONE
 */
void update_display(uint16_t FREQ, uint16_t MAX, uint16_t MIN, uint16_t RMS, uint16_t DC)
{
    //define tuning factors for different voltage values
    float max_tune_factor = 0.985;
    float min_tune_factor = 0.94;
    float dc_tune_factor = 0.9843;
    //check if the input data is valid
    check_valid(FREQ, MAX, MIN, RMS, DC);
    //In the following code segment, the cursor position is set for each measurement and then
    //the measurement is updated.
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_FREQ);
    freq_to_console(FREQ);
    
    //if DC mode write "----" to Vp-p otherwise print normally
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_VPP);
    if(MAX == 0xFFFF)
        voltage_to_console(0xFFFF);
    else
        voltage_to_console((MAX*max_tune_factor-MIN*min_tune_factor));
    
    //if DC mode write "----" to MAX otherwise print normally
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_MAX);
    if (MAX != 0xFFFF)
        voltage_to_console(MAX*max_tune_factor);
    else
        voltage_to_console(MAX);
    
    //if DC mode write "----" to MIN otherwise print normally
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_MIN);
    if (MIN != 0xFFFF)
        voltage_to_console(MIN*min_tune_factor);
    else
        voltage_to_console(MIN);
    //write updated RMS value to console
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_RMS);
    voltage_to_console(RMS);
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_RMS_BAR);
    set_voltage_bars(RMS);
    //write updated DC value to console
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_DC);
    voltage_to_console(DC*dc_tune_factor);
    UART_write_string(CURSOR_HOME);
    UART_write_string(CURSOR_POSITION_DC_BAR);
    set_voltage_bars(DC*dc_tune_factor);
    return;
}
