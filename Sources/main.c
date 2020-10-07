/* ###################################################################
**     Filename    : main.c
**     Project     : Lab4
**     Processor   : MK70FN1M0VMJ12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-07-20, 13:27, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 4.0
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


// CPU module - contains low level hardware initialization routines
#include "Cpu.h"
#include "Events.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "IO_Map.h"
#include "Packet.h"
#include "Flash.h"
#include "LEDs.h"
#include "PIT.h"
#include "RTC.h"
#include "FTM.h"
#include "I2C.h"
#include "accel.h"
#include "TSI.h"

#define StartupCommand  0x04		//! Commands of the Tower serial  protocol to  be  implemented
#define SpecialCommand  0x09
#define TowerNumberCommand   0x0B
#define NumberGet 1
#define NumberSet 2
#define TowerModeCommand 0x0D
#define ModeGet 1
#define ModeSet 2
#define FlashReadCommand  0x08
#define FlashWriteCommand 0x07
#define RTCsetCommand 0x0C
#define ProtocolModeCommand 0x0A
#define AccelerometerValuesCommand 0x10
#define ModeAsync 0
#define ModeSync 1

const uint32_t BaudRate = 115200;	//! Set baudRate as given in the Lab document
uint16union_t Number;
uint16union_t Mode;
uint8_t pMode;		//protocol mode
uint16_t* NumberPtr = (uint16_t*)0x80000;
uint16_t* ModePtr = (uint16_t*)0x80002;
uint8_t minutes,seconds,hours;
TFTMChannel ThisTimer;
TI2CModule  aI2CModule;
TAccelSetup Accelerometer;
char AccelerometerValues[3];

/*! @brief sets up and initializes all the modules.
 *  @returns void
 */
void Setup(void);
/*! @brief handles all the packets
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandlePacket();
/*! @brief handles the startup packet
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandleStartUp();
/*! @brief handles the special version command packet
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandleSpecialCommand();
/*! @brief handles the tower number command packet
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandleTowerNumberCommand();
/*! @brief handles the tower mode command packet
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandleTowerModeCommand();
/*! @brief handles the Flash read command packet
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandleFlashReadCommand();
/*! @brief handles the Flash write command packet
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandleFlashWriteCommand();
/*! @brief performs the PIT Interrupt service routine
 *  @param argument A pointer to a void argument
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void PIT_ISR_CALLBACK(void *argument);
/*! @brief performs the RTC Interrupt service routine
 *  @param argument A pointer to a void argument
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void RTC_ISR_CALLBACK(void *argument);
/*! @brief handles the RTC set command packet
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void HandleRTCsetCommand();
/*! @brief performs the FTM0 Interrupt service routine
 *  @param argument A pointer to a void argument
 *  @return void
 *  @note Assumes that Tower has been set up.
 */
void FTM0_ISR_Callback(void *argument );

void ACCEL_ReadCompleteISR_Callback(void *argument);

