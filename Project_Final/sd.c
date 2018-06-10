/*
 * Engineer(s): Ezzeddeen Gazali and Tyler Starr
 * Create Date: 06/01/2018
 */
#include "sd.h"

//define variables used for FatFS library
FATFS FatFs;
FIL Fil;
UINT bw;
//define file number for determining next file to creat
uint16_t file_number = 1;
/*
 * Function determines the name of the next log file to create
 * and returns its name as a pointer to a string
 * INPUTS   NONE
 * RETURN   char * file_to_create = name of next file to create as string
 */
char * create_log_file(void)
{
    FRESULT fr = FR_OK;
    FILINFO fno;
    file_number = 1;
    static char file_to_create[10];
    while(fr == FR_OK){
        sprintf(file_to_create, "log_%d.gpx", file_number);
        fr = f_stat(file_to_create, &fno);
        file_number++;
    }
    return file_to_create;
}
/*
 * Function creates and opens the log file for editing
 * INPUTS   NONE
 * RETURN   int incicating success of failure to open log file
 */
int init_log_file(void)
{
    //mount system
    f_mount(&FatFs, "", 0);
    //if file is open return 0
    if (f_open(&Fil, create_log_file(), FA_WRITE | FA_CREATE_ALWAYS) == FR_OK)
        return 0;
    //if file failes to open return -1
    return -1;
}
/*
 * Function writes and input string to the log file
 * INPUTS   char * string_to_write = string the write to log file
 * RETURN   NONE
 */
void write_log_file(char * string_to_write)
{
    //determine the number of bytes in string for f_write function
    uint16_t byte_num;
    byte_num = strlen(string_to_write);
    //use f_write to write string to log file
    f_write(&Fil, string_to_write, byte_num, &bw);
    return;
}
/*
 * Function writes header needed for GPX formatted log file
 * INPUTS   NONE
 * RETURN   NONE
 */
void write_header(void)
{
    //write log name in header
    char file_name[6];
    sprintf(file_name, "log_%d", file_number-1);
    write_log_file("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    write_log_file("<gpx version=\"1.0\">\r\n");
    write_log_file("<trk><name>");
    write_log_file(file_name);
    write_log_file("</name><number>1</number><trkseg>\r\n");
    return;
}
/*
 * Function a single data point in the opened log file.
 * INPUTS   NONE
 * RETURN   NONE
 */
void write_gps_coord()
{
    //write latitude to log
    write_log_file("<trkpt lat=\"");
    write_log_file(latitude_to_string());
    //write longitude to log
    write_log_file("\" lon=\"");
    write_log_file(longitude_to_string());
    //write elevation to log
    write_log_file("\"><ele>");
    write_log_file(alt_to_string());
    write_log_file("</ele><time>");
    //write the data and time to log. Each gps coord needs time stamp.
    write_log_file(date_to_string());
    write_log_file("T");
    write_log_file(time_to_string());
    write_log_file("Z</time></trkpt>\r\n");
    return;
}
/*
 * Function writes and final lines to the log file for GPX format
 * INPUTS   NONE
 * RETURN   NONE
 */
void end_log_file(void)
{
    write_log_file("</trkseg></trk>\r\n");
    write_log_file("</gpx>");
    return;
}
/*
 * Function closes the log file concluding logging
 * INPUTS   NONE
 * RETURN   NONE
 */
void close_log_file(void)
{
    f_close(&Fil);
    return;
}
