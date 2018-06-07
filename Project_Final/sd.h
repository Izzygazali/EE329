#include <stdio.h>  /* Device specific declarations */
#include "ff.h"     /* Declarations of FatFs API */
#include "msp.h"
#include "delay.h"
#include "string.h"
#include "string_conv.h"

char * create_log_file(void);
int init_log_file(void);
void write_log_file(char * string_to_write);
void close_log_file(void);
void write_header(void);
void write_gps_coord();
void end_log_file(void);
