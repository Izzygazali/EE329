#include "UART.h"

#define reset_flag BIT0
#define cap_payload_flag BIT1
#define cap_parsed_flag BIT2

//bit 0 - hold in reset
//bit 1 - captured payload
//bit 2 - payload parsed
//helper variables for getting gps data
volatile uint16_t gps_flags = 0;
volatile uint16_t gps_payload[100];
volatile int16_t payload_size = 0;
uint16_t gps_payload_index = 0;
volatile uint8_t  curr_gps_byte = 0;

//variables used for parsing
uint8_t class = 0;
uint8_t id = 0;
uint32_t curr_lat = 0;
uint32_t curr_lon = 0;
uint32_t curr_tow = 0;
uint32_t curr_dist = 0;


uint32_t classid;
enum event_type{
    reset
};

enum event_type event;


enum state_type{
    get_sync_1,
    get_sync_2,
    get_class,
    get_ID,
    get_length_1,
    get_length_2,
    get_payload,
    parse_payload
};


uint16_t get_gps_flags(void)
{
    return gps_flags;
}

void reset_gps_flags(void)
{
    gps_flags = 0;
}


void reset_gps_odometer(void)
{
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A2->TXBUF = 0xFF;
    __delay_cycles(12000000);
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A2->TXBUF = 0xB5;
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A2->TXBUF = 0x62;
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A2->TXBUF = 0x01;
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A2->TXBUF = 0x10;
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A2->TXBUF = 0x00;
    return;
}


void gps_parse_logic(void)
{
    switch((class << 8) | id)
    {
        case 0x0102:
            curr_lon = (gps_payload[7] << 24)| (gps_payload[6] << 16) | (gps_payload[5] << 8) | gps_payload[4];
            curr_lat = (gps_payload[11] << 24)| (gps_payload[10] << 16) | (gps_payload[9] << 8) | gps_payload[8];
            curr_tow = (gps_payload[3] << 24)| (gps_payload[2] << 16) | (gps_payload[1] << 8) | gps_payload[0];
            break;
        case 0x0109:
            curr_dist = (gps_payload[11] << 24)| (gps_payload[10] << 16) | (gps_payload[9] << 8) | gps_payload[8];
            break;
        case 0x0501:
            event = reset;
        case 0x0500:
             event = reset;
        default:
            break;
    }
    return;
}

void gps_FSM(void)
{
    static enum state_type state = get_sync_1;


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
            gps_payload[gps_payload_index] = curr_gps_byte;
            gps_payload_index++;
            payload_size--;
            if (payload_size <= 0)
                state = parse_payload;
            break;
        case parse_payload:
            gps_parse_logic();
            state = get_sync_1;
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
    __enable_irq();
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
    return;
}






void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        curr_gps_byte = EUSCI_A2->RXBUF;
        gps_FSM();
    }
}
