
/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/7/2018
 */
#include "UART.h"

/*
 * Function that sends a string to the console
 * INPUTS:      char inputString[] = string to write to console
 * RETURN:      NONE
 */
void UART_write_string(char inputString[]){
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
    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK      |   //set SMCLK as source
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
