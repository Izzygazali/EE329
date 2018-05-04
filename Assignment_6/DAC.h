/* Engineers:   Ezzeddeen Gazali and Tyler Starr
 * Created:     4/25/2018
 */
#include "msp.h"

//Define useful parameters
#define UCB0CLK     BIT5
#define SIMO        BIT6
#define SOMI        BIT7
#define GAIN        BIT5
#define SHDN        BIT4
#define CS_DAC      BIT0
#define CCR0_COUNT  60300
#define CCR1_COUNT  30200

//Function prototypes
void SPI_init();
void DAC_init();
void DAC_data(uint16_t data);
void Timer_init();

