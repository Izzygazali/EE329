#include "msp.h"
#include "LCD.h"
#include "keypad.h"

//Define States of the FSM
#define     LOCKED      0
#define     UNLOCKED    1
#define     CLEAR       2

//Clear key is set to he * button
#define     CLR_KEY     42

//Correct Password: set to 1234
static const uint8_t CORRECT_KEY[] = {49,50,51,52};

//Global variable, digit from keypad
extern int digit;


/*
 * Struct that defines two variables of type uint8_t, NS and FLG_repeat. The
 * struct allows the logic functions to return FSM control variables at once.
 * NS           => next state of the FSM
 * FLG_repeat   => flag to indicate whether the program needs to iterate
 *                 through the FSM again. This flag is used to free the MCU for
 *                 other tasks when no state change is required.
 *                 1 iterate through FSM
 *                 0 return to main. (free to perform other tasks)
 */
struct FSM_CONTROL {
    uint8_t NS;
    uint8_t FLG_repeat;
};

uint8_t CHECK_KEY(uint8_t entered_key, uint8_t index);
struct FSM_CONTROL LOCKED_LOGIC(void);
struct FSM_CONTROL UNLOCKED_LOGIC(void);
struct FSM_CONTROL CLEAR_LOGIC(void);
void LOCK_FSM();

