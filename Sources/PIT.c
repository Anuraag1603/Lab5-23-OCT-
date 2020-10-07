/*! @file PIT.c
 *
 * /*!
**  @addtogroup PIT_module PIT module documentation
**  @{
*/
/*!
 *  @brief Routines for controlling Periodic Interrupt Timer (PIT) on the TWR-K70F120M.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE PIT
 */
#include "PIT.h"

static void *GlobalArguments;
static void (*GlobalFunction)(void *);
uint32_t Global_modClock;

BOOL PIT_Init(const uint32_t moduleClk, void (*userFunction)(void *), void *userArguments)
{
  Global_modClock = moduleClk;
  GlobalFunction = userFunction;
  GlobalArguments = userArguments;

  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
  PIT_MCR &= ~PIT_MCR_MDIS_MASK; 		//! enabled to allow any kind of setup to pit
  PIT_MCR |= PIT_MCR_FRZ_MASK; 			//!freeze timer in debug mode
  PIT_TFLG0 &= ~PIT_TFLG_TIF_MASK; 		//!clear any previous tif interrupts
  PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK;		//!disables the timer
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;		//!enables interrupts
  NVICICPR2 |=  (1 << 4);			//!loads the NVIC
  NVICISER2 |=  (1 << 4);
  return bTRUE;
}

void PIT_Set(const uint32_t period, const BOOL restart)
{
  if(restart)
  {
    PIT_Enable(bFALSE);
    PIT_LDVAL0 = ((period/1000)*(Global_modClock/1000000))-1;
    PIT_Enable(bTRUE);
  }
  else
  {
    PIT_LDVAL0 = ((period/1000)*(Global_modClock/1000000))-1;
    uint32_t temp = PIT_LDVAL0;
  }
}

void PIT_Enable(const BOOL enable)
{
  if(enable)
  {
      PIT_TCTRL0  |= PIT_TCTRL_TEN_MASK;	//!ENABLES THE PIT TIMER
  }
  else
  {
    PIT_TCTRL0  &= ~PIT_TCTRL_TEN_MASK;		//!Disables THE PIT TIMER
  }
}

void __attribute__ ((interrupt)) PIT_ISR(void)
{
  (*GlobalFunction)(GlobalArguments);	//! points to the callback function to cause interrupts
    PIT_TFLG0 &= ~PIT_TFLG_TIF_MASK;	//!Disables the flag
}
/*! END  */
/*!
** @}
*/



