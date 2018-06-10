/* Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 06/01/2018
 * Description: This library file implements utilizes the FatFS
 * library to write data to log files on an sd card using the
 * FAT files system.
 *
 * Pin Assignments: P1.5 -> SCLK
 *                  P1.6 -> MCU MOSI
 *                  P1.7 -> MCU MISO
 *                  P4.0 -> CS
 */

//include necassary library files for sd.c
#include "ff.h"
#include "msp.h"
#include "string.h"
#include "string_conv.h"

//define functions implemented in sd.c
char * create_log_file(void);
int init_log_file(void);
void write_log_file(char * string_to_write);
void close_log_file(void);
void write_header(void);
void write_gps_coord();
void end_log_file(void);
