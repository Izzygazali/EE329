#include "msp.h"
#include "keypad.h"

#define LOCKED 0
#define UNLOCKED 1
#define CLEAR 2
uint8_t keyValue;
const uint8_t correctKey[] = {1,2,3,4};

void ENTER_PASSWORD(uint8_t keyValue){
    static uint8_t i;
    static uint8_t enteredKey[];
    for(i = 0; i < 4; i++){
        enteredKey[i] = keyValue;
    }
}

uint8_t CHECK_PASSWORD(uint8_t enteredKey[]){
    uint8_t i;
    for(i = 0; i < 4; i++){
        if(enteredKey[i] != correctKey[i]){
            return 0;
        }
    }
    return 1;
}

void LOCK_SYSTEM_FSM()
{
    static uint8_t PS = LOCKED;
    uint8_t NS;
    while(1)
    {
        switch(PS)
        {
            case LOCKED:
                Clear_LCD();
                Return_Home();
                LCD_Write_String("LOCKED STATE");
                LCD_newLine();
                LCD_Write_String("ENTER KEY:");
                break;
            case UNLOCKED:
                Clear_LCD();
                Return_Home();
                LCD_Write_String("HELLO WORLD!");
                break;
            case CLEAR:
                Clear_LCD();
                Return_Home();
                NS = LOCKED;
                break;
            default:
                NS = LOCKED;
                break;
        }

        PS = NS
    }
}







int main(void) {
    WDTCTL = WDTPW | WDTHOLD;                       // Stop watchdog timer
    LCD_init();
    KEYPAD_INIT();
    __enable_irq();                                 // Enable global interrupt
    while(1);
}

// Port5 ISR
void PORT5_IRQHandler(void){
        keyValue = KEYPAD_GET_KEY();
     //   LOCK_SYSTEM_FSM();
        P5->IFG &= ~(ROW1 + ROW2 + ROW3 + ROW4);
}



