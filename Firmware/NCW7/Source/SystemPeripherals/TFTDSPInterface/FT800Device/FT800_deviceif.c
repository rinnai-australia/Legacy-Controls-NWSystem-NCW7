//*********************************************************************************************************************************************************************************
// FT800_deviceif.c
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Release Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * Version:		N-C7  	   - v090
 * Date:		01/10/2020
 * Modifier:  	Emilio R. La Greca
 *
 * Original release.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include		<com_defn.h>
#include		<FT800_deviceif.h>
#include		<mcuaxess.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Force SPI transfer timeout - Enable for testing ONLY
//#define			FT800_DEVICEIF__ENABLE_SPI_XFER_TIMEOUT_TIME	1
// Tx buffer size
#define			FT800_DEVICEIF__TX_BUFFER_SIZE						128
// Rx buffer sie
#define			FT800_DEVICEIF__RX_BUFFER_SIZE						128

typedef enum	FT800_DEVICEIF__TRANSFER_TYPE {					// Transfer Type
																//
	FT800_DEVICEIF__TT__32_BIT = 0,								// - 32 bit
	FT800_DEVICEIF__TT__16_BIT = 1,								// - 16 bit
	FT800_DEVICEIF__TT__08_BIT = 2								// - 8 bit

} FT800_DEVICEIF__TRANSFER_TYPE;

typedef enum	FT800_DEVICEIF__DISPLAY_ENABLE_DISABLE_STATE {	// Display Enable/Disable State
																//
	FT800_DEVICEIF__DEDS__GET_GPIO_DIR = 0,						// - GPIO Direction Bit : Get
	FT800_DEVICEIF__DEDS__SET_GPIO_DIR = 1,						// - GPIO Direction Bit : Set
	FT800_DEVICEIF__DEDS__GET_GPIO_VAL = 2,						// - GPIO Value Bit : Get
	FT800_DEVICEIF__DEDS__SET_GPIO_VAL = 3,						// - GPIO Value Bit : Set

} FT800_DEVICEIF__DISPLAY_ENABLE_DISABLE_STATE;

typedef enum	FT800_DEVICEIF__TX_STATE {						// Tx State
																//
	FT800_DEVICEIF__TXS__IDLE 	    = 0,						// - Idle
	FT800_DEVICEIF__TXS__ACTV 	    = 1,						// - Occurring
	FT800_DEVICEIF__TXS__DONE_VALID = 2,						// - Done : Valid
	FT800_DEVICEIF__TXS__DONE_ERROR = 3							// - Done : Error

} FT800_DEVICEIF__TX_STATE;

typedef enum	FT800_DEVICEIF__RX_STATE {						// Rx State
																//
	FT800_DEVICEIF__RXS__IDLE       = 0,						// - Idle
	FT800_DEVICEIF__RXS__ACTV  	    = 1,						// - Occurring
	FT800_DEVICEIF__RXS__DONE_VALID = 2,						// - Done : Valid
	FT800_DEVICEIF__RXS__DONE_ERROR = 3							// - Done : Error

} FT800_DEVICEIF__RX_STATE;

typedef enum	FT800_DEVICEIF__INIT_STATE {					// Device Initialisation State
																//
	FT800_DEVICEIF__INIT__IDLE  			 	= 0,			// - Idle
	FT800_DEVICEIF__INIT__START 			 	= 1,			// - Start
	FT800_DEVICEIF__INIT__POWER_DOWN 	     	= 2,			// - Power down device
	FT800_DEVICEIF__INIT__POWER_UP   	   	 	= 3,			// - Power up device
	FT800_DEVICEIF__INIT__SET_DEVICE_CLKEXT 	= 4,			// - Se device to use ext clock
	FT800_DEVICEIF__INIT__SET_DEVICE_CLK48M 	= 5,			// - Clock @ 48MHz
	FT800_DEVICEIF__INIT__SET_DEVICE_ACTIVE 	= 6,			// - Set device as active
	FT800_DEVICEIF__INIT__GET_DEVICE_ID 	 	= 7,			// - Get ID
	FT800_DEVICEIF__INIT__ERROR		 	 		= 8				// - Error

} FT800_DEVICEIF__INIT_STATE;

#define			FT800_DEVICEIF__MAX_INIT_STATES		9
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	FT800DeviceTxDetails {											// FT800 Tx Details
																				//
	FT800_DEVICEIF__TX_STATE	State;											// - State
	FT800_DEVICEIF__Buffer		AddxBuffer;										// - Addx buffer
	FT800_DEVICEIF__Buffer		DataBuffer;										// - Data buffer
																				//
} FT800DeviceTxDetails;															//
																				//
typedef struct	FT800DeviceRxDetails {											// FT800 Rx Details
																				//
	FT800_DEVICEIF__RX_STATE	State;											// - State
	FT800_DEVICEIF__Buffer		DataBuffer;										// - Data buffer
	uint8_t						ucCmdByteCount;									// - Command byte count
																				//
} FT800DeviceRxDetails;															//
																				//
typedef struct	TFTDriverInterface {											// TFT Driver Interface
																				//
	FT800DeviceTxDetails		Tx;												// - Tx Control
	FT800DeviceRxDetails		Rx;												// - Rx Control
	volatile uint8_t			ucSPITransferTimeoutx1ms;						// - SPI timeout timer
																				//
} TFTDriverInterface;															//
																				//
static TFTDriverInterface	mTFTDriverInterface;								// Container


typedef struct	TFTDriverInitialisation {										// TFT Driver Initialisation Details
																				//
	FT800_DEVICEIF__INIT_STATE						State;						// - Device init state
																				//
	uint8_t											ucDeviceCount;				// - Number of times device has been initialised
	bool											bPassed;					// - Passed/Failed flag
																				//
	volatile uint8_t								ucDelayTimerx1ms;			// - 1ms based delay timer
	bool											bDelayTimerCheck;			// - Delay timer check flag
																				//
} TFTDriverInitialisation;														//
																				//
typedef struct	TFTDriver {														// TFT Driver Details
																				//
	TFTDriverInitialisation							Initialisation;				// - Initialisation control
																				//
	FT800_DEVICEIF__DISPLAY_ENABLE_DISABLE_STATE	DisplayEnableDisableState;	// - Display enable/disable state
	GENTIMDT__TimingCounter							FailedDelayTimingCounter;	// - For test purpose - DeviceID read failre delay, emilio 07-10-16
																				//
} TFTDriver;																	//
																				//
