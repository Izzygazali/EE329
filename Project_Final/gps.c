/*
 * Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 05/22/2018
 */
#include "gps.h"
//flags described in gps.h
volatile uint16_t gps_flags = 0;
//code segment sent to gps, resets odometer
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

/*
 * Function returns the gps flag for use in other functions
 * INPUTS   NONE
 * RETURN   uint16_t gps_flags = flags for gps as defined in gps.h
 */
uint16_t get_gps_flags(void)
{
    return gps_flags;
}
/*
 * Function resets flags in the gps_flags variable
 * INPUTS   uint16_t flags = flags to reset
 * RETURN   NONE
 */
void reset_gps_flags(uint16_t flags)
{
    gps_flags &= ~flags;
    return;
}
/*
 * Function returns the current latitude in degrees
 * INPUTS   NONE
 * RETURN   uint32_t curr_lat = latitude scaled by 10^7
 */
uint32_t get_curr_lat(void)
{
    return curr_lat;
}
/*
 * Function returns the current longitude in degrees
 * INPUTS   NONE
 * RETURN   uint32_t curr_lon = longitude scaled by 10^7
 */
uint32_t get_curr_lon(void)
{
    return curr_lon;
}
/*
 * Function returns the current speed in cm/s
 * INPUTS   NONE
 * RETURN   uint32_t curr_speed = speed in cm/s
 */
uint32_t get_curr_speed(void)
{
   return curr_speed;
}
/*
 * Function returns the current time of the week in ms
 * INPUTS   NONE
 * RETURN   uint32_t curr_tow = time of week in ms
 */
uint32_t get_curr_tow(void)
{
    return curr_tow;
}
/*
 * Function returns the difference between the current time of week
 * and the time of week at the start of logging
 * INPUTS   NONE
 * RETURN   uint32_t diff_tow = logging time in ms
 */
uint32_t get_diff_tow(void)
{
    return diff_tow;
}
/*
 * Return the current distance traveled, from gps odometer, in meters
 * INPUTS   NONE
 * RETURN   uint32_t curr_dist = distance travelled in meters
 */
uint32_t get_curr_dist(void)
{
   return curr_dist;
}
/*
 * Function returns current year
 * INPUTS   NONE
 * RETURN   uint16_t curr_year = returns year (i.e. 2018)
 */
uint16_t get_curr_year(void)
{
   return curr_year;
}
/*
 * Function returns current month
 * INPUTS   NONE
 * RETURN   uint8_t curr_month = returns month (01-12)
 */
uint8_t get_curr_month(void)
{
   return curr_month;
}
/*
 * Function returns current day of the month
 * INPUTS   NONE
 * RETURN   uint8_t curr_day = returns day of month (0-31)
 */
uint8_t get_curr_day(void)
{
   return curr_day;
}
/*
 * Function returns current hour of the day in 24 hour format
 * INPUTS   NONE
 * RETURN   uint8_t curr_month = returns current hour (0-24)
 */
uint8_t get_curr_hour(void)
{
   return curr_hour;
}
/*
 * Function returns current minute of hour
 * INPUTS   NONE
 * RETURN   uint8_t curr_month = minute of hour (0-59)
 */
uint8_t get_curr_minute(void)
{
   return curr_minute;
}
/*
 * Function returns current second of minute
 * INPUTS   NONE
 * RETURN   uint8_t curr_month = returns second (0-59)
 */
uint8_t get_curr_second(void)
{
   return curr_second;
}
/*
 * Function resets the odometer on the gps module
 * INPUTS   NONE
 * RETURN   NONE
 */
void reset_gps_odometer(void)
{
    //define variable to shift through array with commands
    uint8_t reset_index;
    //make sure UART isn't currently transmitting
    while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
    //send 0xFF to indicate the beginning of a command
    EUSCI_A2->TXBUF = 0xFF;
    //wait ~500ms as recommended by datasheet
    __delay_cycles(12000000);
    //transmit code segement define above to reset odometer
    for (reset_index = 0; reset_index <= 7; reset_index++){
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = odo_reset[reset_index];
    }
    return;
}
/*
 * Function resets the time of week that the log starts
 * this is used to allow more than one log without a power cycle
 * INPUTS   NONE
 * RETURN   NONE
 */
void reset_tow(void)
{
    //reset time of week to 0
    old_tow = 0;
    return;
}
/*
 * Function identifies the payload structure by the class and id then parses the data into
 * seperate variables for use in other parts of the program
 * INPUTS   NONE
 * RETURN   NONE
 */
