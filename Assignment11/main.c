/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/25/2018
 * Description:
 */
#include "msp.h"
#include "keypad.h"
#include "servo.h"


void main(void){
    uint8_t count = 0;
    uint8_t angle = 0;

    //disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    PWM_init();
    INIT_KEYPAD();

    while(1)
    {
        if(get_flags() == 1)
        {
            if(get_input() == '*' && (angle > 0))
            {
                angle--;
                servo_angle(angle);
            }
            else if(get_input() == '#' && (angle < 18))
            {
                angle++;
                servo_angle(angle);
            }
            else if(get_input() >= 48 && get_input() <= 57)
            {
                if(count == 0)
                {
                    count++;
                    angle = (get_input()-48)*10;
                }
                else if(count == 1)
                {
                    count = 0;
                    angle += (get_input()-48);
                    servo_angle(angle);
                }
            }
            set_flags(0);
        }
    }
}



