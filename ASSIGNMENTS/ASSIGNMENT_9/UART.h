/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     5/7/2018
 * Description: Library file containing useful functions for setting DAC level
 *              through UART communication
 */
#include "msp.h"
#include <stdio.h>
#include <string.h>

#define CLEAR_LINE          "\x1B[2K"
#define CURSOR_HOME         "\x1B[H"

//function prototypes.
void UART_init(void);
void UART_write_string(char inputString[]);

