#include "gps.h"

#include "string_conv.h"


//flags for indicating gps operation state
//bit 0 - data valid
//bit 1 - new data received
volatile uint16_t gps_flags = 0;

//code segment to send to gps to reset odometer
const uint8_t odo_reset[] = {
     0xB5, 0x62, 0x01, 0x10, 0x00, 0x00, 0x11, 0x34
};

//helper variables for getting gps data
volatile uint16_t gps_payload[200];
uint16_t gps_payload_index = 0;
volatile int16_t payload_size = 0;
volatile uint8_t  curr_gps_byte = 0;

//variables used for identifying data segment to parse
uint8_t class = 0;
uint8_t id = 0;
uint32_t classid;

//variables to store current data
uint32_t curr_lat = 0;
uint32_t curr_lon = 0;
uint32_t curr_tow = 0;
uint32_t diff_tow = 0;
uint32_t curr_dist = 0;
uint16_t curr_year = 0;
uint8_t  curr_month = 0;
uint8_t  curr_day = 0;
uint8_t  curr_hour = 0;
uint8_t  curr_minute = 0;
uint8_t  curr_second = 0;
uint8_t  curr_sats = 0;
uint32_t curr_speed = 0;
uint32_t old_tow = 0;

//states used to parse gps data
enum state_type{
    get_sync_1,
    get_sync_2,
    get_class,
    get_ID,
    get_length_1,
    get_length_2,
    get_payload,
    get_check_a,
    parse_payload
};


uint16_t get_gps_flags(void)
{
    return gps_flags;
}
void reset_gps_flags(uint16_t flags)
{
    gps_flags &= ~flags;
    return;
}
uint32_t get_curr_lat(void)
{
    return curr_lat;
}
uint32_t get_curr_lon(void)
{
    return curr_lon;
}
uint32_t get_curr_speed(void)
{
   return curr_speed;
}
uint32_t get_curr_tow(void)
{
    return curr_tow;
}
uint32_t get_diff_tow(void)
{
    return diff_tow;
}
uint32_t get_curr_dist(void)
{
   return curr_dist;
}
uint16_t get_curr_year(void)
{
   return curr_year;
}
uint8_t get_curr_month(void)
{
   return curr_month;
}
uint8_t get_curr_day(void)
{
   return curr_day;
}
uint8_t get_curr_hour(void)
{
   return curr_hour;
}
uint8_t get_curr_minute(void)
{
   return curr_minute;
}
uint8_t get_curr_second(void)
{
   return curr_second;
}

void reset_gps_odometer(void)
{
    uint8_t reset_index;
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A2->TXBUF = 0xFF;
    __delay_cycles(12000000);
    for (reset_index = 0; reset_index <= 7; reset_index++){
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = odo_reset[reset_index];
    }
    return;
}

void reset_tow(void)
{
    old_tow = 0;
    return;
}

void gps_parse_logic(void)
{
    switch((class << 8) | id)
    {
        //class, id indicates a NAV message with lat, lon, and tow
        case 0x0102:
            curr_lon = (gps_payload[7] << 24)| (gps_payload[6] << 16) | (gps_payload[5] << 8) | gps_payload[4];
            curr_lat = (gps_payload[11]<< 24)| (gps_payload[10] << 16)| (gps_payload[9] << 8) | gps_payload[8];
            curr_tow = (gps_payload[3] << 24)| (gps_payload[2] << 16) | (gps_payload[1] << 8) | gps_payload[0];
            if (old_tow == 0)
                old_tow = curr_tow;
            diff_tow = curr_tow - old_tow;
            gps_flags |= new_data_flag;
            break;
        //class, id indicates a NAV message with the odometer data
        case 0x0109:
            curr_dist = (gps_payload[11] << 24)| (gps_payload[10] << 16) | (gps_payload[9] << 8) | gps_payload[8];
            gps_flags |= new_data_flag;
            break;
        case 0x0121:
            curr_year = (gps_payload[13] << 8) | (gps_payload[12]);
            curr_month = gps_payload[14];
            curr_day = gps_payload[15];
            if (gps_payload[16] < 7)
                curr_hour = 17 + gps_payload[16];
            else
                curr_hour = gps_payload[16] - 7;
            curr_minute = gps_payload[17];
            curr_second = gps_payload[18];
            gps_flags |= new_data_flag;
            break;
        case 0x0135:
            curr_sats = gps_payload[5];
            if (curr_sats >= 10)
                gps_flags |= data_valid_flag;
            gps_flags |= new_data_flag;
            break;
        case 0x0112:
            curr_speed = (gps_payload[23] << 24)| (gps_payload[22] << 16) | (gps_payload[21] << 8) | gps_payload[20];
            gps_flags |= new_data_flag;
            break;
        case 0x0500:
            gps_flags &= ~gps_ack_flag;
            break;
        case 0x0501:
            gps_flags |= gps_ack_flag;
            break;
    }
    return;
}

void gps_FSM(void)
{
    static uint8_t CK_A = 0;
    uint8_t CK_A_test = 0;
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
            if (gps_payload_index >= 200){
                state = parse_payload;
                break;
            }
            gps_payload[gps_payload_index] = curr_gps_byte;
            gps_payload_index++;
            payload_size--;
            if (payload_size == 0)
                state = parse_payload;
            break;
        case get_check_a:
            CK_A_test = curr_gps_byte;
            if (CK_A_test == CK_A)
                state = parse_payload;
            else
                state = get_sync_1;
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
    CS->CTL1 = CS_CTL1_SELS_3 | CS_CTL1_SELM_3 | CS_CTL1_SELA__REFOCLK;
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
    //enable interrupts for UART A0 on NVIC
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
