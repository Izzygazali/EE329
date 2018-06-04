#include "msp.h"

//Device Identifier values
#define MPL3115A2_ADDRESS       0x60        //standard 7-bit I2C slave address
#define MPL3115A2_WHOAMI        0x0C        //device ID register

//Control Register 1
#define CTRL_REG1               0x26        //register address
#define CTRL_REG1_OSR_OFFSET    0x03        //offset for over sample ratio
#define CTRL_REG1_SBYB          0x01        //set active mode
#define CTRL_REG1_OST           0x02        //OST bit will initiate an immediate measurement
#define CTRL_REG1_RST           0x04        //software reset
#define CTRL_REG1_ALT           0x80        //Altimeter mode

//Data Registers
//altitude:     20-bit signed integer in meters
//pressure:     20-bit unsigned integer in Pascal
//temperature:  12-bit signed fractional value in Celsius, LSB represents the fractional part
#define REGISTER_PRESSURE_MSB   0x01        //bits(19-12) of pressure/altitude data
#define REGISTER_PRESSURE_CSB   0x02        //bits(11-4) of pressure/altitude data
#define REGISTER_PRESSURE_LSB   0x03        //bits(3-0) of pressure/altitude data, upper 4-bits of byte
#define REGISTER_TEMP_MSB       0x04        //bits(11-4) of temperature data
#define REGISTER_TEMP_LSB       0x05        //bits(3-0) of temperature data, upper 4-bits of byte

//Sensor status register
#define REGISTER_STATUS         0X00        //register address
#define REGISTER_STATUS_TDR     0x02        //new temperature data available
#define REGISTER_STATUS_PDR     0x04        //new pressure/altitude data available

//Barometric input for altitude calculations. Input is unsigned 16 bit integer and
//value is in units of 2 Pa.
#define BAR_IN_MSB              0x14        //MSB of Barometric input
#define BAR_IN_LSB              0x15        //LSB of Barometric input

//Pressure data, temperature data, and event flag generator register
#define PT_DATA_CFG             0x13        //register address
#define PT_DATA_CFG_TDEFE       0x01        //Data event flag for new temperature data
#define PT_DATA_CFG_PDEFE       0x02        //Data event flag for new pressure/altitude data
#define PT_DATA_CFG_DREM        0x04        //Data ready event mode

//function prototypes
void Init_I2C(uint8_t Device_Address);
void Init_MPL3115A2(void);
void Write_MPL3115A2(uint8_t MemAddress, uint8_t MemByte);
void set_sea_pressure(float pascal);
void EUSCIB0_IRQHandler(void);
float get_altitude(void);
float get_temperature(void);
uint8_t Read_MPL3115A2(uint8_t MemAddress);
