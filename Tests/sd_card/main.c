/*----------------------------------------------------------------------*/
/* Foolproof FatFs sample project for AVR              (C)ChaN, 2014    */
/*----------------------------------------------------------------------*/

#include <stdio.h>	/* Device specific declarations */
#include "ff.h"		/* Declarations of FatFs API */

#include "msp.h"
#include "delay.h"
#include "string.h"

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

char * determine_file_name(void)
{
    char * file_to_create;
    uint16_t file_number = 1;
    FRESULT fr = FR_OK;
    FILINFO fno;
    while(fr == FR_OK){
        sprintf(file_to_create, "log_%d.txt", file_number);
        fr = f_stat(file_to_create, &fno);
        file_number++;
    }

    return file_to_create;
}
int init_sd_card(void)
{
    char * file_to_create;
    f_mount(&FatFs, "", 0);
    file_to_create = determine_file_name();
    if (f_open(&Fil, file_to_create, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK)
        return -1;
    else
        return 0;
}

int main (void)
{
    set_DCO(FREQ_12_MHz);
    init_sd_card();
    /*
    UINT bw;


	f_mount(&FatFs, "", 0);		Give a work area to the default drive

	if (f_open(&Fil, "duckfuck.gpx", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {	 Create a file

		f_write(&Fil, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\r \
		               <gpx version=\"1.0\">\r\n \
		               <name>Example gpx</name>\r\n \
		               <wpt lat=\"46.57638889\" lon=\"8.89263889\">\r\n \
		               <ele>2372</ele>\r\n \
                       <name>LAGORETICO</name>\r\n \
		               </wpt>\r\n \
		               <trk><name>Example gpx</name><number>1</number><trkseg>\r\n \
                       <trkpt lat=\"46.57608333\" lon=\"8.89241667\"><ele>2376</ele><time>2007-10-14T10:09:57Z</time></trkpt>\r\n \
                       <trkpt lat=\"46.57619444\" lon=\"8.89252778\"><ele>2375</ele><time>2007-10-14T10:10:52Z</time></trkpt>\r\n \
                       <trkpt lat=\"46.57641667\" lon=\"8.89266667\"><ele>2372</ele><time>2007-10-14T10:12:39Z</time></trkpt>\r\n \
                       <trkpt lat=\"46.57650000\" lon=\"8.89280556\"><ele>2373</ele><time>2007-10-14T10:13:12Z</time></trkpt>\r\n \
                       <trkpt lat=\"46.57638889\" lon=\"8.89302778\"><ele>2374</ele><time>2007-10-14T10:13:20Z</time></trkpt>\r\n \
                       <trkpt lat=\"46.57652778\" lon=\"8.89322222\"><ele>2375</ele><time>2007-10-14T10:13:48Z</time></trkpt>\r\n \
                       <trkpt lat=\"46.57661111\" lon=\"8.89344444\"><ele>2376</ele><time>2007-10-14T10:14:08Z</time></trkpt>\r\n \
		               </trkseg></trk>\r\n \
		               </gpx>\r\n", 1350, &bw);

		f_close(&Fil);

		if (bw == 15) {		 Lights green LED if data written well
			P1->DIR |= BIT0;
			P1->OUT |= BIT0;
		}
	}*/

	for (;;) ;
}


