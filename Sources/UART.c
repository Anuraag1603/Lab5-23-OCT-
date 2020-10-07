/*! @file UART.c
 *
 * /*!
**  @addtogroup UART_module UART module documentation
**  @{
*/
/*!
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE UART
 */
#include "UART.h"
#include "FIFO.h"
#include "Cpu.h"

TFIFO TxFIFO;					//! Create Transmit FIFO
TFIFO RxFIFO;					//! Create Receive FIFO

BOOL UART_Init(const uint32_t baudRate, const uint32_t moduleClk)
{
  SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;		//! Enable UART2
  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;		//! Enable PortE

  PORTE_PCR16 = PORT_PCR_MUX(3);		//! Set PortE, Pin 16 to transmit
  PORTE_PCR17 = PORT_PCR_MUX(3);		//! Set PortE, Pin 17 to receive


/*! set up the baud rate generator for receiving and transmitting data*/
  uint32_t remain = (moduleClk / 16) % baudRate;
  uint32_t SBR = ((moduleClk / 16) - remain)/ baudRate;
  uint32_t BRFA = (32 * remain) / baudRate;

  uint16union_t cache;
  cache.l = SBR;

  UART2_BDH = cache.s.Hi;
  UART2_BDL = cache.s.Lo;			//! Set the system clock

  UART2_C1 = 0;
  UART2_C2 = UART_C2_RE_MASK | UART_C2_TE_MASK;  //! Enabling the appropriate UART2 control registers for read and write operations
  UART2_C4 = UART_C4_BRFA(BRFA);

  UART2_C2 |= UART_C2_RIE_MASK;
  UART2_C2 &= ~UART_C2_TIE_MASK;
  NVICICPR1 =  (1 << 17);
  NVICISER1 =  (1 << 17);

  FIFO_Init(&TxFIFO);				//!Initialize transmit and receive buffers
  FIFO_Init(&RxFIFO);

  return bTRUE;
}

BOOL UART_InChar(uint8_t * const dataPtr)
{
  if (dataPtr)				//! check for memory availability
  {
    return FIFO_Get(&RxFIFO, dataPtr);	//! get a byte from the receive FIFO if not empty and store at memory location of dataPtr
  }
  return bFALSE;
}

BOOL UART_OutChar(const uint8_t data)
{
  if(FIFO_Put(&TxFIFO, data))		//! put a data byte to the transmit FIFO if not full
  {
    UART2_C2 |= UART_C2_TIE_MASK;
  }
  return bTRUE;
}

void UART_Poll(void)
{
  uint8_t temp = 0;
  if (UART2_S1 & UART_S1_RDRF_MASK)		/*! check receive data register full (RDRF) flag */
  {
    temp = UART2_D; 				/*! read one byte from data register low */
    if (!FIFO_Put(&RxFIFO, temp))
    { 						/*! put it to receive buffer for later use */
    }
  }
  if (UART2_S1 & UART_S1_TDRE_MASK)		/*! check transmit data register empty (TDRE) flag */
  {
    if (FIFO_Get(&TxFIFO,  &temp))
    {   					/*! try to get one byte from transmit buffer */
      UART2_D = temp; 				/*! set it to data register for hardware transmission process */
    }
  }

}

void __attribute__ ((interrupt)) UART_ISR(void)
{
  if (UART2_S1 & UART_S1_RDRF_MASK)		/*! check receive data register full (RDRF) flag */
  {   				/*! read one byte from data register low */
    FIFO_Put(&RxFIFO, UART2_D);
    //!UART2_S1 |= ~UART_S1_RDRF_MASK;
  }

  if (UART2_S1 & UART_S1_TDRE_MASK)		/*! check transmit data register empty (TDRE) flag */
  {
    if (!(FIFO_Get(&TxFIFO,  (uint8_t*)&UART2_D)))
    {   					/*! try to get one byte from transmit buffer */
      UART2_C2 &= ~UART_C2_TIE_MASK;  				/*! set it to data register for hardware transmission process */
    }
      //!UART2_S1 |= ~UART_S1_TDRE_MASK;
  }

}

/*! END UART.c */
/*! END  */
/*!
** @}
*/

