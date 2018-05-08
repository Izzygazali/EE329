#include "UART.h"

static uint8_t value_flag = 0;
static uint16_t DAC_level = 0;

uint8_t get_value_flag(){
    return value_flag;
}

uint16_t get_DAC_level(){
    return DAC_level;
}

void UART_cursor_home(void){
    UART_write_command("[H");
    return;
}

void UART_clear_console(void){
    UART_write_command("[2J");
    return;
}


void UART_write_command(unsigned char inputString[])
{
    int i = 0;
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = 27;
    while(inputString[i] != 0)
    {
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0->TXBUF = inputString[i];
        i++;
    }
    return;
}

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
    EUSCI_A0 -> BRW = 39;                                   //baud rate = 39
    EUSCI_A0 -> MCTLW = (0X02 << EUSCI_A_MCTLW_BRS_OFS) |
                        (1<< EUSCI_A_MCTLW_BRF_OFS)     |
                         EUSCI_A_MCTLW_OS16;
    EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;              //reset released for operation


    EUSCI_A0 -> IE |= EUSCI_A_IE_RXIE;                      //receive interrupt enable
    EUSCI_A0 -> IFG &= ~EUSCI_A_IFG_RXIFG;
    NVIC -> ISER[0] = 1 <<(EUSCIA0_IRQn & 31);
}

void EUSCIA0_IRQHandler(void)
{
    static uint8_t index = 0;
    static uint32_t value = 0;
    uint8_t input;

    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        input = (EUSCI_A0 -> RXBUF);

        if(input >= 48 && input <= 57)
        {
            if(value_flag != 2)
            {
                if(index == 0){
                    UART_cursor_home();
                    UART_clear_console();
                    UART_write_string("Enter DAC Level:\n\r");
                }
                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;

                value = (value << 4) | (input-48);
                index++;
                if(index == 4){
                    value_flag = 2;
                }
            }
        }
        else if (input == ENTER_KEY){
           DAC_level = ((value & 0xF000)>> 12)*1000 +
                       ((value & 0x0F00)>> 8)*100   +
                       ((value & 0x00F0)>> 4)*10    +
                       ((value) & 0x000F);
           value_flag = 1;
           index = 0;
           value = 0;

           UART_cursor_home();
           UART_clear_console();
           UART_write_string("done");
        }
        else if(value_flag != 2)
        {
            value_flag = 0;
        }
    }
}