static TFTDriver	mTFTDriver;													// Container
//
// Data Buffers
// - WR/RD addressing
#define			FT800_DEVICEIF__TX_ADDX_BUFFER_SIZE		16
static uint8_t	maucTFTDriverTxAddxBuffer[FT800_DEVICEIF__TX_ADDX_BUFFER_SIZE];
// - WR data (local)
#define			FT800_DEVICEIF__TX_DATA_BUFFER_SIZE		16
static uint8_t	maucTFTDriverTxDataBuffer[FT800_DEVICEIF__TX_DATA_BUFFER_SIZE];
// - RD data (local)
#define			FT800_DEVICEIF__RX_DATA_BUFFER_SIZE		16
static uint8_t	maucTFTDriverRxDataBuffer[FT800_DEVICEIF__RX_DATA_BUFFER_SIZE];
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	FT800_DEVICEIF__kTFTDriverFailedDelayTiming = {	&mTFTDriver.FailedDelayTimingCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static FT800_DEVICEIF__XFER_STATUS	WrXX(FUNC_ACTION vAction, FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx, uint32_t vulData);
static FT800_DEVICEIF__XFER_STATUS	RdXX(FUNC_ACTION vAction, FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx, uint32_t* pulData);

static void							WrHostCommand_InitTx(uint8_t vucHostCommand);

static void							WrXX_InitTx(FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx, uint32_t vulData);
static void							WrN8_InitTx(uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer);

static void							RdXX_InitTx(FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx);
static void							RdN8_InitTx(uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer);
static void							RdXX_DataRx(FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t* pulData);

static FT800_DEVICEIF__XFER_STATUS	EnableDisableDisplay(FUNC_ACTION vAction, bool vbEnableIt);
static uint8_t						ucSPIXferTimeoutTime(uint32_t vulBufferLength);

static void							InitStateUpdate_START(void);
static void							InitStateUpdate_POWER_DOWN(void);
static void							InitStateUpdate_POWER_UP(void);
static void							InitStateUpdate_SET_DEVICE_CLKEXT(void);
static void							InitStateUpdate_SET_DEVICE_CLK48M(void);
static void							InitStateUpdate_SET_DEVICE_ACTIVE(void);
static void							InitStateUpdate_GET_DEVICE_ID(void);
static void							InitStateUpdate_ERROR(void);
static void							InitStateUpdate(void);
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises the FT800 device interface.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DEVICEIF__Init(void)
{
	// Driver initialisation
	mTFTDriver.Initialisation.State         = FT800_DEVICEIF__INIT__IDLE;
	mTFTDriver.Initialisation.ucDeviceCount = 0;
	//
	// Driver display enable/disable state
	mTFTDriver.DisplayEnableDisableState = FT800_DEVICEIF__DEDS__GET_GPIO_DIR;
	//
	// TX Details
	// - State
	mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
	// RX Details
	// - State
	mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
	//
	// FT800 device interface init
	MCUAXESS__FT800_InterfaceInit();

#ifdef		FT800_DEVICE_ID_READ_FAIL_TEST
	GENTIMDT__StartTimer_TB_1SEC( 20, &mTFTDriver.FailedDelayTimingCounter );
#endif

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Puts the FT800 device to sleep.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DEVICEIF__GoToSleep(void)
{
	// Go to sleep command
	FT800_DEVICEIF__WrHostCommand( FUNC_LOOP, FT800__HOST_CMD_SLEEP );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	FT800 driver update - 1ms processes.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DEVICEIF__Update01ms(void)
{
	if ( mTFTDriverInterface.ucSPITransferTimeoutx1ms != 0 )
	{
		mTFTDriverInterface.ucSPITransferTimeoutx1ms--;
	}

	if ( mTFTDriver.Initialisation.ucDelayTimerx1ms != 0 )
	{
		mTFTDriver.Initialisation.ucDelayTimerx1ms--;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Configure to begin FT800 driver initialisation.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DEVICEIF__FT800DeviceInit_Config(void)
{
	mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__START;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the FT800 driver initialisation.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DEVICEIF__FT800DeviceInit_Update(void)
{
	InitStateUpdate();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Inidicates whether or not the driver initialisation process completed and passed.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_DEVICEIF__FT800DeviceInit_bCompletedAndPassed(void)
{
	if ( mTFTDriver.Initialisation.State != FT800_DEVICEIF__INIT__IDLE )
	{
		return false;
	}
	else
	if ( !mTFTDriver.Initialisation.bPassed )
	{
		return false;
	}
	else
	{
		return true;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Inidicates whether or not the driver initialisation process completed but failed.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_DEVICEIF__FT800DeviceInit_bCompletedButFailed(void)
{
	if ( mTFTDriver.Initialisation.State != FT800_DEVICEIF__INIT__IDLE )
	{
		return false;
	}
	else
	if ( mTFTDriver.Initialisation.bPassed )
	{
		return false;
	}
	else
	{
		return true;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Does a read of the device ID to indicate if it is ready - 5 attempts at read are made.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_DEVICEIF__bDeviceIsReady(void)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	uint8_t						ucValue, ucAttemptCount;
	bool						bIsReady;

	ucAttemptCount = 0; bIsReady = false;

	do
	{
		if ( ( XferStatus = FT800_DEVICEIF__Rd08( FUNC_LOOP, FT800__REG_ID, &ucValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
		{
			bIsReady = (bool)( ucValue == 0x7C );
		}

		ucAttemptCount++;
	}
	while ( ( !bIsReady ) && ( ucAttemptCount < 5 ) );

#ifdef		FT800_DEVICE_ID_READ_FAIL_TEST		// Check for override for test only
	if (mTFTDriver.FailedDelayTimingCounter.ulValue == 0)
	{
		bIsReady = false; GENTIMDT__StartTimer_TB_1SEC(20, &mTFTDriver.FailedDelayTimingCounter);
	}
#endif

	return bIsReady;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Driver interface update after byte reception detected.  After the update indicates whether or not a byte is to be transmitted.  The value to transmit placed in the
 * 			memory location pointed by pucValue.
 * @param:	pucValue - Pointer to the byte receivd and to transmit.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_DEVICEIF__bUpdateTxAfterRx(uint8_t* pucValue)
{
	bool		bTxByte = true;
	//
	// Rx Processing
	if ( mTFTDriverInterface.Rx.State != FT800_DEVICEIF__RXS__ACTV )
	{
	}
	else
	if ( mTFTDriverInterface.Tx.DataBuffer.ulIndex >= 1 )
	{
		if ( mTFTDriverInterface.Rx.DataBuffer.ulIndex < mTFTDriverInterface.Rx.DataBuffer.ulLength )
		{
			// Update buffer with data
			mTFTDriverInterface.Rx.DataBuffer.pucData[mTFTDriverInterface.Rx.DataBuffer.ulIndex++] = *pucValue;
			// Check if all bytes expected received
			if ( mTFTDriverInterface.Rx.DataBuffer.ulIndex >= mTFTDriverInterface.Rx.DataBuffer.ulLength )
			{
				mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__DONE_VALID;
			}
		}
		else
		{
			// Buffer overrun - Flag error
			mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__DONE_ERROR;
		}
	}
	//
	// Tx Processing
	if ( mTFTDriverInterface.Tx.State == FT800_DEVICEIF__TXS__ACTV )
	{
		if ( mTFTDriverInterface.Tx.AddxBuffer.ulIndex < mTFTDriverInterface.Tx.AddxBuffer.ulLength )
		{
			*pucValue = mTFTDriverInterface.Tx.AddxBuffer.pucData[mTFTDriverInterface.Tx.AddxBuffer.ulIndex++];
		}
		else
		if ( mTFTDriverInterface.Tx.DataBuffer.ulIndex < mTFTDriverInterface.Tx.DataBuffer.ulLength )
		{
			*pucValue = mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulIndex++];
		}
		else
		{
			bTxByte = false; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__DONE_VALID;
		}
	}
	else
	if ( mTFTDriverInterface.Rx.State == FT800_DEVICEIF__RXS__ACTV )
	{
		if ( mTFTDriverInterface.Tx.AddxBuffer.ulIndex < mTFTDriverInterface.Tx.AddxBuffer.ulLength )
		{
			*pucValue = mTFTDriverInterface.Tx.AddxBuffer.pucData[mTFTDriverInterface.Tx.AddxBuffer.ulIndex++];
		}
		else
		if ( mTFTDriverInterface.Tx.DataBuffer.ulIndex < mTFTDriverInterface.Tx.DataBuffer.ulLength )
		{
			*pucValue = 0x00; mTFTDriverInterface.Tx.DataBuffer.ulIndex++;
		}
		else
		{
			bTxByte = false;
		}
	}
	else
	{
		bTxByte = false;
	}

	return bTxByte;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes a host command to the FT800.
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			vucHostCommand - Host command to write.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__WrHostCommand(FUNC_ACTION vAction, uint8_t vucHostCommand)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	do
	{
		switch ( mTFTDriverInterface.Tx.State )
		{
			case FT800_DEVICEIF__TXS__IDLE:
				WrHostCommand_InitTx( vucHostCommand );
				// No break intentional - _InitTx() will force ACTV state
			case FT800_DEVICEIF__TXS__ACTV:
#ifdef		FT800_DEVICEIF__ENABLE_SPI_XFER_TIMEOUT_TIME		// Timeout timing check placed ere so loop update not needed, NC7 - v01
				if ( mTFTDriverInterface.ucSPITransferTimeoutx1ms == 0 )
				{
					XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				}
				else
				{
					XferStatus = FT800_DEVICEIF__XS_ACTV;
				}
#else
				XferStatus = FT800_DEVICEIF__XS_ACTV;
#endif
				break;

			case FT800_DEVICEIF__TXS__DONE_VALID:
				XferStatus = FT800_DEVICEIF__XS_DONE_VALID; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;

			case FT800_DEVICEIF__TXS__DONE_ERROR:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;

			default:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;
		}
		//
		// When back at idle stop the SPI and deselect the device
		if ( mTFTDriverInterface.Tx.State == FT800_DEVICEIF__TXS__IDLE )
		{
			MCUAXESS__FT800_SPIStopTransfer();
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes a 32 bit value to the FT800 at the address specified.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to write data to.
 * 			vulData	- 32 bits of data to write.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Wr32(FUNC_ACTION vAction, uint32_t vulAddx, uint32_t vulData)
{
	return WrXX( vAction, FT800_DEVICEIF__TT__32_BIT, vulAddx, vulData );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes a 16 bit value to the FT800 at the address specified.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to write data to.
 * 			vuiData	- 16 bits of data to write.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Wr16(FUNC_ACTION vAction, uint32_t vulAddx, uint16_t vuiData)
{
	return WrXX( vAction, FT800_DEVICEIF__TT__16_BIT, vulAddx, (uint32_t)vuiData );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes a 8 bit value to the FT800 at the address specified.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to write data to.
 * 			vucData	- 8 bits of data to write.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Wr08(FUNC_ACTION vAction, uint32_t vulAddx, uint8_t vucData)
{
	return WrXX( vAction, FT800_DEVICEIF__TT__08_BIT, vulAddx, (uint32_t)vucData );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes the data bytes in the buffer to the FT800 from the address specified.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to write data to.
 * 			pBuffer	- Buffer of data to write.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__WrN8(FUNC_ACTION vAction, uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	do
	{
		switch ( mTFTDriverInterface.Tx.State )
		{
			case FT800_DEVICEIF__TXS__IDLE:
				WrN8_InitTx( vulAddx, pBuffer );
				// No break intentional - _InitTx() will force ACTV state
			case FT800_DEVICEIF__TXS__ACTV:
#ifdef		FT800_DEVICEIF__ENABLE_SPI_XFER_TIMEOUT_TIME		// Timeout timing check placed ere so loop update not needed, NC7 - v01
				if ( mTFTDriverInterface.ucSPITransferTimeoutx1ms == 0 )
				{
					XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				}
				else
				{
					XferStatus = FT800_DEVICEIF__XS_ACTV;
				}
#else
				XferStatus = FT800_DEVICEIF__XS_ACTV;
#endif
				break;

			case FT800_DEVICEIF__TXS__DONE_VALID:
				XferStatus = FT800_DEVICEIF__XS_DONE_VALID; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;

			case FT800_DEVICEIF__TXS__DONE_ERROR:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;

			default:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;
		}
		//
		// When back at idle stop the SPI and deselect the device
		if ( mTFTDriverInterface.Tx.State == FT800_DEVICEIF__TXS__IDLE )
		{
			MCUAXESS__FT800_SPIStopTransfer();
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reads a 32 bit value from the FT800 at the address specified.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to read data from.
 * 			pulData	- Pointer where 32 bits of data retrieved are to be saved.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Rd32(FUNC_ACTION vAction, uint32_t vulAddx, uint32_t* pulData)
{
	return RdXX( vAction, FT800_DEVICEIF__TT__32_BIT, vulAddx, pulData );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reads a 16 bit value from the FT800 at the address specified.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to read data from.
 * 			puiData	- Pointer where 16 bits of data retrieved are to be saved.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Rd16(FUNC_ACTION vAction, uint32_t vulAddx, uint16_t* puiData)
{
	uint32_t	ulReturnValue;
	uint16_t	uiReturnValue;

	FT800_DEVICEIF__XFER_STATUS	XferStatus = RdXX( vAction, FT800_DEVICEIF__TT__16_BIT, vulAddx, &ulReturnValue );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		uiReturnValue = (uint16_t)ulReturnValue & 0x0000FFFF; *puiData = uiReturnValue;
	}

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reads a 8 bit value from the FT800 at the address specified.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to read data from.
 * 			pucData - Pointer where 8 bits of data retrieved are to be saved.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Rd08(FUNC_ACTION vAction, uint32_t vulAddx, uint8_t* pucData)
{
	uint32_t	ulReturnValue;
	uint8_t		ucReturnValue;

	FT800_DEVICEIF__XFER_STATUS	XferStatus = RdXX( vAction, FT800_DEVICEIF__TT__08_BIT, vulAddx, &ulReturnValue );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		ucReturnValue = (uint8_t)ulReturnValue & 0x000000FF; *pucData = ucReturnValue;
	}

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reads the data bytes in the buffer from the FT800 from the address specified.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vulAddx	- Address in FT800 to read data from.
 * 			pBuffer	- Buffer where retrieved data placed.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__RdN8(FUNC_ACTION vAction, uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	do
	{
		switch ( mTFTDriverInterface.Rx.State )
		{
			case FT800_DEVICEIF__RXS__IDLE:
				RdN8_InitTx( vulAddx, pBuffer );
				// No break intentional - _InitTx() will force ACTV state
			case FT800_DEVICEIF__RXS__ACTV:
#ifdef		FT800_DEVICEIF__ENABLE_SPI_XFER_TIMEOUT_TIME		// Timeout timing check placed ere so loop update not needed, NC7 - v01
				if ( mTFTDriverInterface.ucSPITransferTimeoutx1ms == 0 )
				{
					XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				}
				else
				{
					XferStatus = FT800_DEVICEIF__XS_ACTV;
				}
#else
				XferStatus = FT800_DEVICEIF__XS_ACTV;
#endif
				break;

			case FT800_DEVICEIF__RXS__DONE_VALID:
				XferStatus = FT800_DEVICEIF__XS_DONE_VALID; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				break;

			case FT800_DEVICEIF__RXS__DONE_ERROR:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				break;

			default:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				break;
		}
		//
		// When back at idle stop the SPI and deselect the device
		if ( mTFTDriverInterface.Rx.State == FT800_DEVICEIF__RXS__IDLE )
		{
			MCUAXESS__FT800_SPIStopTransfer();
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions that allow enabling/disabling of the display via GPIO bit.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__EnableDisplay(FUNC_ACTION vAction)
{
	return EnableDisableDisplay( vAction, true );
}
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__DisableDisplay(FUNC_ACTION vAction)
{
	return EnableDisableDisplay( vAction, false );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions switch backlight off/on.
 * @param:	vAction	 - Action to take within function (Loop until complete or break out).
 * 			vucLevel - Level to switch backlight on at (1 - 128 : 0 is off).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__SwitchBacklight(FUNC_ACTION vAction, uint8_t vucLevel)
{
	return FT800_DEVICEIF__Wr08( vAction, FT800__REG_PWM_DUTY, vucLevel );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to set PCLK value.
 * @param:	vAction	 - Action to take within function (Loop until complete or break out).
 * 			vucValue - PCLK value.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__SetPCLKValue(FUNC_ACTION vAction, uint8_t vucValue)
{
	return FT800_DEVICEIF__Wr08( vAction, FT800__REG_PCLK, vucValue );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to enable/disable screen shot function.
 * @param:	vAction  - Action to take within function (Loop until complete or break out).
 * 			vbEnable - Flag indicating enable/disable.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_EnableDisable(FUNC_ACTION vAction, bool vbEnable)
{
	if ( vbEnable )
	{
		return FT800_DEVICEIF__Wr08( vAction, FT800__REG_SCREENSHOT_EN, 1 );
	}
	else
	{
		return FT800_DEVICEIF__Wr08( vAction, FT800__REG_SCREENSHOT_EN, 0 );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags to start the screen shot.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_Start(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__Wr08( vAction, FT800__REG_SCREENSHOT_START, 1 );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to start/stop read screen shot function.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vbStart	- Flag indicating start/stop read.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_ReadStartStop(FUNC_ACTION vAction, bool vbStart)
{
	if ( vbStart )
	{
		return FT800_DEVICEIF__Wr08( vAction, FT800__REG_SCREENSHOT_READ, 1 );
	}
	else
	{
		return FT800_DEVICEIF__Wr08( vAction, FT800__REG_SCREENSHOT_READ, 0 );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Defines the Y position for the screen shot capture.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vuiYpos - Y position for scan : Zero based.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_SetYPosition(FUNC_ACTION vAction, uint16_t vuiYpos)
{
	return FT800_DEVICEIF__Wr16( vAction, FT800__REG_SCREENSHOT_Y, vuiYpos );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks busy status when taking a screen shot - Two 32 bit reads to get 64-bit value.
 * @param:	vAction	 - Action to take within function (Loop until complete or break out).
 * 			pbIsBusy - Pointer to memory holding busy status.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_CheckBusyStatus(FUNC_ACTION vAction, bool* pbIsBusy)
{
	static uint8_t	sucStep = 0;

	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	uint32_t					ulBusyStatus1, ulBusyStatus2;

	do
	{
		switch ( sucStep )
		{
			case 0:
				if ( ( XferStatus = FT800_DEVICEIF__Rd32( vAction, FT800__REG_SCREENSHOT_BUSY+0, &ulBusyStatus1 ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					sucStep++;
				}
				break;

			case 1:
				if ( ( XferStatus = FT800_DEVICEIF__Rd32( vAction, FT800__REG_SCREENSHOT_BUSY+4, &ulBusyStatus2 ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					sucStep++;
				}
				break;
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( sucStep < 2 ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR )
	{
		sucStep = 0;
	}
	else
	if ( sucStep >= 2 )
	{
		if ( ( ulBusyStatus1 != 0 ) || ( ulBusyStatus2 != 0 ) )
		{
			*pbIsBusy = true;
		}
		else
		{
			*pbIsBusy = false;
		}

		sucStep = 0;
	}

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Gets RGBA values when taking a screen shot - Four 8 bit reads to get the four values.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vuiXPos	- Position along line where RGBA value to be scanned.
 * 			pRGBA	- Pointer to where the RGBA valaues are placed.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_ReadRGBAValues(FUNC_ACTION vAction, uint16_t vuiXPos, FT800_DEVICEIF__RGBA* pRGBA)
{
	static uint8_t				sucStep = 0;
	static FT800_DEVICEIF__RGBA	RGBA;

	FT800_DEVICEIF__XFER_STATUS	XferStatus = FT800_DEVICEIF__XS_DONE_VALID;

	do
	{
		switch ( sucStep )
		{
			case 0:
				if ( ( XferStatus = FT800_DEVICEIF__Rd08( vAction, FT800__RAM_SCREENSHOT + (4 * vuiXPos) + sucStep, &RGBA.ucB ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					sucStep++;
				}
				break;

			case 1:
				if ( ( XferStatus = FT800_DEVICEIF__Rd08( vAction, FT800__RAM_SCREENSHOT + (4 * vuiXPos) + sucStep, &RGBA.ucG ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					sucStep++;
				}
				break;

			case 2:
				if ( ( XferStatus = FT800_DEVICEIF__Rd08( vAction, FT800__RAM_SCREENSHOT + (4 * vuiXPos) + sucStep, &RGBA.ucR ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					sucStep++;
				}
				break;

			case 3:
				if ( ( XferStatus = FT800_DEVICEIF__Rd08( vAction, FT800__RAM_SCREENSHOT + (4 * vuiXPos) + sucStep, &RGBA.ucA ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					sucStep++;
				}
				break;
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( sucStep < 4 ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR )
	{
		sucStep = 0;
	}
	else
	if ( sucStep >= 4 )
	{
		pRGBA->ucA = RGBA.ucA;
		pRGBA->ucR = RGBA.ucR;
		pRGBA->ucG = RGBA.ucG;
		pRGBA->ucB = RGBA.ucB;

		sucStep = 0;
	}

	return XferStatus;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes a value to the FT800 at the address specified.
 * @param:	vAction	  - Action to take within function (Loop until complete or break out).
 * 			vXferType - The type of data being transferred - 32, 16 or bit.
 * 			vulAddx	  - Address in FT800 to write data to.
 * 			vulData	  - 32 bits of data to write.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static FT800_DEVICEIF__XFER_STATUS	WrXX(FUNC_ACTION vAction, FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx, uint32_t vulData)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	do
	{
		switch ( mTFTDriverInterface.Tx.State )
		{
			case FT800_DEVICEIF__TXS__IDLE:
				WrXX_InitTx( vXferType, vulAddx, vulData );
				// No break intentional - _InitTx() will force ACTV state
			case FT800_DEVICEIF__TXS__ACTV:
				XferStatus = FT800_DEVICEIF__XS_ACTV;
				break;

			case FT800_DEVICEIF__TXS__DONE_VALID:
				XferStatus = FT800_DEVICEIF__XS_DONE_VALID; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;

			case FT800_DEVICEIF__TXS__DONE_ERROR:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;

			default:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
				break;
		}
		//
		// When back at idle stop the SPI and deselect the device
		if ( mTFTDriverInterface.Tx.State == FT800_DEVICEIF__TXS__IDLE )
		{
			MCUAXESS__FT800_SPIStopTransfer();
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reads a value from the FT800 at the address specified.
 * @param:	vAction	  - Action to take within function (Loop until complete or break out).
 * 			vXferType - The type of data being transferred - 32, 16 or bit.
 * 			vulAddx	  - Address in FT800 to read data from.
 * 			pulData	  - Pointer where 32 bits of data retrieved are to be saved.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static FT800_DEVICEIF__XFER_STATUS	RdXX(FUNC_ACTION vAction, FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx, uint32_t* pulData)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	do
	{
		switch ( mTFTDriverInterface.Rx.State )
		{
			case FT800_DEVICEIF__RXS__IDLE:
				RdXX_InitTx(vXferType, vulAddx);
				// No break intentional - _InitTx() will force ACTV state
			case FT800_DEVICEIF__RXS__ACTV:
#ifdef		FT800_DEVICEIF__ENABLE_SPI_XFER_TIMEOUT_TIME		// Timeout timing check placed ere so loop update not needed, NC7 - v01
				if (mTFTDriverInterface.ucSPITransferTimeoutx1ms == 0)
				{
					XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				}
				else
				{
					XferStatus = FT800_DEVICEIF__XS_ACTV;
				}
#else
				XferStatus = FT800_DEVICEIF__XS_ACTV;
#endif
				break;

			case FT800_DEVICEIF__RXS__DONE_VALID:
				RdXX_DataRx(vXferType, pulData);
				XferStatus = FT800_DEVICEIF__XS_DONE_VALID; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				break;

			case FT800_DEVICEIF__RXS__DONE_ERROR:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				break;

			default:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
				break;
		}
		//
		// When back at idle stop the SPI and deselect the device
		if ( mTFTDriverInterface.Rx.State == FT800_DEVICEIF__RXS__IDLE )
		{
			MCUAXESS__FT800_SPIStopTransfer();
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initiates the transfer of the host command to the FT800.
 * @param:	vucHostCommand - The host command to send.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							WrHostCommand_InitTx(uint8_t vucHostCommand)
{
	// Prepare addx details
	// - Define pointer to addx buffer
	mTFTDriverInterface.Tx.AddxBuffer.pucData = &maucTFTDriverTxAddxBuffer[0];
	// - Actual addx
	mTFTDriverInterface.Tx.AddxBuffer.pucData[0] = vucHostCommand;
	mTFTDriverInterface.Tx.AddxBuffer.pucData[1] = 0x00;
	mTFTDriverInterface.Tx.AddxBuffer.pucData[2] = 0x00;
	// - Define addx buffer length
	mTFTDriverInterface.Tx.AddxBuffer.ulLength = 3;
	mTFTDriverInterface.Tx.AddxBuffer.ulIndex  = 0;
	//
	// Prepare data details
	// - No associated data
	mTFTDriverInterface.Tx.DataBuffer.ulLength = 0;
	mTFTDriverInterface.Tx.DataBuffer.ulIndex  = 0;
	//
	// Define state
	mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__ACTV;
	mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
	// Timeout timer
	mTFTDriverInterface.ucSPITransferTimeoutx1ms = ucSPIXferTimeoutTime( mTFTDriverInterface.Tx.DataBuffer.ulLength );
	//
	// Enable SPI for transfer (This action selects the device)
	MCUAXESS__FT800_SPIInitTransfer( mTFTDriverInterface.Tx.AddxBuffer.pucData[mTFTDriverInterface.Tx.AddxBuffer.ulIndex++] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initiates the transfer of data to the FT800 to perform a write.
 * @param:	vXferType - The type of data being transferred - 32, 16 or bit.
 * 			vulAddx	  - Address in FT800 to write data to.
 * 			vulData	  - Data to write.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							WrXX_InitTx(FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx, uint32_t vulData)
{
	// Prepare addx details
	// - Define pointer to addx buffer (local)
	mTFTDriverInterface.Tx.AddxBuffer.pucData = &maucTFTDriverTxAddxBuffer[0];
	// - Actual addx
	mTFTDriverInterface.Tx.AddxBuffer.pucData[0] = 0x80 | (uint8_t)( ( vulAddx & 0x00FF0000 ) >> 16 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[1] = (uint8_t)( ( vulAddx & 0x0000FF00 ) >> 8 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[2] = (uint8_t)( ( vulAddx & 0x000000FF ) >> 0 );
	// - Define addx buffer length
	mTFTDriverInterface.Tx.AddxBuffer.ulLength = 3;
	mTFTDriverInterface.Tx.AddxBuffer.ulIndex  = 0;
	//
	// Prepare data details
	// - Define pointer to data buffer (local)
	mTFTDriverInterface.Tx.DataBuffer.pucData = &maucTFTDriverTxDataBuffer[0];
	// - Reset length
	mTFTDriverInterface.Tx.DataBuffer.ulLength = 0;
	mTFTDriverInterface.Tx.DataBuffer.ulIndex  = 0;
	// - Actual data
	switch ( vXferType )
	{
		case FT800_DEVICEIF__TT__32_BIT:
			mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulLength++] = ((vulData & 0x000000FF) >> 0);
			mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulLength++] = ((vulData & 0x0000FF00) >> 8);
			mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulLength++] = ((vulData & 0x00FF0000) >> 16);
			mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulLength++] = ((vulData & 0xFF000000) >> 24);
			break;

		case FT800_DEVICEIF__TT__16_BIT:
			mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulLength++] = ((vulData & 0x000000FF) >> 0);
			mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulLength++] = ((vulData & 0x0000FF00) >> 8);
			break;

		default:
			mTFTDriverInterface.Tx.DataBuffer.pucData[mTFTDriverInterface.Tx.DataBuffer.ulLength++] = ((vulData & 0x000000FF) >> 0);
			break;
	}
	//
	// Define state
	mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__ACTV;
	mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
	// Timeout timer
	mTFTDriverInterface.ucSPITransferTimeoutx1ms = ucSPIXferTimeoutTime( mTFTDriverInterface.Tx.DataBuffer.ulLength );
	//
	// Enable SPI for transfer (This action selects the device)
	MCUAXESS__FT800_SPIInitTransfer( mTFTDriverInterface.Tx.AddxBuffer.pucData[mTFTDriverInterface.Tx.AddxBuffer.ulIndex++] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initiates the transfer of data to the FT800 to perform a write.
 * @param:	vulAddx - Address in FT800 to write data to.
 * 			pBuffer	- Pointer to buffered data to write.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							WrN8_InitTx(uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer)
{
	// Prepare addx details
	// - Define pointer to addx buffer (local)
	mTFTDriverInterface.Tx.AddxBuffer.pucData = &maucTFTDriverTxAddxBuffer[0];
	// - Actual addx
	mTFTDriverInterface.Tx.AddxBuffer.pucData[0] = 0x80 | (uint8_t)( ( vulAddx & 0x00FF0000 ) >> 16 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[1] = (uint8_t)( ( vulAddx & 0x0000FF00 ) >> 8 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[2] = (uint8_t)( ( vulAddx & 0x000000FF ) >> 0 );
	// - Define addx buffer length
	mTFTDriverInterface.Tx.AddxBuffer.ulLength = 3;
	mTFTDriverInterface.Tx.AddxBuffer.ulIndex  = 0;
	//
	// Prepare data details
	// - Define pointer to data buffer (external)
	mTFTDriverInterface.Tx.DataBuffer.pucData  = pBuffer->pucData;
	// - Reset length
	mTFTDriverInterface.Tx.DataBuffer.ulLength = pBuffer->ulLength;
	mTFTDriverInterface.Tx.DataBuffer.ulIndex  = 0;
	//
	// Define state
	mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__ACTV;
	mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__IDLE;
	// Timeout timer
	mTFTDriverInterface.ucSPITransferTimeoutx1ms = ucSPIXferTimeoutTime( pBuffer->ulLength );
	//
	// Enable SPI for transfer (This action selects the device)
	MCUAXESS__FT800_SPIInitTransfer( mTFTDriverInterface.Tx.AddxBuffer.pucData[mTFTDriverInterface.Tx.AddxBuffer.ulIndex++] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initiates the transfer of data to the FT800 to perform a read.
 * @param:	vXferType - The type of data being transferred - 32, 16 or bit.
 * 			vulAddx	  - Address in FT800 to read data from.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							RdXX_InitTx(FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t vulAddx)
{
	// Prepare addx details
	// - Define pointer to addx buffer (local)
	mTFTDriverInterface.Tx.AddxBuffer.pucData  = &maucTFTDriverTxAddxBuffer[0];
	// - Actual addx
	mTFTDriverInterface.Tx.AddxBuffer.pucData[0] = 0x00 | (uint8_t)( ( vulAddx & 0x00FF0000 ) >> 16 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[1] = (uint8_t)( ( vulAddx & 0x0000FF00 ) >> 8 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[2] = (uint8_t)( ( vulAddx & 0x000000FF ) >> 0 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[3] = 0x00;		// Need dummy byte for read!!!!!!!!!!
	// - Define buffer length for addx only
	mTFTDriverInterface.Tx.AddxBuffer.ulLength = 4;
	mTFTDriverInterface.Tx.AddxBuffer.ulIndex  = 0;
	//
	// Prepare data Tx details
	// - No data Tx
	mTFTDriverInterface.Tx.DataBuffer.ulLength = 0;
	// - Reset index
	mTFTDriverInterface.Tx.DataBuffer.ulIndex  = 0;
	//
	// Prepare data Rx details
	// - Define pointer to data buffer (local)
	mTFTDriverInterface.Rx.DataBuffer.pucData  = &maucTFTDriverRxDataBuffer[0];
	// - Reset index
	mTFTDriverInterface.Rx.DataBuffer.ulIndex  = 0;
	//
	// - Bytes to clock out
	switch ( vXferType )
	{
		case FT800_DEVICEIF__TT__32_BIT:
			// Update length of buffer to write - Just 0x00 will be written
			mTFTDriverInterface.Tx.DataBuffer.ulLength += 4;
			// Number of bytes to read back
			mTFTDriverInterface.Rx.DataBuffer.ulLength  = mTFTDriverInterface.Tx.DataBuffer.ulLength;// + 1;
			break;

		case FT800_DEVICEIF__TT__16_BIT:
			// Update length of buffer to write - Just 0x00 will be written
			mTFTDriverInterface.Tx.DataBuffer.ulLength += 2;
			// Number of bytes to read back
			mTFTDriverInterface.Rx.DataBuffer.ulLength  = mTFTDriverInterface.Tx.DataBuffer.ulLength;// + 1;
			break;

		default:
			// Update length of buffer to write - Just 0x00 will be written
			mTFTDriverInterface.Tx.DataBuffer.ulLength += 1;
			// Number of bytes to read back
			mTFTDriverInterface.Rx.DataBuffer.ulLength  = mTFTDriverInterface.Tx.DataBuffer.ulLength;// + 1;
			break;
	}
	//
	// Define state
	mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
	mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__ACTV;
	mTFTDriverInterface.Rx.ucCmdByteCount = 0;
	// Timeout timer
	mTFTDriverInterface.ucSPITransferTimeoutx1ms = ucSPIXferTimeoutTime( mTFTDriverInterface.Rx.DataBuffer.ulLength );
	//
	// Enable SPI for transfer (This action selects the device)
	MCUAXESS__FT800_SPIInitTransfer( mTFTDriverInterface.Tx.AddxBuffer.pucData[mTFTDriverInterface.Tx.AddxBuffer.ulIndex++] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initiates the transfer of data to the FT800 to perform a read.
 * @param:	vulAddx	- Address in FT800 to read data from.
 * 			pBuffer - Buffer information indicating how many bytes to read and where to put them.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							RdN8_InitTx(uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer)
{
	// Prepare addx details
	// - Define pointer to addx buffer (local)
	mTFTDriverInterface.Tx.AddxBuffer.pucData = &maucTFTDriverTxAddxBuffer[0];
	// - Actual addx
	mTFTDriverInterface.Tx.AddxBuffer.pucData[0] = 0x00 | (uint8_t)( ( vulAddx & 0x00FF0000 ) >> 16 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[1] = (uint8_t)( ( vulAddx & 0x0000FF00 ) >> 8 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[2] = (uint8_t)( ( vulAddx & 0x000000FF ) >> 0 );
	mTFTDriverInterface.Tx.AddxBuffer.pucData[3] = 0x00;		// Need dummy byte for read!!!!!!!!!!
	// - Define buffer length for addx only
	mTFTDriverInterface.Tx.AddxBuffer.ulLength = 4;
	mTFTDriverInterface.Tx.AddxBuffer.ulIndex  = 0;
	//
	// Prepare data Tx details
	// - Buffer length number of bytes to be Txed out
	mTFTDriverInterface.Tx.DataBuffer.ulLength = pBuffer->ulLength;
	// - Reset index
	mTFTDriverInterface.Tx.DataBuffer.ulIndex  = 0;
	//
	// Prepare data Rx details
	// - Define pointer to data buffer (external)
	mTFTDriverInterface.Rx.DataBuffer.pucData  = pBuffer->pucData;
	// - Buffer length number of bytes+1 to be received in
	mTFTDriverInterface.Rx.DataBuffer.ulLength = pBuffer->ulLength;// + 1;
	// - Reset index
	mTFTDriverInterface.Rx.DataBuffer.ulIndex  = 0;
	//
	// Define state
	mTFTDriverInterface.Tx.State = FT800_DEVICEIF__TXS__IDLE;
	mTFTDriverInterface.Rx.State = FT800_DEVICEIF__RXS__ACTV;
	mTFTDriverInterface.Rx.ucCmdByteCount = 0;
	// Timeout timer
	mTFTDriverInterface.ucSPITransferTimeoutx1ms = ucSPIXferTimeoutTime( pBuffer->ulLength );
	//
	// Enable SPI for transfer (This action selects the device)
	MCUAXESS__FT800_SPIInitTransfer( mTFTDriverInterface.Tx.AddxBuffer.pucData[mTFTDriverInterface.Tx.AddxBuffer.ulIndex++] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reassembles the value from data bytes received.
 * @param:	vXferType - The type of data being transferred - 32, 16 or bit.
 * 			pulData	  - Pointer where data retrieved are to be saved.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							RdXX_DataRx(FT800_DEVICEIF__TRANSFER_TYPE vXferType, uint32_t* pulData)
{
	uint32_t	ulValue;

	switch ( vXferType )
	{
		case FT800_DEVICEIF__TT__32_BIT:
			ulValue =  (uint32_t)mTFTDriverInterface.Rx.DataBuffer.pucData[0];
			ulValue |= (uint32_t)mTFTDriverInterface.Rx.DataBuffer.pucData[1] << 8;
			ulValue |= (uint32_t)mTFTDriverInterface.Rx.DataBuffer.pucData[2] << 16;
			ulValue |= (uint32_t)mTFTDriverInterface.Rx.DataBuffer.pucData[3] << 24;
			break;

		case FT800_DEVICEIF__TT__16_BIT:
			ulValue =  (uint32_t)mTFTDriverInterface.Rx.DataBuffer.pucData[0];
			ulValue |= (uint32_t)mTFTDriverInterface.Rx.DataBuffer.pucData[1] << 8;
			break;

		default:
			ulValue =  (uint32_t)mTFTDriverInterface.Rx.DataBuffer.pucData[0];
			break;
	}

	*pulData = ulValue;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function that allows enabling/disabling of the display via FT800 GPIO bit.
 * @param:	vAction	   - Action to take within function (Loop until complete or break out).
 * 			vbEnableIt - Flag indicating whether to enable/disable the display
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static FT800_DEVICEIF__XFER_STATUS	EnableDisableDisplay(FUNC_ACTION vAction, bool vbEnableIt)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	static uint8_t				sucValue;
	uint8_t						ucValueToSet;

	do
	{
		switch ( mTFTDriver.DisplayEnableDisableState )
		{
			case FT800_DEVICEIF__DEDS__GET_GPIO_DIR:	// Get direction bit
				if ( ( XferStatus = FT800_DEVICEIF__Rd08( vAction, FT800__REG_GPIO_DIR, &sucValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					mTFTDriver.DisplayEnableDisableState++;
				}
				break;

			case FT800_DEVICEIF__DEDS__SET_GPIO_DIR: // Set direction bit
				if ( ( XferStatus = FT800_DEVICEIF__Wr08( vAction, FT800__REG_GPIO_DIR, FT800__REG_GPIO__DISPLAY_ENABLE_MASK | sucValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					mTFTDriver.DisplayEnableDisableState++;
				}
				break;

			case FT800_DEVICEIF__DEDS__GET_GPIO_VAL: // Get enable bit
				if ( ( XferStatus = FT800_DEVICEIF__Rd08( vAction, FT800__REG_GPIO, &sucValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					mTFTDriver.DisplayEnableDisableState++;
				}
				break;

			case FT800_DEVICEIF__DEDS__SET_GPIO_VAL: // Set enable bit
				if ( vbEnableIt )
				{
					ucValueToSet = sucValue |  FT800__REG_GPIO__DISPLAY_ENABLE_MASK;
				}
				else
				{
					ucValueToSet = sucValue & ~FT800__REG_GPIO__DISPLAY_ENABLE_MASK;
				}

				if ( ( XferStatus = FT800_DEVICEIF__Wr08( vAction, FT800__REG_GPIO, ucValueToSet ) ) == FT800_DEVICEIF__XS_DONE_VALID )
				{
					mTFTDriver.DisplayEnableDisableState++;
				}
				break;
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( mTFTDriver.DisplayEnableDisableState <= FT800_DEVICEIF__DEDS__SET_GPIO_VAL ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR )
	{
		// Error during xfer - Reset state
		mTFTDriver.DisplayEnableDisableState = FT800_DEVICEIF__DEDS__GET_GPIO_DIR;
	}
	else
	if ( mTFTDriver.DisplayEnableDisableState > FT800_DEVICEIF__DEDS__SET_GPIO_VAL )
	{
		// Process completed - Reset state
		mTFTDriver.DisplayEnableDisableState = FT800_DEVICEIF__DEDS__GET_GPIO_DIR;
	}
	else
	{
		// Process ongoing - Override transfer status
		XferStatus = FT800_DEVICEIF__XS_ACTV;
	}

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Defines the SPI timeout time allowed for based on the number of bytes to transfer assuming a 9MHz SPI clock frequency.  From measurement a transfer of 4 bytes takes
 * 			approx 9.4us so 512 bytes takes approx 1.2ms.  The timeout time is calculated as T(ms) = 5 + ( ( (N - 512) / 512 ) * 2) when N > 512 bytes, or just 5ms when N < 512
 * 			bytes.
 * @param:	vulBufferLength	- Number of bytes being transferred.
 * @retval:	Timeout time in ms.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static uint8_t						ucSPIXferTimeoutTime(uint32_t vulBufferLength)
{
	if (vulBufferLength < 512)
	{
		return 5;
	}
	else
	{
		return 5 + ( ( (vulBufferLength - 512) / 512 ) * 2);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//																		TFT FT800 DRIVER INIT STATE FUNCTIONS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - START
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_START(void)
{
	// Start Initialization
	// - Deselect device
	MCUAXESS__FT800_CS( IO_PIN_SET );
	MCUAXESS__FT800_PD( IO_PIN_SET );
	MCUAXESS__FT800_InterfaceInit();		// Reset interface, emilio 23-06-17
	// - Delay for 5ms
	mTFTDriver.Initialisation.ucDelayTimerx1ms = 5;
	mTFTDriver.Initialisation.bDelayTimerCheck = true;
	// - Update init attempt counter
	mTFTDriver.Initialisation.ucDeviceCount++;
	// - Next state after delay timer expiry
	mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__POWER_DOWN;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - POWER_DOWN
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_POWER_DOWN(void)
{
	// Full restart - PD pin lo
	MCUAXESS__FT800_PD( IO_PIN_RST );
	// Pulse lo for 5ms
	mTFTDriver.Initialisation.ucDelayTimerx1ms = 5;
	mTFTDriver.Initialisation.bDelayTimerCheck = true;
	// Next state after delay timer expiry
	mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__POWER_UP;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - POWER_DOWN
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_POWER_UP(void)
{
	// Allow power up to proceed
	// - Force power up
	MCUAXESS__FT800_PD( IO_PIN_SET );
	// - Delay for 25ms
	mTFTDriver.Initialisation.ucDelayTimerx1ms = 50;
	mTFTDriver.Initialisation.bDelayTimerCheck = true;
	// - Next state after delay timer expiry
	mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__SET_DEVICE_CLKEXT;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - SET_DEVICE_CLKEXT
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_SET_DEVICE_CLKEXT(void)
{
	// Set to use external clock
	if ( FT800_DEVICEIF__WrHostCommand( FUNC_LOOP, FT800__HOST_CMD_CLKEXT ) != FT800_DEVICEIF__XS_DONE_VALID )
	{
		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__ERROR;
	}
	else
	{
		mTFTDriver.Initialisation.ucDelayTimerx1ms = 5;
		mTFTDriver.Initialisation.bDelayTimerCheck = true;								// Delay for 5ms

		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__SET_DEVICE_CLK48M;		// Next state after delay timer expiry
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - SET_DEVICE_CLK48M
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_SET_DEVICE_CLK48M(void)
{
	// Set to use external clock 48MHz
	if ( FT800_DEVICEIF__WrHostCommand( FUNC_LOOP, FT800__HOST_CMD_CLK48M ) != FT800_DEVICEIF__XS_DONE_VALID )
	{
		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__ERROR;
	}
	else
	{
		mTFTDriver.Initialisation.ucDelayTimerx1ms = 5;
		mTFTDriver.Initialisation.bDelayTimerCheck = true;								// Delay for 5ms

		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__SET_DEVICE_ACTIVE;		// Next state after delay timer expiry
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - SET_DEVICE_ACTIVE
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_SET_DEVICE_ACTIVE(void)
{
	// Set the device to its active state
	// - Make active again
	if ( FT800_DEVICEIF__WrHostCommand( FUNC_LOOP, FT800__HOST_CMD_ACTIVE ) != FT800_DEVICEIF__XS_DONE_VALID )
	{
		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__ERROR;
	}
	else
	{
		mTFTDriver.Initialisation.ucDelayTimerx1ms = 25;
		mTFTDriver.Initialisation.bDelayTimerCheck = true;

		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__GET_DEVICE_ID;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - SET_DEVICE_ACTIVE
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_GET_DEVICE_ID(void)
{
	uint8_t		ucValue;
	//
	// Read ID - Device ready if 0x7C (if not error)
	// - Issue read register command
	if ( FT800_DEVICEIF__Rd08( FUNC_LOOP, FT800__REG_ID, &ucValue ) == FT800_DEVICEIF__XS_DONE_VALID )		// Valid?
	{
		if ( ucValue != 0x7C )
		{
			// Error
			mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__ERROR;
		}
		else
		{
			// Just force PCLK and PWM_DUTY to zero
			FT800_DEVICEIF__Wr08( FUNC_LOOP,     FT800__REG_PCLK, 0 );
			FT800_DEVICEIF__Wr08( FUNC_LOOP, FT800__REG_PWM_DUTY, 0 );
			FT800_DEVICEIF__Wr08( FUNC_LOOP,  FT800__REG_CSPREAD, 1 );
			FT800_DEVICEIF__Wr08( FUNC_LOOP, FT800__REG_PWM_DUTY, 0 );
			//
			// Device ready!
			mTFTDriver.Initialisation.bPassed = true;
			mTFTDriver.Initialisation.State   = FT800_DEVICEIF__INIT__IDLE;

			MCUAXESS__FT800_OperateInFastMode();		// Go fast SPI, emilio 03-07-17
		}
	}
	else
	{
		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__ERROR;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - SET_DEVICE_ACTIVE
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate_ERROR(void)
{
	if ( mTFTDriver.Initialisation.ucDeviceCount > 5 )
	{
		mTFTDriver.Initialisation.bPassed = false;
		mTFTDriver.Initialisation.State   = FT800_DEVICEIF__INIT__IDLE;
	}
	else
	{
		// Restart the FT800 initialisation process
		mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__START;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Device init state update - SET_DEVICE_ACTIVE
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							InitStateUpdate(void)
{
	if ( ( mTFTDriver.Initialisation.bDelayTimerCheck ) && ( mTFTDriver.Initialisation.ucDelayTimerx1ms == 0 ) )
	{
		mTFTDriver.Initialisation.bDelayTimerCheck = false;
	}

	if ( mTFTDriver.Initialisation.bDelayTimerCheck )
	{
		return;
	}

	switch (mTFTDriver.Initialisation.State)
	{
		case FT800_DEVICEIF__INIT__IDLE:
			// Idle - Do nothing
			break;

		case FT800_DEVICEIF__INIT__START:				InitStateUpdate_START();				break;
		case FT800_DEVICEIF__INIT__POWER_DOWN:			InitStateUpdate_POWER_DOWN();		break;
		case FT800_DEVICEIF__INIT__POWER_UP:			InitStateUpdate_POWER_UP();			break;
		case FT800_DEVICEIF__INIT__SET_DEVICE_CLKEXT:	InitStateUpdate_SET_DEVICE_CLKEXT();	break;
		case FT800_DEVICEIF__INIT__SET_DEVICE_CLK48M:	InitStateUpdate_SET_DEVICE_CLK48M();	break;
		case FT800_DEVICEIF__INIT__SET_DEVICE_ACTIVE:	InitStateUpdate_SET_DEVICE_ACTIVE();	break;
		case FT800_DEVICEIF__INIT__GET_DEVICE_ID:		InitStateUpdate_GET_DEVICE_ID();		break;
		case FT800_DEVICEIF__INIT__ERROR:				InitStateUpdate_ERROR();				break;

		default:
			// Restart configuration if here...
			mTFTDriver.Initialisation.State = FT800_DEVICEIF__INIT__START;
			break;
	}

	return;
}
//*********************************************************************************************************************************************************************************
