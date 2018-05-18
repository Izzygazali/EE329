#include "msp.h"

#define CLEAR_SCREEN            "\x1B[2K"
#define CURSOR_HOME             "\x1B[H"
#define CURSOR_POSITION_FREQ    "\x1B[5;15H"
#define CURSOR_POSITION_VALID   "\x1B[5;70H"
#define CURSOR_POSITION_VPP     "\x1B[6;14H"
#define CURSOR_POSITION_MAX     "\x1B[7;14H"
#define CURSOR_POSITION_MIN     "\x1B[8;14H"
#define CURSOR_POSITION_RMS     "\x1B[9;14H"
#define CURSOR_POSITION_DC      "\x1B[10;14H"
#define CURSOR_POSITION_RMS_BAR "\x1B[9;24H"
#define CURSOR_POSITION_DC_BAR  "\x1B[10;24H"
#define COLOR_BACKGROUND_BLACK  "\x1B[40m"
#define COLOR_BACKGROUND_RED    "\x1B[41m"
#define COLOR_BACKGROUND_GREEN  "\x1B[42m"
#define COLOR_FOREGROUND_RED    "\x1B[31m"
#define COLOR_FOREGROUND_WHITE  "\x1B[37m"
#define COLOR_FOREGROUND_GREEN  "\x1B[32m"

void UART_write_string(char inputString[]);
void check_valid(uint16_t FREQ, uint16_t MAX, uint16_t MIN, uint16_t RMS, uint16_t DC);
void UART_init(void);
void initialize_console(void);
uint16_t binary_to_bcd(uint16_t binary_number);
void voltage_to_console(uint16_t voltage);
void freq_to_console(uint16_t frequency);
void set_voltage_bars(uint16_t voltage);
void update_display(uint16_t FREQ, uint16_t MAX, uint16_t MIN, uint16_t RMS, uint16_t DC);
