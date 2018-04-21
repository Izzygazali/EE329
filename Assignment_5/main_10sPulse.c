/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/21/2018
 * Description: Program that outputs a 50% duty cycle clock signal to port 1.0
 *              with a 20s period.
 */
#include "msp.h"
#define CCR0_COUNT 30250

int main(void) {
    //Disable Watchdog Timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;


    //Set DCO clock frequency and select as source for SMCLK
    CS ->KEY = CS_KEY_VAL;              //enable writing to clock systems
    CS ->CTL0 = 0;                      //clear control register 0
    CS ->CTL0 |= CS_CTL0_DCORSEL_0;     //set DCO freq to 1.5 MHz
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;  //Set DCO as the source for SMCLK
    CS ->KEY = 0;                       //disable writing to clock systems

    // Configure GPIO
    P1->DIR |= BIT0;
    P1->OUT |= BIT0;

    // TACCR0 interrupt enabled
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    //Set CCR0 value that generate an interrupt every 20ms
    TIMER_A0->CCR[0] = CCR0_COUNT;

    // SMCLK, up mode for 20s period pulse
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    // Enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);

    while (1);
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void)
{
    //When 500 interrupts have been generated, toggle the clock signal.
    //Interrupts are generated every 20ms, toggeling the signal every 500 ISRs
    //will generated a 50% duty cycle clock signal with a 10s wide pulse.
    static uint16_t countISR = 0;
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG){
        countISR += 1;
        if (countISR == 500){
            P1->OUT ^= BIT0;
            countISR = 0;
        }
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
}





