#include "UART.h"

static uint8_t value_flag = 0;
static uint16_t uart_value = 0;

uint8_t get_value_flag(void)
{
    return value_flag;
}

uint16_t get_uart_value(void)
{
    return uart_value;
}

void reset_value_flag(void)
{
    value_flag = 0;
}

void init_UART(void)
{
    //set SMCLK and MCLK to 12MHz
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS->CTL0 = CS_CTL0_DCORSEL_3;
    CS->CTL1 = CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;

    //initialize ports for UART
    P1->SEL0 |= (RX+TX);
    P1->SEL1 &= ~(RX+TX);
    //set reset to hold UART FSM in reset state
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;
    //set clock source to SMCLK
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK;
    //set registers to produce 9600 baud UART with SMCLK at 12MHz
    EUSCI_A0->BRW = 78;
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
    //start FSM by setting FSM reset to zero
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
    //enable UART receive interrupts and clear the flag
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;
    //enable interrupts for UART A0 on NVIC and globally
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    __enable_irq();
    return;
}

void EUSCIA0_IRQHandler(void)
{
    //variable to store individual digits of user input
    static uint16_t con_digits = 0;
    //variable to count number of digits entered
    static uint8_t digit_index = 0;

    //check that UART recieve flag was set
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        //check if the "enter" key was pressed
        if ((EUSCI_A0->RXBUF) == 13)
        {
            //calculate value from individual digits of entered number
            uart_value = (con_digits & 0x000F)
                         +((con_digits & 0x00F0) >> 4)*10
                         +((con_digits & 0x0F00) >> 8)*100
                         +((con_digits & 0xF000) >> 12)*1000;
            //check if value is in valid range for DAC
            if (uart_value > 4095){
                //write "error" flag
                value_flag = 2;
                //clear digits of entered number
                con_digits = 0;
            }else{
                //write valid number flag
                value_flag = 1;
                //clear digits of entered number
                con_digits = 0;
            }
            //reset digit number on user input
            digit_index = 0;
        }
        else
        {
            //shift 4 bit to store next user input digit
            con_digits = con_digits << 4;
            //store individual digit in con_digits
            con_digits |= (EUSCI_A0->RXBUF-48);
            //increment to indicate number of digits input
            digit_index++;
            //Echo digit to provide user feedback
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
        }
    }
}
