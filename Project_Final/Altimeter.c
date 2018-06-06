#include "Altimeter.h"

//global variable definitions
uint16_t TransmitFlag = 0;
uint32_t REG1_CONTROL_WORD = 0;


/*
 * Function that sets the local sea level barometric pressure
 * INPUTS       float pascal = the pressure to use as the baseline
 * RETURN       NONE
 */
void set_sea_pressure(float pascal)
{
    uint16_t sea_level_pressure = pascal/2;             //Barometric input is in 2*Pa
    Write_MPL3115A2(BAR_IN_MSB, sea_level_pressure>>8); //write MSB to barometric input register
    __delay_cycles(CYCLES);                               //delay for write
    Write_MPL3115A2(BAR_IN_LSB, sea_level_pressure);    //write LSB to barometric input register
    __delay_cycles(CYCLES);                               //delay for write
    return;
}

/*
 * Function that gets the floating point altitude in meters
 * INPUTS       NONE
 * RETURN       float altitude = current altitude value
 */
float get_altitude()
{
    //variable definitions
    float altitude;
    int32_t temp_alt;
    uint8_t sta = 0;

    //If a measurement has already been initiated, wait for measurement to complete
    while((Read_MPL3115A2(CTRL_REG1) & CTRL_REG1_OST));
    __delay_cycles(CYCLES);

    //Initiate new measurement
    REG1_CONTROL_WORD |= CTRL_REG1_OST;
    Write_MPL3115A2(CTRL_REG1, REG1_CONTROL_WORD);
    __delay_cycles(CYCLES);

    //wait for new altitude data to be available
    while(0x00 == (sta & REGISTER_STATUS_PDR))
    {
        sta = Read_MPL3115A2(REGISTER_STATUS);
        __delay_cycles(CYCLES);
    }

    //altitude is determined as specified in the MPL3115A2 data sheet
    temp_alt = (Read_MPL3115A2(REGISTER_PRESSURE_MSB))<<24;
    __delay_cycles(CYCLES);
    temp_alt |= (Read_MPL3115A2(REGISTER_PRESSURE_CSB))<<16;
    __delay_cycles(CYCLES);
    temp_alt |= (Read_MPL3115A2(REGISTER_PRESSURE_LSB))<<8;
    altitude = temp_alt;
    altitude /= 65536.0;

    //reset OST bit to indicate no measurement initiated.
    REG1_CONTROL_WORD &= ~CTRL_REG1_OST;
    Write_MPL3115A2(CTRL_REG1, REG1_CONTROL_WORD);
    __delay_cycles(CYCLES);

    return altitude;
}

/*
 * Function that gets the floating point temperature in degrees celsius
 * INPUTS       NONE
 * RETURN       float altitude = current temperature value
 */
float get_temperature(void)
{
    //variable definitions
    float temperature;
    int16_t temp;
    uint8_t sta = 0;

    //wait for new temperature data to be available
    while(0x00 == (sta & REGISTER_STATUS_TDR))
    {
        sta = Read_MPL3115A2(REGISTER_STATUS);
        __delay_cycles(CYCLES);
    }

    //MSB and LSB bits are shifted into their proper location in temp
    temp = Read_MPL3115A2(REGISTER_TEMP_MSB);
    __delay_cycles(CYCLES);
    temp<<=8;
    temp|= Read_MPL3115A2(REGISTER_TEMP_LSB);
    __delay_cycles(CYCLES);
    temp>>=4;

    //check if temperature is negative
    if (temp & 0x800)
        temp |= 0xF000;

    //floating point temperature is divided by 16.0 to put least significant 4-bits
    //in the fractional part of the value.
    temperature = temp;
    temperature /= 16.0;

    return temperature;
}

/*
 * Function that initializes the I2C bus for communicating with MPL3115A2
 * INPUTS       uint8_t Device_Address = address of the MPL3115A2
 * RETURN       NONE
 */
void Init_I2C(uint8_t Device_Address)
{
    P6->SEL1 |= (SDA | SCL);                    // Set I2C pins of eUSCI_B3

    NVIC->ISER[0] = 1 << ((EUSCIB3_IRQn) & 31); // Enable eUSCIB3 interrupt in NVIC module

    EUSCI_B3->CTLW0 |= EUSCI_B_CTLW0_SWRST;     // Software reset enabled
    EUSCI_B3->CTLW0 = EUSCI_B_CTLW0_SWRST   |   // Remain eUSCI in reset mode
                      EUSCI_B_CTLW0_MODE_3  |   // I2C mode
                      EUSCI_B_CTLW0_MST     |   // Master mode
                      EUSCI_B_CTLW0_SYNC    |   // Sync mode
                      EUSCI_B_CTLW0_SSEL__SMCLK;// SMCLK

    EUSCI_B3->BRW = 60;                         // baud rate = SMCLK / 60 = 200kHz
    EUSCI_B3->I2CSA = Device_Address;           // Slave address
    EUSCI_B3->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Release eUSCI from reset

    EUSCI_B3->IE |= EUSCI_A_IE_RXIE |           // Enable receive interrupt
                    EUSCI_A_IE_TXIE;
    return;
}

