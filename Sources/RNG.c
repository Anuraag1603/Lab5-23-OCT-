/*! @file RNG.c
 *
 * /*!
**  @addtogroup RNG_module RNG module documentation
**  @{
*/
/*!
 *  @brief Random Number Generator Accelerator (RNGA) on the TWR-K70F120M.
 *  @date Created: 19 OCT 2015, Last edit: 19 OCT 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE RNG
 */
#include "RNG.h"
#include "Cpu.h"
#include "types.h"
#include "MK70F12.h"

uint32_t RANDOM;

BOOL RNG_Init()
{
  SIM_SCGC3 |= SIM_SCGC3_RNGA_MASK;		//! Enable RNG
  RNG_CR |= RNG_CR_INTM_MASK;			//! Masks the triggering of an error interrupt
  RNG_CR |= RNG_CR_HA_MASK;			//! Enables notification of security violations
  RNG_CR |= RNG_CR_GO_MASK;			//! Enables random-data generation
  return bTRUE;
}

uint32_t RNG_Get()
{
 if (RNG_Poll())
    {
     RANDOM = RNG_OR;
     return RANDOM;
    }
 return NULL;
}

BOOL RNG_Poll()
{
  if (RNG_SR & RNG_SR_OREG_LVL_MASK)
    return bTRUE;
  return bFALSE;
}
  /*! END RNG.c */
  /*! END  */
  /*!
  ** @}
  */
