/*! @file
 *
 *  @brief I/O routines for the K70 I2C interface.
 *
 *  This contains the functions for operating the I2C (inter-integrated circuit) module.
 *
 *  @author PMcL
 *  @date 2015-09-17
 */

// new types
#include "I2C.h"
#include "stdlib.h"

#define read 0x01;
#define write 0x00;

uint8_t ThisPrimarySlaveAddress;
const int scl[64] = {20,22,24,26,28,30,34,40,28,32,36,40,44,48,56,68,48,56,64,72,80,88,104,128,80,96,112,128,144,160,192,240,160,192,224,256,288,320,
    384,480,320,384,448,512,576,640,768,960,640,768,896,1024,1152,1280,1536,1920,1280,1536,1792,2048,2304,2560,3072,3840};
const int Mul[3] = {1,2,4};
uint8_t mul,icr;
int error,baudRate;

void I2C_waitForChannel();
void I2C_waitForAck();
void I2C_start();
void I2C_stop();
void I2C_reStart();
void I2C_MasterAck(const BOOL required);
void pause1();

BOOL I2C_Init(const TI2CModule* const aI2CModule, const uint32_t moduleClk)
{
  SIM_SCGC4 |= SIM_SCGC4_IIC0_MASK;	//! Turn on clock to I2C0 module
  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;	//! Turn on clock to PortE module
  PORTE_PCR18 = PORT_PCR_MUX(4);	//I2C0 SDA line
  PORTE_PCR19 = PORT_PCR_MUX(4);	//I2C0 SCL line

  for(int i=0;i<=63;i++)
  {
    for(int j=0; j<=2; j++)
    {
      error = aI2CModule->baudRate - moduleClk/(Mul[j]*scl[i]);
      if(abs(error) < abs(aI2CModule->baudRate-baudRate))
      {
        mul=j;
        icr = i;
        baudRate=moduleClk/(Mul[j]*scl[i]);
      }
    }
  }
  I2C0_F |= mul<<6;
  I2C0_F |= icr;
  I2C0_C1 |= I2C_C1_IICEN_MASK ;

  I2C_SelectSlaveDevice (aI2CModule->primarySlaveAddress);



  return bTRUE;
}
/*! @brief Selects the current slave device
 *
 * @param slaveAddress The slave device address.
 *
 */
void I2C_SelectSlaveDevice(const uint8_t slaveAddress)
{
  ThisPrimarySlaveAddress = slaveAddress;
}
/*! @brief Write a byte of data to a specified register
 *
 * @param registerAddress The register address.
 * @param data The 8-bit data to write.
 *
 */
void I2C_Write(const uint8_t registerAddress, const uint8_t data)
{
  I2C_waitForChannel();
  I2C_start();
  I2C0_D = (uint8_t)(ThisPrimarySlaveAddress<<1) | (uint8_t)write;
  I2C_waitForAck();
  I2C0_D = registerAddress;
  I2C_waitForAck();
  I2C0_D = data;
  I2C_waitForAck();
  I2C_stop();
  pause1();

}
/*! @brief Reads data of a specified length starting from a specified register
 *
 * @param registerAddress The register address.
 * @param data A pointer to store the bytes that are read.
 * @param nbBytes The number of bytes to read.
 *
 */
void I2C_PollRead(const uint8_t registerAddress, uint8_t* data, const uint8_t nbBytes)
{
  I2C_waitForChannel();
  I2C_start();	//Master-transmitt mode
  I2C0_D = (uint8_t)(ThisPrimarySlaveAddress<<1) | (uint8_t)write;
  I2C_waitForAck();
  I2C0_D = registerAddress;
  I2C_waitForAck();
  I2C_reStart();
  I2C0_D = (uint8_t)(ThisPrimarySlaveAddress<<1) | (uint8_t)read;
  I2C_waitForAck();
  I2C0_C1 &= ~I2C_C1_TX_MASK;	//Master-recieve mode selected
  I2C_MasterAck(bTRUE);	//enables TXAK ack for the master TO ACK
  *data = I2C0_D;	//dummy read
  I2C_waitForAck();
  for(int i = 0; i<(nbBytes-2); i++)
  {
    *data = I2C0_D;
    data++;
    I2C_waitForAck();
  }
  I2C_MasterAck(bFALSE);
  *data = I2C0_D;	//2ND LAST BYTE
  data++;
  I2C_waitForAck();
  I2C_stop();
  *data = I2C0_D;	//LAST BYTE
  pause1();
}

void I2C_IntRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
{
  I2C0_C1 |= I2C_C1_IICIE_MASK ;
  NVICICPR0 |=  (1 << 24);			//!loads the NVIC
  NVICISER0 |=  (1 << 24);

}

void __attribute__ ((interrupt)) I2C_ISR(void)
{

}



void I2C_waitForChannel()
{
  while (I2C0_S & I2C_S_BUSY_MASK)
  {
  }
}

void I2C_waitForAck()
{
  while ((I2C0_S & I2C_S_IICIF_MASK)==0)
  {
  }
  I2C0_S |= I2C_S_IICIF_MASK;
}

void I2C_start()
{
  I2C0_C1 |= I2C_C1_MST_MASK;
  I2C0_C1 |= I2C_C1_TX_MASK;
}

void I2C_stop()
{
  I2C0_C1 &= ~I2C_C1_MST_MASK;
  I2C0_C1 &= ~I2C_C1_TX_MASK;
}

void I2C_reStart()
{
  I2C0_C1 |= I2C_C1_RSTA_MASK;
}

void I2C_MasterAck(const BOOL required)
{
  if(required)
    I2C0_C1 &= ~I2C_C1_TXAK_MASK;
  else
    I2C0_C1 |= I2C_C1_TXAK_MASK;
}

void pause1()
{
  int Delay = 0xFFFF;
  while(Delay--);
}