/*
 * Function that initializes the MPL3115A2 for data measurement
 * INPUTS       NONE
 * RETURN       NONE
 */
void Init_MPL3115A2(void)
{
    //used for debugging to determine if correct device identified
    uint8_t whoami = Read_MPL3115A2(MPL3115A2_WHOAMI);
    __delay_cycles(CYCLES);

    //set device in reset mode
    REG1_CONTROL_WORD = CTRL_REG1_RST;
    Write_MPL3115A2(CTRL_REG1,REG1_CONTROL_WORD);
    __delay_cycles(CYCLES);

    //wait for device to enter reset mode
   // while(Read_MPL3115A2(CTRL_REG1) & CTRL_REG1_RST);

    //set device in Altimeter mode with a sampling rate of 128
    REG1_CONTROL_WORD = CTRL_REG1_ALT | (0x07<<CTRL_REG1_OSR_OFFSET);
    Write_MPL3115A2(CTRL_REG1,REG1_CONTROL_WORD);
    __delay_cycles(CYCLES);

    //enable data event flags and data ready flag generation
    Write_MPL3115A2(PT_DATA_CFG, PT_DATA_CFG_TDEFE   |
                                 PT_DATA_CFG_PDEFE   |
                                 PT_DATA_CFG_DREM);
    __delay_cycles(CYCLES);

    //set device in active mode
    REG1_CONTROL_WORD |= CTRL_REG1_SBYB;
    Write_MPL3115A2(CTRL_REG1, REG1_CONTROL_WORD);
    __delay_cycles(CYCLES);
    return;
}

/*
 * Function that writes a single byte to the MPL3115A2.
 * INPUTS       uint8_t MemAddress = 1 byte MPL3115A2 address to write to
 *              uint8_t MemByte = 1 byte value that is written in the specified address
 * RETURN       NONE
 *
 * Procedure :
 *     start
 *     transmit address+W (control+0)     -> ACK (from MPL3115A2)
 *     transmit data      (address)       -> ACK (from MPL3115A2)
 *     transmit data      (data)          -> ACK (from MPL3115A2)
 *     stop
 */
void Write_MPL3115A2(uint8_t MemAddress, uint8_t MemByte)
{
    uint8_t Address;
    Address = MemAddress;

    EUSCI_B3->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B3->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    while (!TransmitFlag);                        // Wait for  address to transmit
    TransmitFlag = 0;

    EUSCI_B3 -> TXBUF = Address;                  // Send the high byte of the memory address

    while (!TransmitFlag);                        // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B3 -> TXBUF = MemByte;                  // Send the byte to write to MPL3115A2

    while (!TransmitFlag);                        // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B3 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;     // I2C stop condition

    return;
}

/*
 * Function that reads a single byte from the MPL3115A2.
 * INPUTS       uint8_t MemAddress = 1 byte MPL3115A2 address to read from
 * RETURN       uint8_t ReceiveByte = 1 byte value that is received from the MPL3115A2
 *
 * Procedure :
 *     start
 *     transmit address+W (control+0)     -> ACK (from MPL3115A2)
 *     transmit data      (address)       -> ACK (from MPL3115A2)
 *     start
 *     transmit address+R (control+1)     -> ACK (from MPL3115A2)
 *     transmit data      (data)          -> ACK (from MPL3115A2)
 *     stop
 */
uint8_t Read_MPL3115A2(uint8_t MemAddress)
{
    uint8_t ReceiveByte;
    uint8_t Address = MemAddress;

    EUSCI_B3->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B3->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    while (!TransmitFlag);                        // Wait for address to transmit
    TransmitFlag = 0;

    EUSCI_B3 -> TXBUF = Address;                  // Send the high byte of the memory address

    while (!TransmitFlag);                        // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B3->CTLW0 &= ~EUSCI_B_CTLW0_TR;         // Set receive mode (read)
    EUSCI_B3->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition (restart)

    // Wait for start to be transmitted
    while ((EUSCI_B3->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // set stop bit to trigger after first byte
    EUSCI_B3->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while (!TransmitFlag);                        // Wait to receive a byte
    TransmitFlag = 0;

    ReceiveByte = EUSCI_B3->RXBUF;                // Read byte from the buffer

    return ReceiveByte;
}

/*
 * I2C Interrupt Service Routine, sets a transmit flag when transmit or receive is complete
 */
void EUSCIB3_IRQHandler(void)
{
    if (EUSCI_B3->IFG & EUSCI_B_IFG_TXIFG0)     // Check if transmit complete
    {
        EUSCI_B3->IFG &= ~ EUSCI_B_IFG_TXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }

    if (EUSCI_B3->IFG & EUSCI_B_IFG_RXIFG0)     // Check if receive complete
    {
        EUSCI_B3->IFG &= ~ EUSCI_B_IFG_RXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }
}
