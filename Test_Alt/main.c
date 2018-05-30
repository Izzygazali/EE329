#include "msp.h"
#include <stdint.h>

#define MPL3115A2_ADDRESS     0x60
#define CTRL_REG1             0x26
#define BAR_IN_MSB            0x14
#define BAR_IN_LSB            0x15
#define ALT                   0X80
#define OSR_OFFSET            0x03
#define PT_DATA_CFG           0x13
#define PT_DATA_CFG_TDEFE     0x01
#define PT_DATA_CFG_PDEFE     0x02
#define PT_DATA_CFG_DREM      0x04
#define OUT_T_DELTA_MSB       0x04
#define OUT_T_DELTA_LSB       0x05
#define REGISTER_STATUS_TDR   0x02

void InitMPL3115A2(uint8_t DeviceAddress);
void WriteMPL3115A2(uint8_t MemAddress, uint8_t MemByte);
uint8_t ReadMPL3115A2(uint8_t MemAddress);

uint16_t TransmitFlag = 0;

void set_sea_pressure(float pascal){
    uint16_t bar = pascal/2;
    WriteMPL3115A2(BAR_IN_MSB, bar>>8);
    __delay_cycles(3000);
    WriteMPL3115A2(BAR_IN_LSB, bar);
    __delay_cycles(3000);
    return;
}


float get_altitude(){
    int32_t alt;

    uint16_t x = (0xB9 | 0x02);

    while(ReadMPL3115A2(CTRL_REG1) & 0x02);
    __delay_cycles(3000);

    WriteMPL3115A2(CTRL_REG1, x);
    __delay_cycles(3000);

    uint8_t sta = 0;
    while(0x00 == (sta & 0x02)){
        sta = ReadMPL3115A2(0x00);
        __delay_cycles(3000);
    }

    alt = (ReadMPL3115A2(0x01))<<24;
    alt |= (ReadMPL3115A2(0x02))<<16;
    alt |= (ReadMPL3115A2(0x03))<<8;

    float altitude = alt;
    altitude /= 65536.0;
    return altitude;
}


float get_temperature(void){
   int16_t t;
   float temp;

   uint8_t sta = 0;
   while(0x00 == (sta & 0x02)){
       sta = ReadMPL3115A2(0x00);
       __delay_cycles(3000);
   }

   t = ReadMPL3115A2(OUT_T_DELTA_MSB);
   t<<=8;
   t|= ReadMPL3115A2(OUT_T_DELTA_LSB);
   t>>=4;
   if (t & 0x800) {
     t |= 0xF000;
   }

   temp = t;
   temp /= 16.0;
   return temp;
}

int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;             // Stop watchdog timer
    __enable_irq();                                         // Enable global interrupt

    InitMPL3115A2(MPL3115A2_ADDRESS);

    uint8_t whoami = ReadMPL3115A2(0x0C);

    __delay_cycles(3000);
    WriteMPL3115A2(CTRL_REG1,0x04);
    __delay_cycles(3000);
    while(ReadMPL3115A2(CTRL_REG1) & 0x04);

    WriteMPL3115A2(CTRL_REG1,0xB8);                            //Altitude mode
    __delay_cycles(3000);

    WriteMPL3115A2(PT_DATA_CFG, 0x07);
    __delay_cycles(3000);

    WriteMPL3115A2(CTRL_REG1, 0xB9);
    __delay_cycles(3000);

    float temp = get_temperature();
    set_sea_pressure(101280);
    float altitude = get_altitude();

    while(1);
}

void InitMPL3115A2(uint8_t DeviceAddress)
{

  P1->SEL0 |= BIT6 | BIT7;                // Set I2C pins of eUSCI_B0

  // Enable eUSCIB0 interrupt in NVIC module
  NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

  // Configure USCI_B0 for I2C mode
  EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST;   // Software reset enabled
  EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST |   // Remain eUSCI in reset mode
          EUSCI_B_CTLW0_MODE_3 |            // I2C mode
          EUSCI_B_CTLW0_MST |               // Master mode
          EUSCI_B_CTLW0_SYNC |              // Sync mode
          EUSCI_B_CTLW0_SSEL__SMCLK;        // SMCLK

  EUSCI_B0->BRW = 30;                       // baudrate = SMCLK / 30 = 100kHz
  EUSCI_B0->I2CSA = DeviceAddress;          // Slave address
  EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;  // Release eUSCI from reset

  EUSCI_B0->IE |= EUSCI_A_IE_RXIE |         // Enable receive interrupt
                  EUSCI_A_IE_TXIE;
}


void WriteMPL3115A2(uint8_t MemAddress, uint8_t MemByte)
{
  uint8_t Address;
  Address = MemAddress;

  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

  while (!TransmitFlag);                // Wait for  address to transmit
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = Address;    // Send the high byte of the memory address

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = MemByte;      // Send the byte to store in EEPROM

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
}

uint8_t ReadMPL3115A2(uint8_t MemAddress)
{
  uint8_t ReceiveByte;
  uint8_t Address = MemAddress;

  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;      // Set transmit mode (write)
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;   // I2C start condition

  while (!TransmitFlag);                // Wait for EEPROM address to transmit
  TransmitFlag = 0;

  EUSCI_B0 -> TXBUF = Address;    // Send the high byte of the memory address
  //__delay_cycles(800);

  while (!TransmitFlag);            // Wait for the transmit to complete
  TransmitFlag = 0;

  EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

  // Wait for start to be transmitted
  while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

  // set stop bit to trigger after first byte
  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

  while (!TransmitFlag);            // Wait to receive a byte
  TransmitFlag = 0;

  ReceiveByte = EUSCI_B0->RXBUF;    // Read byte from the buffer

  return ReceiveByte;
}

////////////////////////////////////////////////////////////////////////////////
//
// I2C Interrupt Service Routine
//
////////////////////////////////////////////////////////////////////////////////
void EUSCIB0_IRQHandler(void)
{
    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0)     // Check if transmit complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0)     // Check if receive complete
    {
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;  // Clear interrupt flag
        TransmitFlag = 1;                       // Set global flag
    }
}


/*





 */
