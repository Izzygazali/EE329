#include "DAC.h"

void SPI_INIT(void)
{
    //Enable SPI function on desired GPIO
    P1 ->SEL0 |= SCLK + SIMO + SOMI;
    P1 ->SEL1 &= ~(SCLK + SIMO + SOMI);
    P4 ->DIR  |= CHIPSEL;
    //Put State-machine into reset state
    EUSCI_B0 ->CTLW0 |= EUSCI_B_CTLW0_SWRST;
    //set SPI operation parameters
    EUSCI_B0 ->CTLW0 |= EUSCI_B_CTLW0_SWRST       |
                        EUSCI_B_CTLW0_MSB         |
                        EUSCI_B_CTLW0_MST         |
                        EUSCI_B_CTLW0_SYNC        |
                        EUSCI_B_CTLW0_SSEL__SMCLK |
                        EUSCI_B_CTLW0_CKPL;
    //Set pre-scalar to 1
    EUSCI_B0 ->BRW = 0x01;
    //enable state machine
    EUSCI_B0 ->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
    return;
}

void WRITE_DAC(uint16_t data)
{
    uint8_t lobyte, hibyte;
    lobyte = 0xFF & data;
    hibyte = (data >> 8) & 0x0F;
    hibyte |= GAIN + SHDN;
    P4 ->OUT  &= ~CHIPSEL;
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0 ->TXBUF = hibyte;
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0 ->TXBUF = lobyte;
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_RXIFG));
    P4 ->OUT |= CHIPSEL;
    return;
}

void INIT_TIMER()
{
       CS ->KEY = CS_KEY_VAL;
       CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;
       CS ->KEY = 0;
       TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
       TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
       TIMER_A0->CCR[0] = CCR0_COUNT;
       TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;
       NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
       return;
}

