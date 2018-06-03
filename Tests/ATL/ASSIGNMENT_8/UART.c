#include "UART.h"

static uint8_t gps_flag = 0;
volatile uint16_t stored_GPS_transmission[];

uint16_t get_gps_flag(void)
{
    return gps_flag;
}

void reset_gps_flag(void)
{
    gps_flag = 0;
}

void init_GPS(void)
{
    //set SMCLK and MCLK to 12MHz
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS->CTL0 = CS_CTL0_DCORSEL_3;
    CS->CTL1 = CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;

    //initialize ports for UART
    P3->SEL0 |= (RX+TX);
    P3->SEL1 &= ~(RX+TX);
    //set reset to hold UART FSM in reset state
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST;
    //set clock source to SMCLK
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST | EUSCI_A_CTLW0_SSEL__SMCLK;
    //set registers to produce 9600 baud UART with SMCLK at 12MHz
    EUSCI_A2->BRW = 78;
    EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
    //start FSM by setting FSM reset to zero
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
    //enable UART receive interrupts and clear the flag
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;
    //enable interrupts for UART A0 on NVIC and globally
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
    __enable_irq();
    return;
}

void EUSCIA2_IRQHandler(void)
{
    static uint16_t class_id_index = 0;
    static uint16_t prev_gps_char = 0;
    uint16_t int_gps_char;
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        int_gps_char = EUSCI_A2->RXBUF;
        if (int_gps_char == 0x62 && prev_gps_char == 0xB5){
            class_id_index = 0;
        }
        stored_GPS_transmission[class_id_index];
        class_id_index++;
        prev_gps_char = int_gps_char;
    }
}
