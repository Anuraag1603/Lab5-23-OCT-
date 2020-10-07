/*! @file packet.c
 *
 * /*!
**  @addtogroup packet_module packet module documentation
**  @{
*/
/*!
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE packet
 */
#include "packet.h"
#include "UART.h"

/*! define and initialize our extern packets */
//!uint8_t Packet_Command = 0, Packet_Parameter1 = 0, Packet_Parameter2 = 0, Packet_Parameter3 = 0;

const uint8_t PACKET_ACK_MASK = 0x80;//!Declaring and Initializing the Packet acknowledgment mask
TPacket Packet;
typedef enum /*! different states representing pattern of packets being received */
{
  STATE0,
  STATE1,
  STATE2,
  STATE3,
  STATE4,
  STATE5
} PACKET_STATE;

uint8_t PacketChecksum(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3)
{
  return (command ^ parameter1 ^ parameter2 ^ parameter3); //!returns the XOR of the packets
}
BOOL Packet_Init(const uint32_t baudRate, const uint32_t busClk)
{
  return UART_Init(baudRate, busClk); 		/*! initializing the packets by calling the supporting initializing module */
}

BOOL Packet_Get(void)
{
  /*! temps for packet receive state machine */
  static PACKET_STATE tempPacketState = STATE1;
  static uint8_t tempPacketCommand = 0, tempPacketParameter1 = 0, tempPacketParameter2 = 0, tempPacketParameter3 = 0, tempPacketChecksum = 0;

  //!UART_Poll();
  switch(tempPacketState)
  {
    case STATE0:
      if (UART_InChar(&tempPacketCommand))
        tempPacketState = STATE1;
       break;

    case STATE1:
      if (UART_InChar(&tempPacketParameter1))
      {
        tempPacketState = STATE2;
      }
      break;
    case STATE2:
      if (UART_InChar(&tempPacketParameter2))
      {
        tempPacketState = STATE3;
      }
      break;
    case STATE3:
      if (UART_InChar(&tempPacketParameter3))
      {
        tempPacketState = STATE4;
      }
      break;
    case STATE4:
      if (UART_InChar(&tempPacketChecksum))
      {
        tempPacketState = STATE5;
      }
      break;
    case STATE5:
      if (tempPacketChecksum != PacketChecksum(tempPacketCommand, tempPacketParameter1, tempPacketParameter2, tempPacketParameter3))
      {
        tempPacketCommand = tempPacketParameter1;
        tempPacketParameter1 = tempPacketParameter2;
        tempPacketParameter2 = tempPacketParameter3;
        tempPacketParameter3 = tempPacketChecksum;
        tempPacketState = STATE4;
      }
      else
      {
	Packet_Command = tempPacketCommand;
        Packet_Parameter1 = tempPacketParameter1;
        Packet_Parameter2 = tempPacketParameter2;
        Packet_Parameter3 = tempPacketParameter3;
        tempPacketState = STATE0;
        return bTRUE;
      }
      break;
    default:
      break;
  }
  return bFALSE;
}

BOOL Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3)
{
  return UART_OutChar(command) &&
         UART_OutChar(parameter1) &&
         UART_OutChar(parameter2) &&
         UART_OutChar(parameter3) &&
         UART_OutChar(PacketChecksum(command, parameter1, parameter2, parameter3));
}
/*! END  */
/*!
** @}
*/
