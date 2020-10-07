/*! @file timer.c
 *
 * /*!
**  @addtogroup timer_module timer module documentation
**  @{
*/
/*!
 *  @brief Routines for setting up the flexible timer module (FTM) on the TWR-K70F120M.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE timer
 */
#include "FTM.h"
static void *GlobalArguments;
static void (*GlobalFunction)(void *);

const uint16_t RATE = 24414;	//rate for the MSCGFCLK FOR 1 SEC


BOOL FTM_Init(void)
{
  SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK; 	//! Enable clock gate
  FTM0_CNTIN =0;
  FTM0_MODE |= FTM_MODE_WPDIS_MASK;	//! Disable write protection
  FTM0_MODE |= FTM_MODE_FTMEN_MASK ;	//! Enable FTM
  FTM0_MOD = 0x0000FFFF;		//!SETS THE FTM TO FREE RUNNING COUNTER/
  FTM0_CNT =0;
  FTM0_SC |= FTM_SC_PS(0b100);
  FTM0_SC |= FTM_SC_CLKS(0b10);		//!fixed frequency clock

  //!Set up The NVIC
  NVICICPR1 |= 1 << 30;
  NVICISER1 |= 1 << 30;
  return bTRUE;
}

BOOL FTM_Set(const TFTMChannel* const aTimer)
{

  if(aTimer->timerFunction == TIMER_FUNCTION_INPUT_CAPTURE)
    {
      FTM0_CnSC(aTimer->channelNb) &= ~FTM_CnSC_MSA_MASK;   //!Set function for input compare
      FTM0_CnSC(aTimer->channelNb) &= ~FTM_CnSC_MSB_MASK;
    }
    else
    {
	FTM0_CnSC(aTimer->channelNb) |= FTM_CnSC_MSA_MASK;   //!Set function for output compare
	FTM0_CnSC(aTimer->channelNb) &= ~FTM_CnSC_MSB_MASK;
    }

  switch (aTimer->ioType.inputDetection)
  {
    case 0:
      FTM0_CnSC(aTimer->channelNb) &= ~FTM_CnSC_ELSA_MASK;
      FTM0_CnSC(aTimer->channelNb) &= ~FTM_CnSC_ELSB_MASK;
      break;
    case 1:
      FTM0_CnSC(aTimer->channelNb) |= FTM_CnSC_ELSA_MASK; 	//!Rising edge
      FTM0_CnSC(aTimer->channelNb) &= ~FTM_CnSC_ELSB_MASK;
      break;
    case 2:
      FTM0_CnSC(aTimer->channelNb) &= ~FTM_CnSC_ELSA_MASK;
      FTM0_CnSC(aTimer->channelNb) |= FTM_CnSC_ELSB_MASK; 	//!Falling edge
      break;
    case 3:
      FTM0_CnSC(aTimer->channelNb) = FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK;	//!Rising and falling edges
      break;
  }
  GlobalArguments = aTimer -> userArguments; 	//!assigns the callback function to the global function
  GlobalFunction = aTimer -> userFunction;	//!assigns the callback arguments to the global arguments
  return bTRUE;
}

BOOL FTM_StartTimer(const TFTMChannel* const aTimer)
{
  if (aTimer->timerFunction == TIMER_FUNCTION_OUTPUT_COMPARE) //!starts timer only for output compare mode
  {
      FTM0_CnV(aTimer->channelNb) = FTM0_CNT + aTimer->delayCount ; //!Set initial count
      FTM0_CnSC(aTimer->channelNb) |= FTM_CnSC_CHIE_MASK;	//!ENABLES THE CHANNEL INTERRUPT
      return bTRUE;
  }
}

void __attribute__ ((interrupt)) FTM0_ISR(void)
{
  FTM0_CnSC(0) &= ~FTM_CnSC_CHF_MASK;
  FTM0_CnSC(0) &= ~FTM_CnSC_CHIE_MASK;
  (*GlobalFunction)(GlobalArguments);		//!CALLS THE FTM CALLBACK FUNCTION
}
/*! END  */
/*!
** @}
*/