void HandleProtocolModeCommand();


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  EnterCritical();
  Setup();
  ExitCritical();
  /* Write your code here */
  for (;;)
  {
    HandlePacket();
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

void Setup(void)
{
  LEDs_Init();		//!Initializes LEDs

  Number.l = 1284 ;	//!sets the last four digit of student number to be used as the tower number

  RTC_Init(&RTC_ISR_CALLBACK,NULL);	//!Initializes RTC
  PIT_Init(CPU_BUS_CLK_HZ,&PIT_ISR_CALLBACK,NULL);	//!Initializes PIT
  PIT_Set(500000000,1);					//!sets PIT as a counter and loads 5 ns
  FTM_Init();			//!Initialize Timer



  TSI_Init();
  test();



  ThisTimer.channelNb = 0;
  ThisTimer.delayCount = 1512;
  ThisTimer.timerFunction = TIMER_FUNCTION_OUTPUT_COMPARE;
  //ThisTimer.ioType.outputAction = TIMER_OUTPUT_HIGH;
  ThisTimer.userFunction = &FTM0_ISR_Callback;
  ThisTimer.userArguments = NULL; //&ThisTimer;
  FTM_Set(&ThisTimer);	//!sets the time

  aI2CModule.baudRate = 100000;
  aI2CModule.primarySlaveAddress = 0x1D;
  aI2CModule.readCompleteCallbackArguments = NULL;
  aI2CModule.readCompleteCallbackFunction = NULL;
  I2C_Init(&aI2CModule, CPU_BUS_CLK_HZ);
  //I2C_Write(0x01,0x02);
  //I2C_PollRead(0x0D,&dataread,1);
  Accelerometer.moduleClk = CPU_BUS_CLK_HZ;
  Accel_Init(&Accelerometer);



  if (Packet_Init(BaudRate, CPU_BUS_CLK_HZ) & Flash_Init())//!initializes the packets and UART using the BaudRate provided and the module clock
  {
      LEDs_On(LED_ORANGE);
  }
  if (*NumberPtr == 0xFFFF)
  {
    Flash_Write16(NumberPtr, Number.l);
  }
  if (*ModePtr == 0xFFFF)
  {
    Flash_Write16(ModePtr, 1);
  }
  HandleStartUp(); 	//!sends the first 4 packets on startup
}

void HandlePacket()
{
  uint8_t ack = 0;		//!initially put ack as 0
  BOOL valid = bFALSE;

  if (Packet_Get())
  {
    LEDs_On(LED_BLUE);
    FTM_StartTimer(&ThisTimer);

    ack = Packet_Command & PACKET_ACK_MASK; /*! detect ACK mask from command */
    Packet_Command &= ~PACKET_ACK_MASK;     /*! clear ACK mask from command */
    switch(Packet_Command)	//!transmit first three packets of startup command
    {
        case StartupCommand:
          HandleStartUp();
          valid = bTRUE;
          break;
        case SpecialCommand:
          HandleSpecialCommand();
          valid = bTRUE;
          break;
        case TowerNumberCommand:
          HandleTowerNumberCommand();
          valid = bTRUE;
          break;
        case TowerModeCommand:
          HandleTowerModeCommand();
          valid = bTRUE;
          break;
        case FlashReadCommand:
          HandleFlashReadCommand();
          valid = bTRUE;
        case FlashWriteCommand:
          HandleFlashWriteCommand();
          valid = bTRUE;
        case RTCsetCommand:
          HandleRTCsetCommand();
          valid = bTRUE;
        case ProtocolModeCommand:
          HandleProtocolModeCommand();
          valid = bTRUE;
        default:
          valid = bFALSE;
          break;
    }

    if (ack)			//!checking for ack request
    {
      if (valid)
      {
    	Packet_Put(Packet_Command | PACKET_ACK_MASK, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3);
      }
      else
      {
    	Packet_Put(Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3);
      }
    }

  }
}

void HandleStartUp()
{
  Number.l = *NumberPtr;
  Mode.l = *ModePtr;
  Packet_Put(StartupCommand, 0, 0, 0);
  Packet_Put(SpecialCommand, 'v', 1, 0);
  Packet_Put(TowerNumberCommand, 1, Number.s.Lo, Number.s.Hi);
  Packet_Put(TowerModeCommand, ModeGet, Mode.s.Lo, Mode.s.Hi);
  Packet_Put(ProtocolModeCommand,1,ModeAsync,0);
}
void HandleSpecialCommand()
{
  Packet_Put(SpecialCommand, 'v', 1, 0);
}

void HandleTowerNumberCommand()
{
  if (Packet_Parameter1 == NumberGet)
  {
    Number.l = *NumberPtr;
    Packet_Put(TowerNumberCommand, 1, Number.s.Lo, Number.s.Hi);
  }
  else if (Packet_Parameter1 == NumberSet)
  {
    Number.s.Lo = Packet_Parameter2;
    Number.s.Hi = Packet_Parameter3;
    Flash_Write16(NumberPtr, Number.l);
  }
}

void HandleTowerModeCommand()
{
  if (Packet_Parameter1 == ModeGet)
  {
    Mode.l = *ModePtr;
    Packet_Put(TowerModeCommand, ModeGet, Mode.s.Lo, Mode.s.Hi);
  }
  else if (Packet_Parameter1 == ModeSet)
  {
    Mode.s.Lo = Packet_Parameter2;
    Mode.s.Hi = Packet_Parameter3;
    Flash_Write16(ModePtr, Mode.l);
  }
}

void HandleFlashReadCommand()
{
  if (Packet_Parameter1 >= 0x01 && Packet_Parameter1 <= 0x08)
  {
    Packet_Put(Packet_Parameter1,0,0,0);
  }

}

void HandleFlashWriteCommand()
{
  if (Packet_Parameter1 >= 0x01 && Packet_Parameter1 <= 0x08) {
      if (Packet_Parameter1 == 0x08) {
	  Flash_Erase();
      } else {
	  Flash_Write8((uint8_t*)(0x80000 + Packet_Parameter1), Packet_Parameter3);
      }
  }
}

void HandleRTCsetCommand()
{
  RTC_IER &= ~RTC_IER_TSIE_MASK;
  RTC_Set(Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
  RTC_IER |= RTC_IER_TSIE_MASK;
}

void HandleProtocolModeCommand()
{
  TAccelMode mode ;

  if (Packet_Parameter2 == ModeAsync)
    {
      mode = ACCEL_POLL;
      Accel_SetMode(mode);
      Packet_Put(ProtocolModeCommand,1,ModeAsync,0);
      Accelerometer.readCompleteCallbackArguments = NULL;
      Accelerometer.readCompleteCallbackFunction = &ACCEL_ReadCompleteISR_Callback;
    }
    else if (Packet_Parameter2 == ModeSync)
    {
      mode = ACCEL_INT;
      Accel_SetMode(mode);
      Packet_Put(ProtocolModeCommand,1,ModeSync,0);
      Accelerometer.dataReadyCallbackArguments = NULL;
      Accelerometer.dataReadyCallbackFunction = NULL;
    }

}
void PIT_ISR_CALLBACK(void *argument)
{
  if(PIT_TFLG0 & PIT_TFLG_TIF_MASK )
  {
     PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
     LEDs_Toggle(LED_GREEN);
     Accel_ReadXYZ(AccelerometerValues); //collect accelerometer data
     Packet_Put(AccelerometerValuesCommand ,AccelerometerValues[0],AccelerometerValues[1],AccelerometerValues[2]); //Send Accelerometer data every second
  }
}

void RTC_ISR_CALLBACK(void *argument)
{
  RTC_Get(&hours,&minutes,&seconds);
  Packet_Put(RTCsetCommand,hours,minutes,seconds);
  LEDs_Toggle(LED_YELLOW);
}

void FTM0_ISR_Callback(void *argument )
{
  ThisTimer.ioType.outputAction = TIMER_OUTPUT_DISCONNECT;
  LEDs_Off(LED_BLUE);
}

void ACCEL_ReadCompleteISR_Callback(void *argument)
{
  if(PIT_TFLG0 & PIT_TFLG_TIF_MASK )
    {
       PIT_TFLG0 |= PIT_TFLG_TIF_MASK;
       Accel_ReadXYZ(AccelerometerValues); //collect accelerometer data
       Packet_Put(AccelerometerValuesCommand ,AccelerometerValues[0],AccelerometerValues[1],AccelerometerValues[2]); //Send Accelerometer data every second
    }

}

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
