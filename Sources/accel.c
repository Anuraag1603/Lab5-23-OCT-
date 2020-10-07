/*! @file
 *
 *  @brief HAL for the accelerometer.
 *  @date Created: 21 SEP 2015, Last edit: 16 Oct 2015.
 *  @author Anuraag Pardeshi-12029866 & Nandita Mehta-11671284
 *
 *  MODULE accel
 */

/*!
 *  @addtogroup <Your group here>
 *  @{
*/
#include "median.h"
// Accelerometer functions
#include "accel.h"

// Inter-Integrated Circuit
#include "I2C.h"

// K70 module registers
#include "MK70F12.h"

// Accelerometer registers
#define ADDRESS_OUT_X_MSB 0x01

#define ADDRESS_INT_SOURCE 0x0C

static union
{
  uint8_t byte;			/*!< The INT_SOURCE bits accessed as a byte. */
  struct
  {
    uint8_t SRC_DRDY   : 1;	/*!< Data ready interrupt status. */
    uint8_t               : 1;
    uint8_t SRC_FF_MT  : 1;	/*!< Freefall/motion interrupt status. */
    uint8_t SRC_PULSE  : 1;	/*!< Pulse detection interrupt status. */
    uint8_t SRC_LNDPRT : 1;	/*!< Orientation interrupt status. */
    uint8_t SRC_TRANS  : 1;	/*!< Transient interrupt status. */
    uint8_t SRC_FIFO   : 1;	/*!< FIFO interrupt status. */
    uint8_t SRC_ASLP   : 1;	/*!< Auto-SLEEP/WAKE interrupt status. */
  } bits;			/*!< The INT_SOURCE bits accessed individually. */
} INT_SOURCE_Struct;

#define INT_SOURCE     		INT_SOURCE_Struct.byte
#define INT_SOURCE_SRC_DRDY	INT_SOURCE_Struct.bits.SRC_DRDY
#define INT_SOURCE_SRC_FF_MT	CTRL_REG4_Struct.bits.SRC_FF_MT
#define INT_SOURCE_SRC_PULSE	CTRL_REG4_Struct.bits.SRC_PULSE
#define INT_SOURCE_SRC_LNDPRT	CTRL_REG4_Struct.bits.SRC_LNDPRT
#define INT_SOURCE_SRC_TRANS	CTRL_REG4_Struct.bits.SRC_TRANS
#define INT_SOURCE_SRC_FIFO	CTRL_REG4_Struct.bits.SRC_FIFO
#define INT_SOURCE_SRC_ASLP	CTRL_REG4_Struct.bits.SRC_ASLP

#define ADDRESS_CTRL_REG1 0x2A

typedef enum
{
  DATE_RATE_800_HZ,
  DATE_RATE_400_HZ,
  DATE_RATE_200_HZ,
  DATE_RATE_100_HZ,
  DATE_RATE_50_HZ,
  DATE_RATE_12_5_HZ,
  DATE_RATE_6_25_HZ,
  DATE_RATE_1_56_HZ
} TOutputDataRate;

typedef enum
{
  SLEEP_MODE_RATE_50_HZ,
  SLEEP_MODE_RATE_12_5_HZ,
  SLEEP_MODE_RATE_6_25_HZ,
  SLEEP_MODE_RATE_1_56_HZ
} TSLEEPModeRate;

static union
{
  uint8_t byte;			/*!< The CTRL_REG1 bits accessed as a byte. */
  struct
  {
    uint8_t ACTIVE    : 1;	/*!< Mode selection. */
    uint8_t F_READ    : 1;	/*!< Fast read mode. */
    uint8_t LNOISE    : 1;	/*!< Reduced noise mode. */
    uint8_t DR        : 3;	/*!< Data rate selection. */
    uint8_t ASLP_RATE : 2;	/*!< Auto-WAKE sample frequency. */
  } bits;			/*!< The CTRL_REG1 bits accessed individually. */
} CTRL_REG1_Struct;

#define CTRL_REG1     		CTRL_REG1_Struct.byte
#define CTRL_REG1_ACTIVE	CTRL_REG1_Struct.bits.ACTIVE
#define CTRL_REG1_F_READ	CTRL_REG1_Struct.bits.F_READ
#define CTRL_REG1_LNOISE	CTRL_REG1_Struct.bits.LNOISE
#define CTRL_REG1_DR		CTRL_REG1_Struct.bits.DR
#define CTRL_REG1_ASLP_RATE	CTRL_REG1_Struct.bits.ASLP_RATE

#define ADDRESS_CTRL_REG2 0x2B

#define ADDRESS_CTRL_REG3 0x2C

