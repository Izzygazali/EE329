/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/21/2018
 * Description: Reflex Game: Player one presses button on P1.1 followed by
 *              player two pressing the button on P1.4. The relfex time is
 *              displayed in on the LCD in ms.
 */
#include "msp.h"
#include "LCD.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Tuned count for reaction times, count for 1ms
#define CCR0_COUNT 1515

//number of times the timer interrupts.
int wrapAround = 0;

/*
 * Function that writes the reflex time on the LCD.
 * INPUT:   number = Time to display
 */
void write_to_LCD(uint16_t number){
    char number_string[20];
    sprintf(number_string, "%d", number);
    LCD_Write_String(number_string);
    LCD_Write_String("ms");
    return;
}

int main(void) {
    //Disable Watchdog Timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    //LCD initialization and prompt
    set_DCO(FREQ_1_5_MHz);
    LCD_init();
    Clear_LCD();
    Return_Home();
    LCD_Write_String("Player 1|npress button");

    //Set DCO clock frequency and select as source for SMCLK
    CS ->KEY = CS_KEY_VAL;              //enable writing to clock systems
    CS ->CTL1 |= CS_CTL1_SELS__DCOCLK;  //Set DCO as the source for SMCLK
    CS ->KEY = 0;                       //disable writing to clock systems


    // Configure GPIO
    P1->DIR &= ~(BIT1+ BIT4);
    P1->IE |= (BIT1 + BIT4);            // Enable interrupts for P1.1 and P1.4
    P1->REN |= (BIT1 + BIT4);           // Add pull up resistor.
    P1->OUT |= (BIT1+ BIT4);
    P1->IES |= (BIT1 + BIT4);           // Select high to low edge Interrupt
    P1->IFG &= ~(BIT1 + BIT4);          // Clear Interrupt flag.


    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //TACCR0 interrupt enabled
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clear Interrupt flag

    //Set CCR0 value that generate an interrupt every 1ms
    TIMER_A0->CCR[0] = CCR0_COUNT;

    // SMCLK, up mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    // Enable interrupts
    __enable_irq();
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);

    while (1);
}


//Port 1 interrupt service routine
void PORT1_IRQHandler(void)
{
    //flag that is set when player one presses the button on P1.1
    static uint8_t player1_pressed;

    //If P1.1 is pressed
    if(P1->IFG & BIT1){
        wrapAround = 0;                     //reset the wrap around counter
        player1_pressed = 1;                //set player 1 press flag.
        Clear_LCD();                        //prompt player 2 for button press
        Return_Home();
        LCD_Write_String("Player 2|npress button");
    }

    //If P1.4 is pressed and player 1 has already pressed his button
    if((P1->IFG & BIT4) && (player1_pressed == 1)){
        player1_pressed = 0;               //clear player 1 press flag.

        Clear_LCD();                       //display reflex time, wait for 5s then
        Return_Home();                     //prompt player 1 for a button press again
        LCD_Write_String("Reflex time:");
        LCD_newLine();
        write_to_LCD(wrapAround);
        delay_ms(5000, FREQ_1_5_MHz );
        Clear_LCD();
        Return_Home();
        LCD_Write_String("Player 1|npress button");

        wrapAround = 0;                     //reset the wrap around counter
    }
    P1->IFG &= ~(BIT1 + BIT4);
}
//TIMER_A0 interrupt service routine
void TA0_0_IRQHandler(void)
{
    //keep track of the number of times the timer wraps around/resets to 0
    if ( TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG){
        wrapAround ++;
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}



