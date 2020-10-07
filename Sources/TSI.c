/*! @file TSI.c
 *
 * /*!
**  @addtogroup TSI_module TSI module documentation
**  @{
*/
/*!
 *  @brief Touch Sense Input (TSI) on the TWR-K70F120M.
 *  @date Created: 19 OCT 2015, Last edit: 19 OCT 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE TSI
 */
#include "TSI.h"
#include "LEDs.h"
#include "Cpu.h"
#include "types.h"
#include "MK70F12.h"


BOOL TSI_Init()
{
  SIM_SCGC5 |= SIM_SCGC5_TSI_MASK;	//! Enable TSI
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;	//! Enable PortA
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;	//! Enable PortB
  PORTA_PCR4 = PORT_PCR_MUX(0);         //! Enable ALT0 for portA pin 4
  PORTB_PCR2 = PORT_PCR_MUX(0); 	//! Enable ALT0 for portB pin 2
  PORTB_PCR3 = PORT_PCR_MUX(0); 	//! Enable ALT0 for portB pin 3
  PORTB_PCR16 = PORT_PCR_MUX(0); 	//! Enable ALT0 for portB pin 16

  //Configure the number of scans and enable the interrupt
  TSI0_GENCS |= TSI_GENCS_NSCN(10);
  TSI0_GENCS |= TSI_GENCS_TSIIE_MASK;
  TSI0_GENCS |= TSI_GENCS_PS(3);
  TSI0_GENCS |= TSI_GENCS_STM_MASK;
  TSI0_SCANC |= TSI_SCANC_EXTCHRG(3);
  TSI0_SCANC |= TSI_SCANC_REFCHRG(31);
  TSI0_SCANC |= TSI_SCANC_SMOD(0);
  TSI0_SCANC |= TSI_SCANC_AMCLKS(1);
  TSI0_SCANC |= TSI_SCANC_AMPSC(0);
  //Enable the channels desired
  TSI0_PEN   |= TSI_PEN_PEN5_MASK;
  TSI0_PEN   |= TSI_PEN_PEN7_MASK;
  TSI0_PEN   |= TSI_PEN_PEN8_MASK;
  TSI0_PEN   |= TSI_PEN_PEN9_MASK;

  //TSI0_THRESHOLD |= ;
  //TSI0_THRESHOLD |= ;
  //Enable TSI module
  TSI0_GENCS |= TSI_GENCS_TSIEN_MASK;  //Enables TSI
  return bTRUE;
}

void test()
{

  uint32union_t cache;
  uint16_t Calb7;
  uint16_t Calb8;

   cache.l = TSI0_CNTR7;

   Calb7 = cache.s.Hi;
   Calb8 = cache.s.Lo;

  while (1)
    {
      uint32union_t cache1;
        uint16_t Calb71;
        uint16_t Calb81;
         cache1.l = TSI0_CNTR7;
         Calb71 = cache.s.Hi;
         Calb81 = cache.s.Lo;
         if (Calb7  Calb71)
           LEDs_Toggle(LED_GREEN);
         if (Calb8 =~ Calb81)
           LEDs_Toggle(LED_YELLOW);
    }
}

BOOL TSI_SelfCalibration()
{

}
  /*! END TSI.c */
  /*! END  */
  /*!
  ** @}
  */
