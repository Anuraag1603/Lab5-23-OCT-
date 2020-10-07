/*! @file Flash.c
 *
 * /*!
**  @addtogroup Flash_module Flash module documentation
**  @{
*/
/*!
 *  @brief Routines for erasing and writing to the Flash.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE Flash
 */

#include "types.h"
#include "Flash.h"
#include "MK70F12.h"
#define BUFFER_SIZE 256
#define PHRASE_SIZE 8

//!uint8_t Buffer[BUFFER_SIZE];
//!uint32_t AlocatedAddress[8];
//!int Asize = 0;
typedef struct{
  void* beginAddress;
  void* endAddress;
  uint8_t size;
}FLASH_MEMORYBLOCK;

FLASH_MEMORYBLOCK AddressBlock[8] = {0};

/*! @brief checks if the memory is allocated
 *  @param address The address to be checked.
 *  @return BOOL - TRUE if Flash memory is allocated .
 *  @note Assumes Flash has been initialized.
 */
static BOOL Flash_CheckAllocation(uint32_t address);

/*! @brief writes any command to flash
 *  @param addr The address to be checked.
 *  @param data The 64-bit data to write
 *  @param command The the command to execute the type of write
 *  @return BOOL - TRUE if Flash was written successfully.
 *  @note Assumes Flash has been initialized.
 */
static BOOL Flash_WriteACommand(volatile uint64_t* const addr, const uint64_t data, const uint8_t command);

/*! @brief Writes a 64-bit (whole phrase) number to Flash.
 *  @param address The address of the data.
 *  @param data The 64-bit data to write
 *  @return BOOL - TRUE if Flash was written successfully, FALSE if address is not aligned to a 8-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
static BOOL Flash_WriteAPhrase(volatile uint64_t* const address, const uint64_t data);


typedef union
{
  uint64_t l;

  struct
  {
  uint32_t Seg0;
  uint32_t Seg1;
  } s32;

  struct
  {
    uint16_t Seg0;
    uint16_t Seg1;
    uint16_t Seg2;
    uint16_t Seg3;
  } s16;
  struct
  {
    uint8_t Seg0;
    uint8_t Seg1;
    uint8_t Seg2;
    uint8_t Seg3;
    uint8_t Seg4;
    uint8_t Seg5;
    uint8_t Seg6;
    uint8_t Seg7;
  } s8;
}FlashPhrase_t;


BOOL Flash_Init(void)
{
  SIM_SCGC3 |= SIM_SCGC3_NFC_MASK;  	/*! !Initialize the Flash Clock  */
  return bTRUE;
}

BOOL Flash_CheckBlock(const uint8_t index, const uint8_t size) {
  int i = 0;
  for (i = 0; i < size; ++i) {
      if (AddressBlock[index+i].size != 0) {
	  return bFALSE;
      }
  }
  return bTRUE;
}

BOOL Flash_AllocateVar(volatile void **variable, const uint8_t size) /*!Allocates space for a non-volatile variable in the Flash memory.*/
{
  if(size == 1 || size == 2 || size == 4)
  {
    for(int i = 0; i < 8; i += size)
    {
	if (Flash_CheckBlock(i, size) == bTRUE) {

	    for(int j = 0; j < size;++j)
	    {
		AddressBlock[i+j].size = size;
		AddressBlock[i+j].beginAddress = (void*)0x080000 + i;
		AddressBlock[i+j].endAddress = (void*)0x080000 + size;
	    }
	    *variable = (void *)0x080000+i;
	    return bTRUE;

	}
    }
  }
  return bFALSE;
}



static BOOL Flash_WriteACommand(volatile uint64_t* const addr, const uint64_t data, const uint8_t command)
{
  FlashPhrase_t address, dataBuffer;
  address.l = (uint32_t)addr;
  dataBuffer.l = data;

  while(!(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK))
  {
  }
  if ((FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK) | (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK))
  {
      FTFE_FSTAT = 0x30;  //!FTFE_FSTAT |= FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK;
  }

  FTFE_FCCOB3 = address.s8.Seg0;
  FTFE_FCCOB2 = address.s8.Seg1;
  FTFE_FCCOB1 = address.s8.Seg2;
  FTFE_FCCOB0 = command;
  FTFE_FCCOB7 = dataBuffer.s8.Seg0;;
  FTFE_FCCOB6 = dataBuffer.s8.Seg1;
  FTFE_FCCOB5 = dataBuffer.s8.Seg2;
  FTFE_FCCOB4 = dataBuffer.s8.Seg3;
  FTFE_FCCOBB = dataBuffer.s8.Seg4;
  FTFE_FCCOBA = dataBuffer.s8.Seg5;
  FTFE_FCCOB9 = dataBuffer.s8.Seg6;
  FTFE_FCCOB8 = dataBuffer.s8.Seg7;

  FTFE_FSTAT = FTFE_FSTAT_CCIF_MASK;  //!FTFE_FSTAT &= FTFE_FSTAT_CCIF_MASK;

  while (!(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK))
  {
  }
  return bTRUE;
}

static BOOL Flash_WriteAPhrase(volatile uint64_t* const address, const uint64_t data)
{

  if ((uint32_t)address % 8 == 0)
  {
    Flash_Erase();
    return Flash_WriteACommand(address, data, 0x07);
  }
  return bFALSE;
}

BOOL Flash_Write32(uint32_t volatile * const address, const uint32_t data)
{
  FlashPhrase_t dataBuf;
  uint32_t volatile* addr = address;
  if ((uint32_t)addr % 4 == 0)\
  {
    if ((uint32_t)addr % 8 != 0)
    {
      --addr; /*! move 4 bytes to left */
      dataBuf.l = *(uint64_t*)addr;
      dataBuf.s32.Seg1 = data;
    }
    else
    {
      dataBuf.l = *(uint64_t*)addr;
      dataBuf.s32.Seg0 = data;
    }

    return Flash_WriteAPhrase((uint64_t*)addr, dataBuf.l);
  }
  return bFALSE;
}

BOOL Flash_Write16(uint16_t volatile * const address, const uint16_t data)
{
  uint16_t volatile * addr = address;
  uint32union_t dataBuf;
  if ((uint32_t)addr % 2 == 0)
  {
    if ((uint32_t)addr % 4 != 0)
    {
      --addr; /*! move 2 bytes to left */
      dataBuf.l = *(uint32_t volatile *)addr;
      dataBuf.s.Hi = data;
    }
    else
    {
      dataBuf.l = *(uint32_t volatile *)addr;
      dataBuf.s.Lo = data;
    }
    return Flash_Write32((uint32_t volatile * const)addr, dataBuf.l);
  }
  return bFALSE;
}

BOOL Flash_Write8(uint8_t volatile * const address, const uint8_t data)
{
  uint8_t volatile * addr = address;
  uint16union_t dataBuf;
  if ((uint32_t)addr % 2 != 0)
  {
    --addr; /*! move 1 byte to left */
    dataBuf.l = *(uint16_t volatile *)addr;
    dataBuf.s.Hi = data;
  }
  else
  {
    dataBuf.l = *(uint16_t volatile *)addr;
    dataBuf.s.Lo = data;
  }
  return Flash_Write16((uint16_t volatile * const)addr, dataBuf.l);
}

BOOL Flash_Erase(void)
{
  return Flash_WriteACommand((volatile uint64_t*)FLASH_DATA_START, 0, 0x09);
}
/*! END  */
/*!
** @}
*/


