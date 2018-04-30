/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/24/2018 */
#include "DAC.h"

/* This function initializes inputs and registers for
 * the SPI interface used with the DAC.
 */
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

/* This function sends the amplitude over SPI
 * to the DAC in two 8-bit bytes. It accepts
 * a 12-bit amplitude in "amplitude"
 */
void WRITE_DAC(uint16_t amplitude)
{
    uint8_t lobyte, hibyte;
    //Determine values of the lobyte
    lobyte = 0xFF & amplitude;
    //Determine values of the hibyte
    hibyte = (amplitude >> 8) & 0x0F;
    //Set control bits to enable DAC
    hibyte |= GAIN + SHDN;
    //Set Chip select low to begin write
    P4 ->OUT  &= ~CHIPSEL;
    //make sure SPI interface is ready to send
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG));
    //send hibyte over SPI to the DAC
    EUSCI_B0 ->TXBUF = hibyte;
    //make sure SPI interface is ready to send
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG));
    //send lobyte over SPI to the DAC
    EUSCI_B0 ->TXBUF = lobyte;
    //make sure SPI interface has finished sending lobyte
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_RXIFG));
    //Set Chip select high to end write
    P4 ->OUT |= CHIPSEL;
    return;
}

/* This function initializes the timer which sets
 * our "sample-rate" for the triangle wave.
 */
void INIT_TIMER()
{
       //Enable writing to clock registers
       CS ->KEY = CS_KEY_VAL;
       //Set Timer CLK to DCOCLK
       CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;
       //Disable writing to clock registers
       CS ->KEY = 0;
       //Initialize Timer A in UP mode with CCR0 which sets the
       //"sample rate" of the triangle wave.
       TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
       TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
       TIMER_A0->CCR[0] = CCR0_COUNT;
       TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;
       //Enable interrupts on Timer A for CCR0
       NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
       return;
}

