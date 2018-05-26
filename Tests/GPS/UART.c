#include "UART.h"

#define reset_flag BIT0
#define cap_payload_flag BIT1


//bit 0 - hold in reset
//bit 1 - captured payload
volatile uint16_t gps_flags = 0;

volatile uint16_t gps_payload[100];
uint16_t gps_payload_index = 0;
volatile uint16_t payload_size = 0;
volatile uint8_t  curr_gps_byte = 0;
uint8_t class = 0;
uint8_t id = 0;

enum event_type{
    reset,
    captured_payload
};

enum event_type event;


enum state_type{
    get_sync_1,
    get_sync_2,
    get_class,
    get_ID,
    get_length_1,
    get_length_2,
    get_payload
};



uint16_t get_gps_flags(void)
{
    return gps_flags;
}

void reset_gps_flags(void)
{
    gps_flags = 0;
}


void gps_state_decode(void)
{
    switch(gps_flags)
    {
        case reset_flag:
            event = reset;
            break;
        case cap_payload_flag:
            event = captured_payload;
            break;
    }
    return;
}

void gps_FSM(void)
{
    static enum state_type state = get_sync_1;
    static uint8_t class = 0;
    static uint8_t id = 0;

    switch(state)
    {
        case get_sync_1:
            if (curr_gps_byte == 0xB5){
                state = get_sync_2;
            }else{
                state = get_sync_1;
            }
            break;
        case get_sync_2:
           if (curr_gps_byte == 0x62){
               state = get_class;
           }else{
               state = get_sync_1;
           }
           break;
        case get_class:
           class = curr_gps_byte;
           state = get_ID;
           break;
        case get_ID:
           id = curr_gps_byte;
           state = get_length_1;
           break;
        case get_length_1:
            payload_size = 0;
            payload_size |= curr_gps_byte;
            state = get_length_2;
            break;
        case get_length_2:
            payload_size |= (curr_gps_byte << 8);
            state = get_payload;

            gps_payload_index = 0;
            break;
        case get_payload:
            if (gps_flags & cap_payload_flag){
                state = get_sync_1;
                if (id == 0x02){
                  state = get_sync_1;
              }
                gps_flags &= ~cap_payload_flag;
            }
            gps_payload[gps_payload_index] = curr_gps_byte;
            gps_payload_index++;
            payload_size--;
            if (payload_size == 0)
                gps_flags |= cap_payload_flag;
                //gps_payload[gps_payload_index+1] = 0xFFFF;
            break;
        default:
            state = get_sync_1;
            break;

    }
    return;
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
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        curr_gps_byte = EUSCI_A2->RXBUF;
        gps_state_decode();
        gps_FSM();
    }
}
