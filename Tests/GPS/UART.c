#include "UART.h"

#define gps_reset_flag BIT0
#define gps_sync_flag BIT1
#define gps_class_id_recv_flag BIT2
#define gps_valid_class_id_flag BIT3
#define gps_invalid_class_id_flag BIT4
#define gps_payload_size_flag BIT5
#define gps_payload_cap_flag BIT6

//bit 0 - hold in reset
//bit 1 - sync found
//bit 2 - class/id found
//bit 3 - class/id valid
//bit 4 - class/id invalid
//bit 5 - payload size flag
//bit 6 - payload capture flag
volatile uint16_t gps_flags = 0;
volatile uint16_t gps_payload[100];
volatile uint32_t payload_size = 0;
volatile uint32_t id_class = 0;


enum event_type{
    sync_recieved,
    class_id_recieved,
    class_id_valid,
    class_id_invalid,
    set_payload_length,
    payload_captured,
    reset
};

enum state_type{
    reset_state,
    wait_for_sync,
    get_class_id,
    check_class_id,
    get_payload_length,
    capture_payload
};


enum event_type event;


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
        case gps_sync_flag:
            event = sync_recieved;
            break;
        case gps_class_id_recv_flag:
            event = class_id_recieved;
            break;
        case gps_valid_class_id_flag:
            event = class_id_valid;
            break;
        case gps_invalid_class_id_flag:
            event = class_id_invalid;
            break;
        case gps_payload_size_flag:
            event = set_payload_length;
            break;
        case gps_payload_cap_flag:
            event = payload_captured;
        case gps_reset_flag:
            event = reset;
        default:
            event = payload_captured;
            break;
    }
    return;
}

void gps_FSM(void)
{
    static enum state_type state = wait_for_sync;
    switch(state)
    {
        case wait_for_sync:
            if (event == sync_recieved){
                state = get_class_id;
                gps_flags &= ~gps_sync_flag;
                id_class = 0;
            }
            break;
        case get_class_id:
            if (event == class_id_recieved){
                state = check_class_id;
                gps_flags &= ~gps_sync_flag;

            }
            break;
        case check_class_id:
            if (event == class_id_valid){
                state = get_payload_length;
                gps_flags &= ~gps_valid_class_id_flag;
            }else if(event == class_id_invalid){
                state = wait_for_sync;
                gps_flags &= ~gps_invalid_class_id_flag;
            }
            if (id_class == 0x0102)
                gps_flags |= gps_valid_class_id_flag;
            else
                gps_flags |= gps_invalid_class_id_flag;
            break;
        case get_payload_length:
            if (event == set_payload_length){
                state = capture_payload;
                gps_flags &= ~gps_payload_size_flag;
            }
            break;
        case capture_payload:
            if (event == set_payload_length){
                state = wait_for_sync;
                gps_flags &= ~gps_payload_cap_flag;
            }
        case reset_state:
            break;
    }
    if (event == reset)
        state = reset_state;
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
    static uint16_t prev_gps_char = 0;
    uint16_t int_gps_char;
    static uint8_t int_count = 0;

    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        int_gps_char = EUSCI_A2->RXBUF;
        if (int_gps_char == 0x62 && prev_gps_char == 0xB5)
            gps_flags |= gps_sync_flag;
        if(event == sync_recieved){
            id_class |= int_gps_char << (int_count << 3);
            int_count++;
            if (int_count == 2){
                int_count = 0;
                gps_flags |= gps_class_id_recv_flag;
            }
        }
        if (event == class_id_valid){
            payload_size |= int_gps_char << (int_count << 3);
            int_count++;
            if (int_count == 2){
                int_count = 0;
                gps_flags |= gps_payload_size_flag;
            }
        }
        if (event == set_payload_length){
            gps_payload[int_count] = int_gps_char;
            int_count++;
            if (int_count == payload_size){
                int_count = 0;
                gps_flags |= gps_payload_cap_flag;
            }

        }
    }
}