static union
{
  uint8_t byte;			/*!< The CTRL_REG3 bits accessed as a byte. */
  struct
  {
    uint8_t PP_OD       : 1;	/*!< Push-pull/open drain selection. */
    uint8_t IPOL        : 1;	/*!< Interrupt polarity. */
    uint8_t WAKE_FF_MT  : 1;	/*!< Freefall/motion function in SLEEP mode. */
    uint8_t WAKE_PULSE  : 1;	/*!< Pulse function in SLEEP mode. */
    uint8_t WAKE_LNDPRT : 1;	/*!< Orientation function in SLEEP mode. */
    uint8_t WAKE_TRANS  : 1;	/*!< Transient function in SLEEP mode. */
    uint8_t FIFO_GATE   : 1;	/*!< FIFO gate bypass. */
  } bits;			/*!< The CTRL_REG3 bits accessed individually. */
} CTRL_REG3_Struct;

#define CTRL_REG3     		CTRL_REG3_Struct.byte
#define CTRL_REG3_PP_OD		CTRL_REG3_Struct.bits.PP_OD
#define CTRL_REG3_IPOL		CTRL_REG3_Struct.bits.IPOL
#define CTRL_REG3_WAKE_FF_MT	CTRL_REG3_Struct.bits.WAKE_FF_MT
#define CTRL_REG3_WAKE_PULSE	CTRL_REG3_Struct.bits.WAKE_PULSE
#define CTRL_REG3_WAKE_LNDPRT	CTRL_REG3_Struct.bits.WAKE_LNDPRT
#define CTRL_REG3_WAKE_TRANS	CTRL_REG3_Struct.bits.WAKE_TRANS
#define CTRL_REG3_FIFO_GATE	CTRL_REG3_Struct.bits.FIFO_GATE

#define ADDRESS_CTRL_REG4 0x2D

static union
{
  uint8_t byte;			/*!< The CTRL_REG4 bits accessed as a byte. */
  struct
  {
    uint8_t INT_EN_DRDY   : 1;	/*!< Data ready interrupt enable. */
    uint8_t               : 1;
    uint8_t INT_EN_FF_MT  : 1;	/*!< Freefall/motion interrupt enable. */
    uint8_t INT_EN_PULSE  : 1;	/*!< Pulse detection interrupt enable. */
    uint8_t INT_EN_LNDPRT : 1;	/*!< Orientation interrupt enable. */
    uint8_t INT_EN_TRANS  : 1;	/*!< Transient interrupt enable. */
    uint8_t INT_EN_FIFO   : 1;	/*!< FIFO interrupt enable. */
    uint8_t INT_EN_ASLP   : 1;	/*!< Auto-SLEEP/WAKE interrupt enable. */
  } bits;			/*!< The CTRL_REG4 bits accessed individually. */
} CTRL_REG4_Struct;

#define CTRL_REG4     		CTRL_REG4_Struct.byte
#define CTRL_REG4_INT_EN_DRDY	CTRL_REG4_Struct.bits.INT_EN_DRDY
#define CTRL_REG4_INT_EN_FF_MT	CTRL_REG4_Struct.bits.INT_EN_FF_MT
#define CTRL_REG4_INT_EN_PULSE	CTRL_REG4_Struct.bits.INT_EN_PULSE
#define CTRL_REG4_INT_EN_LNDPRT	CTRL_REG4_Struct.bits.INT_EN_LNDPRT
#define CTRL_REG4_INT_EN_TRANS	CTRL_REG4_Struct.bits.INT_EN_TRANS
#define CTRL_REG4_INT_EN_FIFO	CTRL_REG4_Struct.bits.INT_EN_FIFO
#define CTRL_REG4_INT_EN_ASLP	CTRL_REG4_Struct.bits.INT_EN_ASLP

#define ADDRESS_CTRL_REG5 0x2E

static union
{
  uint8_t byte;			/*!< The CTRL_REG5 bits accessed as a byte. */
  struct
  {
    uint8_t INT_CFG_DRDY   : 1;	/*!< Data ready interrupt enable. */
    uint8_t                : 1;
    uint8_t INT_CFG_FF_MT  : 1;	/*!< Freefall/motion interrupt enable. */
    uint8_t INT_CFG_PULSE  : 1;	/*!< Pulse detection interrupt enable. */
    uint8_t INT_CFG_LNDPRT : 1;	/*!< Orientation interrupt enable. */
    uint8_t INT_CFG_TRANS  : 1;	/*!< Transient interrupt enable. */
    uint8_t INT_CFG_FIFO   : 1;	/*!< FIFO interrupt enable. */
    uint8_t INT_CFG_ASLP   : 1;	/*!< Auto-SLEEP/WAKE interrupt enable. */
  } bits;			/*!< The CTRL_REG5 bits accessed individually. */
} CTRL_REG5_Struct;

