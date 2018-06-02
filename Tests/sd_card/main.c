/*----------------------------------------------------------------------*/
/* Foolproof FatFs sample project for AVR              (C)ChaN, 2014    */
/*----------------------------------------------------------------------*/

#include <stdio.h>	/* Device specific declarations */
#include "ff.h"		/* Declarations of FatFs API */
#include "msp.h"
#include "delay.h"

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */


int main (void)
{
    set_DCO(FREQ_12_MHz);
	UINT bw;


	f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

	if (f_open(&Fil, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {	/* Create a file */

		f_write(&Fil, "It works!\r\n", 11, &bw);	/* Write data to the file */

		f_close(&Fil);								/* Close the file */

		if (bw == 11) {		/* Lights green LED if data written well */
			P1->DIR |= BIT0;
			P1->OUT |= BIT0;
		}
	}

	for (;;) ;
}


