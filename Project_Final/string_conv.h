/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 06/02/2018
 * Description: This library file implements useful functions
 *              for converting the gathered data from the gps and MPL3115A2
 *              modules into string in the desired format.
 *
 * NOTE: This file makes heavy use of the sprintf() function which could be problematic
 * in certain cases, however, for our particular application using the function is not an issue
 */

#include "msp.h"
#include "gps.h"
#include "Altimeter.h"
#include <stdio.h>


//function prototypes
char * date_to_string();
char * time_to_string();
char * longitude_to_string();
char * latitude_to_string();
char * temp_to_string();
char * alt_to_string();
char * speed_to_string();
char * tow_to_string();
char * dist_to_string();
char * pace_to_string();
