/*
 * Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 04/24/2018
 */
#include "function_generator.h"

/*
 * Function to initializes EUSCI_B0 to use SPI with the correct mode and
 * settings to be used with the DAC
 * INPUTS:      NONE
 * RETURN:      NONE
 */
void SPI_INIT(void)
{
    //select EUSCI_B0,configure ports
    P1 ->SEL0 |= SCLK + SIMO + SOMI;
    P1 ->SEL1 &= ~(SCLK + SIMO + SOMI);
    P4 ->DIR  |= CHIPSEL;

    EUSCI_B0 ->CTLW0 |= EUSCI_B_CTLW0_SWRST;        //Put State-machine into reset state
    EUSCI_B0 ->CTLW0 |= EUSCI_B_CTLW0_SWRST       | //hold logic in reset state
                        EUSCI_B_CTLW0_MSB         | //MSB first
                        EUSCI_B_CTLW0_MST         | //master mode select
                        EUSCI_B_CTLW0_SYNC        | //synchronous mode
                        EUSCI_B_CTLW0_SSEL__SMCLK | //eUSCI clock source, SMCLK
                        EUSCI_B_CTLW0_CKPL;         //clock inactive high

    EUSCI_B0 ->BRW = 0x01;                          //Set pre-scalar to 1
    EUSCI_B0 ->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;       //enable state machine
    return;
}

/*
 * Function that takes in the 12 bit data word for DAC. SPI is used to send
 * the data to the DAC
 * INPUTS:      uint16_t data = DAC data word, only lower 12 bits matter
 * RETURN:      NONE
 */
void WRITE_DAC(uint16_t amplitude)
{
    uint8_t lobyte, hibyte;

    lobyte = 0xFF & amplitude;          //Determine values of the lobyte
    hibyte = (amplitude >> 8) & 0x0F;   //Determine values of the hibyte
    hibyte |= GAIN + SHDN;              //Set control bits to enable DAC

    P4 ->OUT  &= ~CHIPSEL;              //Set Chip select low to begin write

    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG)); //wait until SPI is ready to send
    EUSCI_B0 ->TXBUF = hibyte;                     //send hibyte over SPI to the DAC
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_TXIFG)); //wait until SPI is ready to send
    EUSCI_B0 ->TXBUF = lobyte;                     //send lobyte over SPI to the DAC
    while (!(EUSCI_B0 ->IFG & EUSCI_B_IFG_RXIFG)); //wait until SPI data is recieved
    P4 ->OUT |= CHIPSEL;                           //Set Chip select high to end write
    return;
}

/*
 * Function that initializes Timer_A0 to be used with SMCLK at 6MHz.
 * CCR0 and CCR1 values set in header file
 * INPUTS:      NONE
 * RETURN:      NONE
 */
void INIT_TIMER()
{
       CS ->KEY = CS_KEY_VAL;               //Enable writing to clock registers
       CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;   //Set Timer CLK to DCOCLK
       CS ->KEY = 0;                        //Disable writing to clock registers

       //Initialize Timer A in UP mode with CCR0 which sets the "sample rate"
       //for the generated waves.
       TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
       TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
       TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
       TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
       TIMER_A0->CCR[1] = CCR1_COUNT;
       TIMER_A0->CCR[0] = CCR0_COUNT;
       TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_ID__4;

       //Enable interrupts on Timer A for CCR0
       NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
       NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
       return;
}

