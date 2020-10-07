/*! @file LEDS.c
 *
 * /*!
**  @addtogroup LEDS_module LEDS module documentation
**  @{
*/
/*
 *  @brief Routines to access the LEDs on the TWR-K70F120M.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE LEDS
 */

#include "LEDs.h"

BOOL LEDs_Init(void)
{
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;	//!enables the port A

  PORTA_PCR11 = PORT_PCR_MUX(1); //!pin for orange
  PORTA_PCR28 = PORT_PCR_MUX(1); //!pin for yellow
  PORTA_PCR29 = PORT_PCR_MUX(1); //!pin for green
  PORTA_PCR10 = PORT_PCR_MUX(1); //!pin for blue

  //!sets the direction for all LED pins to output
  GPIOA_PDDR |= 1<<11;
  GPIOA_PDDR |= 1<<28;
  GPIOA_PDDR |= 1<<29;
  GPIOA_PDDR |= 1<<10;

  LEDs_Off(LED_ORANGE);
  LEDs_Off(LED_YELLOW);
  LEDs_Off(LED_GREEN);
  LEDs_Off(LED_BLUE);
  return bTRUE;
}

void LEDs_On(const TLED color)
{
  switch(color)
  {
    case LED_ORANGE:
      GPIOA_PCOR |= 1<<11;
      break;
    case LED_YELLOW:
      GPIOA_PCOR |= 1<<28;
      break;
    case LED_GREEN:
      GPIOA_PCOR |= 1<<29;
      break;
    case LED_BLUE:
      GPIOA_PCOR |= 1<<10;
      break;
    default:
      break;
  }
}
void LEDs_Off(const TLED color)
{
  switch(color)
  {
    case LED_ORANGE:
      GPIOA_PSOR |= 1<<11;
      break;
    case LED_YELLOW:
      GPIOA_PSOR |= 1<<28;
      break;
    case LED_GREEN:
      GPIOA_PSOR |= 1<<29;
      break;
    case LED_BLUE:
      GPIOA_PSOR |= 1<<10;
      break;
    default:
      break;
  }
}

void LEDs_Toggle(const TLED color)
{
  switch(color)
  {
    case LED_ORANGE:
      GPIOA_PTOR |= 1<<11;
      break;
    case LED_YELLOW:
      GPIOA_PTOR |= 1<<28;
      break;
    case LED_GREEN:
      GPIOA_PTOR |= 1<<29;
      break;
    case LED_BLUE:
      GPIOA_PTOR |= 1<<10;
      break;
    default:
      break;
    }
}
/* END  */
/*!
** @}
*/

