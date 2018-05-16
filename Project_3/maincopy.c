#include "msp.h"

void UART_init(void){
    //GPIO settings: P1.2 RX, P1.3 TX
    P1 -> SEL0 |=  (BIT2 | BIT3);
    P1 -> SEL1 &= ~(BIT2 | BIT3);

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_2;           // Set DCO to 6MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELS_3;              // SMCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses


    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SWRST;               //hold in reset state
    EUSCI_A0 -> CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK  |       //set SMCLK as source
                         EUSCI_A_CTLW0_SWRST;               // Remain eUSCI in reset

    EUSCI_A0 -> BRW = 39;                                   //baud rate = 39 => 6MHz/9600/16
    EUSCI_A0 -> MCTLW = (1 << EUSCI_A_MCTLW_BRF_OFS)     |  //First modulation stage select
                        (2 << EUSCI_A_MCTLW_BRS_OFS)     |  //Second modulation stage select
                         EUSCI_A_MCTLW_OS16;                //Oversampling
    EUSCI_A0 -> CTLW0 &= ~EUSCI_A_CTLW0_SWRST;              //reset released for operation

    EUSCI_A0 -> IE |= EUSCI_A_IE_RXIE;                      //receive interrupt enable
    EUSCI_A0 -> IFG &= ~EUSCI_A_IFG_RXIFG;                  //clear receive interrupt flag
    NVIC -> ISER[0] = 1 <<(EUSCIA0_IRQn & 31);
}


//define states
enum state_type{
    reset_state,
    get_DC_offset,
    get_wave_freq,
    sample_wave,
    perform_calculations,
    display_results
};


//define events
enum event_type{
    reset,
    DC_offset_set,
    wave_freq_set,
    wave_sampled,
    calculations_done,
    results_displayed
};

enum event_type event;



void main(void){
    WDTCTL = WDTPW | WDTHOLD;   //disable watchdog timer
    UART_init();

    enum state_type state;
    state = get_DC_offset;

    while(1)
    {
        switch(state)
        {
        case get_DC_offset:
            //DC offset function
            if(event == DC_offset_set)
                state = get_wave_freq;
            break;
        case get_wave_freq:
            //get freq function
            if(event == wave_freq_set)
                state = sample_wave;
            break;
        case sample_wave:
            //sample wave function
            if(event == wave_sampled)
                state = perform_calculations;
            break;
        case perform_calculations:
            //calculations function
            if(event == calculations_done)
                state = display_results;
            break;
        case display_results:
            //display results function
            if(event == results_displayed)
                state = reset_state;
            break;
        case reset_state:
            //reset variables
            state = get_DC_offset;
            event = reset;
            break;
        default:
            state = reset_state;
            break;
        }
    }
}

//EUSCI_A0 ISR
void EUSCIA0_IRQHandler(void)
{
    uint8_t input;                                          //key pressed by user

    //only enter if interrupt flag is set
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        input = (EUSCI_A0 -> RXBUF);                        //get key pressed by user

        //if a number 0-9 is entered
        if(input >= 49 && input <= 53)
        {
            input -= 48;
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));    //echo entered number
            EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;

            if(input == 1)
                event = DC_offset_set;
            else if(input == 2)
                event = wave_freq_set;
            else if(input == 3)
                event = wave_sampled;
            else if(input == 4)
                event = calculations_done;
            else if(input == 5)
                event = results_displayed;

        }
    }
}
