#include "msp.h"
#include "keypad.h"
#include "LCD.h"

/* States
 * 0 => LOCKED
 * 1 => INPUTTING PASS
 * 2 => WRONG PASS
 * 3 => RIGHT PASS
 */

#define LOCKED 0
#define UNLOCKED 1
#define CLEAR 2
#define CORRECT_KEY 5

int key;

void LOCK_FSM()
{
    int PS = LOCKED;
    int OLD_PS = PS;
    int NS;
    while(PS != OLD_PS)
    {
        switch (PS){
            case LOCKED:
                if (key = CORRECT_KEY){
                    NS = UNLOCKED;
                }else{
                    NS = LOCKED;
                }
            case UNLOCKED:
                NS = LOCKED;
            case CLEAR:
                NS = LOCKED;
            case default:
                NS = LOCKED;
                return;
        }
        PS = NS;
    }S
}


void INIT_COMP()
{
    LCD_init();
    INIT_KEYPAD();
    return;
}




void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    INIT_COMP();

    uint8_t state = 0;



}


/*void main(void)
{
    int oldKey = 0;
    LCD_init();
    LCD_CLR();
    INIT_KEYPAD();
    WDTCTL = WDTPW | WDTHOLD;
    while(1){
        uint8_t key = GET_CHAR_KEYPAD();
        if (key != oldKey && key != 0xFF){
            LCD_CLR();
            LCD_HOME();
            WRITE_CHAR_LCD(key);
            P2->OUT = key & (LEDR + LEDG + LEDB);
            oldKey = key;
        }else if(key == 0xFF){
            LCD_CLR();
            LCD_HOME();
            P2->OUT &= ~(LEDR + LEDG + LEDB);
            oldKey = key;
        }
        __delay_cycles(5000);
    }

}*/