void gps_parse_logic(void)
{
    //combines class and id then uses it to determine which parsing logic to use
    switch((class << 8) | id)
    {
        //class, id indicates a NAV message with lat, lon, and tow
        case 0x0102:
            //pick out bytes which correspons to longitude
            curr_lon = (gps_payload[7] << 24)| (gps_payload[6] << 16) | (gps_payload[5] << 8) | gps_payload[4];
            //pick out bytes which correspons to latitude
            curr_lat = (gps_payload[11]<< 24)| (gps_payload[10] << 16)| (gps_payload[9] << 8) | gps_payload[8];
            //pick out bytes which correspons to time of the week
            curr_tow = (gps_payload[3] << 24)| (gps_payload[2] << 16) | (gps_payload[1] << 8) | gps_payload[0];
            //if the time of week isn't set (ie 0) set the old time of week to the curren time of week
            if (old_tow == 0)
                old_tow = curr_tow;
            //calculate the difference between the current and old time of week
            diff_tow = curr_tow - old_tow;
            //set flag indicating new gps data has been recieved
            gps_flags |= new_data_flag;
            break;
        //class, id indicates a NAV message with the odometer data
        case 0x0109:
            //pick out bytes which correspons to current distance
            curr_dist = (gps_payload[11] << 24)| (gps_payload[10] << 16) | (gps_payload[9] << 8) | gps_payload[8];
            //set flag indicating new gps data has been recieved
            gps_flags |= new_data_flag;
            break;
        //class, id indicates a NAV message with year, month, day, hour, minute, and second
        case 0x0121:
            //pick out bytes which correspons to year
            curr_year = (gps_payload[13] << 8) | (gps_payload[12]);
            //pick out bytes which correspons to month
            curr_month = gps_payload[14];
            //pick out bytes which correspons to day
            curr_day = gps_payload[15];
            //convert UTC to PST
            if (gps_payload[16] < 7)
                curr_hour = 17 + gps_payload[16];
            else
                curr_hour = gps_payload[16] - 7;
            //pick out bytes which correspons to minute
            curr_minute = gps_payload[17];
            //pick out bytes which correspons to second
            curr_second = gps_payload[18];
            //set flag indicating new gps data has been recieved
            gps_flags |= new_data_flag;
            break;
        //class, id indicates a NAV message with satellite information
        case 0x0135:
            //pick out bytes which correspons to number of locked satellites
            curr_sats = gps_payload[5];
            //if there are more than 10 satellites set flag indicating gps data is valid
            if (curr_sats >= 10)
                gps_flags |= data_valid_flag;
            //set flag indicating new gps data has been recieved
            gps_flags |= new_data_flag;
            break;
        //class, id indicates a NAV message with velocity information
        case 0x0112:
            //pick out bytes which correspons to speed in cm/s
            curr_speed = (gps_payload[23] << 24)| (gps_payload[22] << 16) | (gps_payload[21] << 8) | gps_payload[20];
            //pick out bytes which correspons to number of locked satellites
            gps_flags |= new_data_flag;
            break;
        //class, id indicates a ACK message with that indicates a NACK
        case 0x0500:
            //set flag indicating the gps NACK
            gps_flags &= ~gps_ack_flag;
            break;
        //class, id indicates a ACK message with that indicates a ACK
        case 0x0501:
            //set flag indicating the gps ACK
            gps_flags |= gps_ack_flag;
            break;
    }
    return;
}
/*
 * Function reconizes and reads in a transmission from the GPS over UART. This FSM is
 * based the reciver protcol description provided for the ublox m8n gps module.
 * INPUTS   NONE
 * RETURN   NONE
 */
void gps_FSM(void)
{
    //define state variable and set to get_sync_1 by default
    static enum state_type state = get_sync_1;
    //switch based on the current state
    switch(state)
    {
        //this state looks for the first byte of the transmission start, 0xB5
        case get_sync_1:
            if (curr_gps_byte == 0xB5){
                state = get_sync_2;
            }else{
                //keep waiting for 0xB5
                state = get_sync_1;
            }
            break;
        //this state looks for the second byte of the transmission start, 0x62
        case get_sync_2:
           if (curr_gps_byte == 0x62){
               state = get_class;
           }else{
               //return to looking for first transmission byte
               state = get_sync_1;
           }
           break;
        //this state reads in the byte corresponding to the class of message
        case get_class:
           //read in the byte for the class of message
           class = curr_gps_byte;
           state = get_ID;
           break;
        //this state reads in the byte corresponding to the id of message
        case get_ID:
           //read in the byte for the id of message
           id = curr_gps_byte;
           state = get_length_1;
           break;
        //this state reads in the first byte corresponding to the length of payload
        case get_length_1:
            //reset payload size
            payload_size = 0;
            //read in the first byte of payload size
            payload_size |= curr_gps_byte;
            state = get_length_2;
            break;
        //this state reads in the second byte corresponding to the length of payload
        case get_length_2:
            //read in the second byte of payload size
            payload_size |= (curr_gps_byte << 8);
            state = get_payload;
            //reset index used to fill payload array
            gps_payload_index = 0;
            break;
        //this state reads in all bytes of the payload into an array for parsing
        case get_payload:
            //catch all to prevent a "freeze" in this state
            if (gps_payload_index >= 200){
                //restart as somthing messed up
                state = get_sync_1;
                break;
            }
            //read in current byte into the payload array
            gps_payload[gps_payload_index] = curr_gps_byte;
            //go to next index in the array
            gps_payload_index++;
            //decrement payload size
            payload_size--;
            //if entire payload has been captured parse payload
            if (payload_size == 0)
                state = parse_payload;
            break;
        case parse_payload:
            //logic to parse payload
            gps_parse_logic();
            //restart to get another gps message
            state = get_sync_1;
            break;
        default:
            //by default start looking for message, catch all
            state = get_sync_1;
            break;
    }
    return;
}
/*
 * Function that starts the UART module used to parse gps data
 * INPUTS   NONE
 * RETURN   NONE
 */
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

/*
 * Interuppt handler that starts gps parsing for each byte
 * recieved from gps
 * INPUTS   NONE
 * RETURN   NONE
 */
void EUSCIA2_IRQHandler(void)
{
    //if GPS data is recieved
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        //set the current gps byte to the RX buffer
        curr_gps_byte = EUSCI_A2->RXBUF;
        //call FSM to handle the new byte of gps data
        gps_FSM();
    }
}