#define CTRL_REG5     			CTRL_REG5_Struct.byte
#define CTRL_REG5_INT_CFG_DRDY		CTRL_REG5_Struct.bits.INT_CFG_DRDY
#define CTRL_REG5_INT_CFG_FF_MT		CTRL_REG5_Struct.bits.INT_CFG_FF_MT
#define CTRL_REG5_INT_CFG_PULSE		CTRL_REG5_Struct.bits.INT_CFG_PULSE
#define CTRL_REG5_INT_CFG_LNDPRT	CTRL_REG5_Struct.bits.INT_CFG_LNDPRT
#define CTRL_REG5_INT_CFG_TRANS		CTRL_REG5_Struct.bits.INT_CFG_TRANS
#define CTRL_REG5_INT_CFG_FIFO		CTRL_REG5_Struct.bits.INT_CFG_FIFO
#define CTRL_REG5_INT_CFG_ASLP		CTRL_REG5_Struct.bits.INT_CFG_ASLP

static void *GlobalDataReadyArguments;
static void (*GlobalDataReadyFunction)(void *);
static void *GlobalReadCompleteArguments;
static void (*GlobalReadCompleteFunction)(void *);
/*! @brief Initializes the accelerometer by calling the initialization routines of the supporting software modules.
 *
 *  @param accelSetup is a pointer to an accelerometer setup structure.
 *  @return BOOL - TRUE if the accelerometer module was successfully initialized.
 */
BOOL Accel_Init(const TAccelSetup* const accelSetup)
{

  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;	// Turn on clock to Port B module
  PORTB_PCR4 |= PORT_PCR_MUX(1);	// PTB4 is configured as GPIO

  I2C_Write(ADDRESS_CTRL_REG2,0x40);	//reset module

  GlobalDataReadyFunction = accelSetup->dataReadyCallbackFunction;
  GlobalDataReadyArguments = accelSetup->dataReadyCallbackArguments;
  GlobalReadCompleteFunction= accelSetup->readCompleteCallbackFunction;
  GlobalReadCompleteArguments= accelSetup->readCompleteCallbackArguments;


}

/*! @brief Reads X, Y and Z accelerations.
 *  @param data is a an array of 3 bytes where the X, Y and Z data are stored.
 */
void Accel_ReadXYZ(uint8_t data[3])
{
  uint8_t SamplesArray_xyz[3][3];
  for(int i=0; i<3; i++)
  {
      I2C_PollRead(ADDRESS_OUT_X_MSB,SamplesArray_xyz[i],3);
  }
  data[0] = Median_Filter3 (SamplesArray_xyz[0][0],SamplesArray_xyz[1][0],SamplesArray_xyz[2][0]);
  data[1] = Median_Filter3 (SamplesArray_xyz[0][1],SamplesArray_xyz[1][1],SamplesArray_xyz[2][1]); //Find the median for the X Y and Z for the 3 samples
  data[2] = Median_Filter3 (SamplesArray_xyz[0][2],SamplesArray_xyz[1][2],SamplesArray_xyz[2][2]);
}

/*! @brief Set the mode of the accelerometer.
 *  @param mode specifies either polled or interrupt driven operation.
 */
void Accel_SetMode(const TAccelMode mode)
{
  switch (mode)
  {
    case(ACCEL_INT):
      PORTB_PCR4 |= PORT_PCR_IRQC(9);	 //!Flag and Interrupt on rising-edge.
      I2C_Write(ADDRESS_CTRL_REG1,0x3B); //! add data rate value(1.56 Hz), make the accel active ,configure to fast read that sets resolution to 8 bits
      I2C_Write(ADDRESS_CTRL_REG4,0x01); //!en_drdy bit - data ready interrupt enable
      I2C_Write(ADDRESS_CTRL_REG5,0x01);	//!cfg-drdy bit - interrupt routed to int1 pin
      I2C_Write(ADDRESS_CTRL_REG3,0x02);	//ipol bit - interrupt polarity ACTIVE high
      NVICICPR2 |=  (1 << 24);			//!loads the NVIC
      NVICISER2 |=  (1 << 24);
      break;
    case(ACCEL_POLL):
      PORTB_PCR4 |= PORT_PCR_IRQC(0);	 //!Flag and Interrupt on rising-edge.
      I2C_Write(ADDRESS_CTRL_REG1,0x03); //! make the accel active and fast(8 bit)
      I2C_Write(ADDRESS_CTRL_REG4,0x00); //!en_drdy bit - data ready interrupt disable
      I2C_Write(ADDRESS_CTRL_REG5,0x00);	//!cfg-drdy bit - interrupt routed to int1 pin
      NVICICPR2 |=  (0 << 24);			//!loads the NVIC
      NVICISER2 |=  (0 << 24);
      break;
    default:
      break;
  }
}

/*! @brief Interrupt service routine for the accelerometer.
 *
 *  The accelerometer has data ready.
 *  The user callback function will be called.
 *  @note Assumes the accelerometer has been initialized.
 */
void __attribute__ ((interrupt)) AccelDataReady_ISR(void)
{
  PORTB_PCR4 |= PORT_PCR_ISF_MASK;	// Clear the interrupt flag
  (*GlobalDataReadyFunction)(GlobalDataReadyArguments);
  (*GlobalReadCompleteFunction)(GlobalReadCompleteArguments);
}

/*!
 * @}
*/
