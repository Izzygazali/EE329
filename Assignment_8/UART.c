/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/7/2018
 */
#include "UART.h"
static uint8_t value_flag = 0;
static uint16_t DAC_level = 0;

/*
 * Function that returns value_flag. value_flag is set to 1 when DAC_level
 * contains a valid value.
 * INPUTS:      NONE
 * RETURN:      value_flag
 */
uint8_t get_value_flag(){
    return value_flag;
}

/*
 * Function that sets value_flag to desired value
 * INPUTS:      NONE
 * RETURN:      value_flag
 */
void set_value_flag(uint8_t num){
    value_flag = num;
    return;
}

/*
 * Function that returns DAC_level. DAC_level contains the value specified by
 * the user to be sent to the DAC.
 * INPUTS:      NONE
 * RETURN:      DAC_level
 */
uint16_t get_DAC_level(){
    return DAC_level;
}


/*
 * Function that sends a string to the console
 * INPUTS:      unsigned char inputString[] = string to write to console
 * RETURN:      NONE
 */
void UART_write_string(unsigned char inputString[]){
    int i = 0;
    while(inputString[i] != 0)
    {
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0->TXBUF = inputString[i];
        i++;
    }
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

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_2;           // Set DCO to 6MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELS_3;              // SMCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses


    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;               //hold in reset state
    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK  |       //set SMCLK as source
                         EUSCI_A_CTLW0_SWRST;               // Remain eUSCI in reset

    EUSCI_A0 -> BRW = 39;                                   //baud rate = 39 => 6MHz/9600/16
    EUSCI_A0 -> MCTLW = (1 << EUSCI_A_MCTLW_BRF_OFS)     |  //First modulation stage select
                        (2 << EUSCI_A_MCTLW_BRS_OFS)     |  //Second modulation stage select
                         EUSCI_A_MCTLW_OS16;                //Oversampling
    EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;              //reset released for operation

    EUSCI_A0 -> IE |= EUSCI_A_IE_RXIE;                      //receive interrupt enable
    EUSCI_A0 -> IFG &= ~EUSCI_A_IFG_RXIFG;                  //clear receive interrupt flag
    NVIC -> ISER[0] = 1 <<(EUSCIA0_IRQn & 31);
}


//EUSCI_A0 ISR
void EUSCIA0_IRQHandler(void)
{
    static uint8_t index = 0;               //index of the number entered
    static uint32_t value = 0;              //temporary variable used to store the entered numbers
    uint8_t input;                          //key pressed by user

    //only enter if interrupt flag is set
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        input = (EUSCI_A0 -> RXBUF);                        //get key pressed by user

        //if a number 0-9 is entered
        if(input >= 48 && input <= 57)
        {
            //only enter if value_flag is not set to 2. value_flag is set to 2 when more than 4 numbers
            //are entered. This ensures that the user can only input 4 digits, since the max DAC level
            //is 4095.
            if(value_flag != 2)
            {
                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));    //echo entered number
                EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;

                //shift previous number(4-bits) to the left and write new number to least significant 4 bits
                value = (value << 4) | (input-48);
                index++;                                        //increment index

                if(index == 4)                                  //when more than 4 numbers are entered
                    value_flag = 2;                             //set value flag to 2
            }
        }
        //if ENTER key is pressed
        else if (input == ENTER_KEY)
        {
           //set the DAC_level to the value entered by the user
           DAC_level = ((value & 0xF000)>> 12)*1000 +           //Digit in thousands place
                       ((value & 0x0F00)>> 8)*100   +           //Digit in hundreds place
                       ((value & 0x00F0)>> 4)*10    +           //Digit in tens place
                       ((value) & 0x000F);                      //Digit in ones place

           value_flag = 1;                                      //set value_flag to 1, indicated valid DAC_level
           index = 0;                                           //reset index to 0
           value = 0;                                           //reset value to 0
        }
    }
}

/*
else if(value_flag != 2)
{
    value_flag = 0;
}
*/
