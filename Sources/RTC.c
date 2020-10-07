/*! @file RTC.c
 *
 * /*!
**  @addtogroup RTC_module RTC module documentation
**  @{
*/
/*!
 *  @brief Routines for controlling the Real Time Clock (RTC) on the TWR-K70F120M.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE RTC
 */
#include "RTC.h"

static void (*GlobalFunction)(void *);
static void *GlobalArguments;


BOOL RTC_Init(void (*userFunction)(void *), void *userArguments)
{
  GlobalFunction = userFunction;	//!assigns the callback function to the global function
  GlobalArguments = userArguments;	//!assigns the callback arguments to the global arguments

  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;	//!ENABLES THE RTC
  RTC_CR |= RTC_CR_OSCE_MASK;		//!ENABLES THE 32 kHz OSCILLATOR

  RTC_CR |= RTC_CR_SC2P_MASK;		//!loads 18pF Oscillator
  RTC_CR |= RTC_CR_SC16P_MASK;

  RTC_LR &= ~RTC_LR_CRL_MASK;	//!locks the rtc control registers
  RTC_SR &= ~RTC_SR_TCE_MASK;
  RTC_TPR = 0;				//!enables the Time Prescaler Register
  RTC_TSR = 0;			   	//!enables TSR to increment once a second
  RTC_SR |= RTC_SR_TCE_MASK;		//! enables the Time Count
  RTC_IER |= RTC_IER_TSIE_MASK;		//!Enables an interrupt every second

  NVICICPR2 =  (1 << 3);		//!loads the NVIC
  NVICISER2 =  (1 << 3);
}

void RTC_Set(const uint8_t hours, const uint8_t minutes, const uint8_t seconds)
{
  uint32_t sec = (hours*3600)+(minutes*60)+(seconds) ;	//!CALCULATES THE TIME TO BE SET
  RTC_SR &= ~RTC_SR_TCE_MASK;				//!DISABLES THE THE TIME COUNTER
  RTC_TPR = 0;
  RTC_TSR = sec;					//!LOADS IN THE TIME SECONDS REGISTER
  RTC_SR &= ~RTC_SR_TOF_MASK;				//!Time overflow flag is set when the time counter is enabled
  RTC_SR &= ~RTC_SR_TIF_MASK;				//!The time invalid flag is set on software reset.
  RTC_SR |= RTC_SR_TCE_MASK;				//!Enables THE THE TIME COUNTER
}

/*! @brief Gets the value of the real time clock.
 *
 *  @param hours The address of a variable to store the real time clock hours.
 *  @param minutes The address of a variable to store the real time clock minutes.
 *  @param seconds The address of a variable to store the real time clock seconds.
 *  @note Assumes that the RTC module has been initialized.
 */
void RTC_Get(uint8_t* const hours, uint8_t* const minutes, uint8_t* const seconds)
{
  uint32_t sec = RTC_TSR;
  RTC_SR &= ~RTC_SR_TCE_MASK;	//!DISABLES THE TIME COUNTER
  //!perform calculations to convert the seconds in to hours , minutes and seconds and store them in appropriate variables to be displayed
  uint32_t remain1 = sec % 3600;
  *hours = (sec - remain1)/3600;
  uint32_t remain2 = remain1 % 60;
  *minutes = (remain1 - remain2)/60;
  *seconds = remain2;
  RTC_SR |= RTC_SR_TCE_MASK;	//!Enables THE TIME COUNTER
}

void __attribute__ ((interrupt)) RTC_ISR(void)
{
  (*GlobalFunction)(GlobalArguments);		//!CALLS THE RTC CALLBACK FUNCTION
}
/*! END  */
/*!
** @}
*/



