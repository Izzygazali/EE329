/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/25/2018
 * Description: Program that rotates a SG90 servo motor between 0 and 180 degrees
 *              in increments of 10 degrees. The user can select the angle by
 *              entering a number between 00 and 18 on the keypad, or by pressing
 *              the * or # key to increment/decrement by 10 degrees.
 */
#include "msp.h"
#include "keypad.h"
#include "servo.h"


void main(void)
{
    //variable definitions
    uint8_t count = 0;          //used to determine number of digits entered on keypad
    uint8_t angle = 0;          //angle the servo will be rotated to. Actual angle is 10 times this value.

    //disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    //initialize PWM on TimerA2 and intialize keypad
    PWM_init();
    INIT_KEYPAD();

    while(1)
    {
        if(get_flags() == 1)                            //if button pressed on keypad.
        {
            if(get_input() == '*' && (angle > 0))           //if button = *
            {
                angle--;                                    //decrement current angle by 10 degrees
                servo_angle(angle);                         //send new angle to servo
            }
            else if(get_input() == '#' && (angle < 18))     //if button = #
            {
                angle++;                                    //increment current angle by 10 degrees
                servo_angle(angle);                         //send new angle to servo
            }
            else if(get_input() >= 48 && get_input() <= 57) //if input between 0 and 9
            {
                if(count == 0)                              //if input is the first digit
                {
                    count++;                                //increment digit count
                    angle = (get_input()-48)*10;            //multiply digit by 10 so digit is in 10's place,
                }                                           //and place it in angle.
                else if(count == 1)                         //if input is the second digit
                {
                    count = 0;                              //reset digit count
                    angle += (get_input()-48);              //add digit to current angle value
                    servo_angle(angle);                     //send new angle to servo
                }
            }
            set_flags(0);                                   //set flag to 0.
        }
    }
}



