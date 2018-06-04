#include "msp.h"
#include "string_conv.h"

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//returns a pointer to the address of the char string.
	//char * p;
	//p = speed_to_string();

	return;

}

