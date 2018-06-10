/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/25/2018
 */
#include "DAC.h"

/*
 * Function to initializes EUSCI_B0 to use SPI with the correct mode and
 * settings to be used with the DAC
 * INPUTS:      NONE
 * RETURN:      NONE
 */
void SPI_init(){
    P1 -> SEL0 |= (UCB0CLK | SIMO | SOMI);              //select EUSCI_B0,configure ports
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST;           //hold logic in reset state
    EUSCI_B0 -> CTLW0 |=(EUSCI_B_CTLW0_MST           |  //master mode select
                         EUSCI_B_CTLW0_MSB           |  //MSB first
                         EUSCI_B_CTLW0_SYNC          |  //synchronous mode
                         EUSCI_B_CTLW0_SSEL__SMCLK   |  //eUSCI clock source, SMCLK
                         EUSCI_B_CTLW0_CKPL);           //clock inactive high

    EUSCI_B0 -> BRW = 0x01;                             //prescaler, divide by 1
    EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST;          //turn on state machine
    return;
}

/*
 * Function to initializes P4.0 as an output for the DACs CS input
 * INPUTS:      NONE
 * RETURN:      NONE
 */
void DAC_init(){
    //GPIO configuration
    P4 -> SEL0 &= ~BIT0;
    P4 -> SEL1 &= ~BIT0;
    P4 -> DIR  |= BIT0;
    //CS initially set low
    P4 -> OUT  &= ~CS_DAC;
    return;
}

/*
 * Function that takes in the 12 bit data word for DAC. SPI is used to send
 * the data to the DAC
 * INPUTS:      uint16_t data = DAC data word, only lower 12 bits matter
 * RETURN:      NONE
 */
void DAC_data(uint16_t data){
    uint8_t hiByte, loByte;

    loByte = data & 0xFF;                           //bottom 8 bits of data
    hiByte = (data >> 8) & 0x0F;                    //upper 4 bits of data
    hiByte |= (GAIN | SHDN);

    P4 -> OUT  &= ~CS_DAC;                          //CS input on DAC set low
    while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));  //wait to receive upper 4 bits
    EUSCI_B0 -> TXBUF = hiByte;
    while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));  //wait to receive lower 8 bits
    EUSCI_B0 -> TXBUF = loByte;
    while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_RXIFG));  //Data received
    P4 -> OUT |= CS_DAC;                            //set CS high
    return;
}

/*
 * Function that initializes Timer_A0 to be used with SMCLK at 3MHz.
 * CCR0 and CCR1 values set in header file
 * INPUTS:      NONE
 * RETURN:      NONE
 */
void Timer_init(){
    //Set DCO clock frequency and select as source for SMCLK
        CS -> KEY = CS_KEY_VAL;              //enable writing to clock systems
        CS -> CTL0 = 0;                      //clear control register 0
        CS -> CTL0 |= CS_CTL0_DCORSEL_1;     //DCO frequency set to 3 MHz
        CS -> CTL1 |= CS_CTL1_SELS__DCOCLK;  //Set DCO as the source for SMCLK
        CS -> KEY = 0;                       //disable writing to clock systems

        // Enable CCR0 and CCR1 interrupts and clear flags.
        TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
        TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;

        //Set CCR0 and CCR1 values.
        TIMER_A0->CCR[0] = CCR0_COUNT;
        TIMER_A0->CCR[1] = CCR1_COUNT;

        //Select SMCLK as the source for TIMERA0 and use UP mode
        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;

        // Enable interrupts
        __enable_irq();
        NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
        NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
}

