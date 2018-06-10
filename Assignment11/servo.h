/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 05/26/2018
 * Description: This library file implements useful functions for setting up
 *              and utilizing the SG90 Servo motor. The functions are documented
 *              prior to their source code.
 *
 * Servo Pin Assignments:  P6.6 -> PWM to servo
 */
#include "msp.h"

//useful definitions
#define PERIOD_COUNT 60250

//function prototypes
void PWM_init(void);
void servo_angle(uint8_t degree);
