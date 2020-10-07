/*! @file FIFO.c
 *
 * /*!
**  @addtogroup FIFO_module FIFO module documentation
**  @{
*/
/*
 *  @brief Routines to implement a FIFO buffer.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE FIFO
 */
#include "FIFO.h"


void FIFO_Init(TFIFO * const FIFO)
{
  if (FIFO)
  {

    FIFO->Start = 0;		       /* Initially set FIFO to zero as its initial status is not known */
    FIFO->End   = 0;
    FIFO->NbBytes = 0;
    return;
  }
}

BOOL FIFO_Put(TFIFO * const FIFO, const uint8_t data)
{
  EnterCritical();
  if (FIFO)
  {
    if (FIFO->NbBytes < FIFO_SIZE)     /* check the FIFO is full or not*/
    {
      FIFO->Buffer[FIFO->End] = data;  //! Store data in the next available empty position in the FIFO
      FIFO->End++;
      if (FIFO->End >= FIFO_SIZE)      /* once End reaches boundary, go back to zero */
        FIFO->End = 0;
      FIFO->NbBytes++;		      //! Increment number of bytes stored
      ExitCritical(); return bTRUE;
    }
    ExitCritical();return bFALSE;
  }
  ExitCritical();return bFALSE;
}

BOOL FIFO_Get(TFIFO * const FIFO, uint8_t * const dataPtr)
{
  EnterCritical();
  if (FIFO && dataPtr)				//!-------
  {
    if (FIFO->NbBytes > 0)			//! Check if FIFO is empty or not
    {
      *dataPtr = FIFO->Buffer[FIFO->Start];	//! Retrieve a byte from FIFO and place it in the memory location pointed by dataPtr
      FIFO->Start++;				//! maintain the position of last stored data
      if (FIFO->Start >= FIFO_SIZE) 		//! Once Start reaches boundary, go back to zero
        FIFO->Start = 0;
        FIFO->NbBytes--;				//! Decrement number of bytes stored
        ExitCritical();return bTRUE;
    }
    ExitCritical();return bFALSE;
    }
  ExitCritical(); return bFALSE;
}
/*! END  */
/*!
** @}
*/
