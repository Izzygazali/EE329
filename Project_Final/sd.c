#include "sd.h"

FATFS FatFs;
FIL Fil;
UINT bw;
uint16_t file_number = 1;

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

int init_log_file(void)
{
    f_mount(&FatFs, "", 0);
    if (f_open(&Fil, create_log_file(), FA_WRITE | FA_CREATE_ALWAYS) == FR_OK)
        return 0;
    return -1;
}

void write_log_file(char * string_to_write)
{
    uint16_t byte_num;
    byte_num = strlen(string_to_write);
    f_write(&Fil, string_to_write, byte_num, &bw);
    return;
}

void close_log_file(void)
{
    f_close(&Fil);
    return;
}

void write_header(void)
{
    char file_name[6];
    sprintf(file_name, "log_%d", file_number-1);
    write_log_file("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    write_log_file("<gpx version=\"1.0\">\r\n");
    write_log_file("<trk><name>");
    write_log_file(file_name);
    write_log_file("</name><number>1</number><trkseg>\r\n");
    return;
}

void write_gps_coord()
{
    write_log_file("<trkpt lat=\"");
    write_log_file(latitude_to_string());
    write_log_file("\" lon=\"");
    write_log_file(longitude_to_string());
    write_log_file("\"><ele>");
    write_log_file(alt_to_string());
    write_log_file("</ele><time>");
    write_log_file(date_to_string());
    write_log_file("T");
    write_log_file(time_to_string());
    write_log_file("Z</time></trkpt>");
    return;
}

void end_log_file(void)
{
    write_log_file("</trkseg></trk>\r\n");
    write_log_file("</gpx>");
    return;
}
