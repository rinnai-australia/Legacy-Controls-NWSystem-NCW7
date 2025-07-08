//*********************************************************************************************************************************************************************************
// FT800_coprccmd.c
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
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			FT800_COPRCCMD__USE_COMMAND_FOR_SWAP	1
//
// Coprocessor commands
#define			FT800_COPRCCMD__CMD_DLSTART				0xFFFFFF00UL
#define			FT800_COPRCCMD__CMD_SWAP				0xFFFFFF01UL
#define			FT800_COPRCCMD__CMD_COLDSTART			0xFFFFFF32UL
#define			FT800_COPRCCMD__CMD_INTERRUPT			0xFFFFFF02UL
#define			FT800_COPRCCMD__CMD_APPEND				0xFFFFFF1EUL
#define			FT800_COPRCCMD__CMD_REGREAD				0xFFFFFF19UL
#define			FT800_COPRCCMD__CMD_MEMWRITE			0xFFFFFF1AUL
#define			FT800_COPRCCMD__CMD_INFLATE				0xFFFFFF22UL
#define			FT800_COPRCCMD__CMD_LOADIMAGE			0xFFFFFF24UL
#define			FT800_COPRCCMD__CMD_MEMCRC				0xFFFFFF18UL
#define			FT800_COPRCCMD__CMD_MEMZERO				0xFFFFFF1CUL
#define			FT800_COPRCCMD__CMD_MEMSET				0xFFFFFF1BUL
#define			FT800_COPRCCMD__CMD_MEMCPY				0xFFFFFF1DUL
#define			FT800_COPRCCMD__CMD_BUTTON				0xFFFFFF0DUL
#define			FT800_COPRCCMD__CMD_CLOCK				0xFFFFFF14UL
#define			FT800_COPRCCMD__CMD_FGCOLOR				0xFFFFFF0AUL
#define			FT800_COPRCCMD__CMD_BGCOLOR				0xFFFFFF09UL
#define			FT800_COPRCCMD__CMD_GRADCOLOR			0xFFFFFF34UL
#define			FT800_COPRCCMD__CMD_GAUGE				0xFFFFFF13UL
#define			FT800_COPRCCMD__CMD_GRADIENT			0xFFFFFF0BUL
#define			FT800_COPRCCMD__CMD_KEYS				0xFFFFFF0EUL
#define			FT800_COPRCCMD__CMD_PROGRESS			0xFFFFFF0FUL
#define			FT800_COPRCCMD__CMD_SCROLLBAR			0xFFFFFF11UL
#define			FT800_COPRCCMD__CMD_SLIDER				0xFFFFFF10UL
#define			FT800_COPRCCMD__CMD_DIAL				0xFFFFFF2DUL
#define			FT800_COPRCCMD__CMD_TOGGLE				0xFFFFFF12UL
#define			FT800_COPRCCMD__CMD_TEXT				0xFFFFFF0CUL
#define			FT800_COPRCCMD__CMD_NUMBER				0xFFFFFF2EUL
#define			FT800_COPRCCMD__CMD_LOADIDENTITY		0xFFFFFF26UL
#define			FT800_COPRCCMD__CMD_SETMATRIX			0xFFFFFF2AUL
#define			FT800_COPRCCMD__CMD_GETMATRIX			0xFFFFFF33UL
#define			FT800_COPRCCMD__CMD_GETPTR				0xFFFFFF23UL
#define			FT800_COPRCCMD__CMD_GETPROPS			0xFFFFFF25UL
#define			FT800_COPRCCMD__CMD_SCALE				0xFFFFFF28UL
#define			FT800_COPRCCMD__CMD_ROTATE				0xFFFFFF29UL
#define			FT800_COPRCCMD__CMD_TRANSLATE			0xFFFFFF27UL
#define			FT800_COPRCCMD__CMD_CALIBRATE			0xFFFFFF15UL
#define			FT800_COPRCCMD__CMD_SPINNER				0xFFFFFF16UL
#define			FT800_COPRCCMD__CMD_SCREENSAVER			0xFFFFFF2FUL
#define			FT800_COPRCCMD__CMD_SKETCH				0xFFFFFF30UL
#define			FT800_COPRCCMD__CMD_STOP				0xFFFFFF17UL
#define			FT800_COPRCCMD__CMD_SETFONT				0xFFFFFF2BUL
#define			FT800_COPRCCMD__CMD_TRACK				0xFFFFFF2CUL
#define			FT800_COPRCCMD__CMD_SNAPSHOT			0xFFFFFF1FUL

typedef enum	FT800_COPRCCMD__CMD_XFER_STATE {

	FT800_COPRCCMD__CXS__IDLE	   	= 0,
	FT800_COPRCCMD__CXS__NEXT	   	= 1,
	FT800_COPRCCMD__CXS__WAIT	   	= 2,
	FT800_COPRCCMD__CXS__ACTV	   	= 3,
	FT800_COPRCCMD__CXS__WRTE	   	= 4,
	FT800_COPRCCMD__CXS__READ	   	= 5,
	FT800_COPRCCMD__CXS__DONE_VALID	= 6,
	FT800_COPRCCMD__CXS__DONE_ERROR = 7

} FT800_COPRCCMD__CMD_XFER_STATE;

typedef enum	FT800_COPRCCMD__CMD_XFER_READ_STATE {

	FT800_COPRCCMD__CXRS__GET_WRREG = 0,
	FT800_COPRCCMD__CXRS__GET_RDREG = 1,
	FT800_COPRCCMD__CXRS__GET_VALUE = 2,
	FT800_COPRCCMD__CXRS__COMPLETED = 3

} FT800_COPRCCMD__CMD_XFER_READ_STATE;

typedef enum	FT800_COPRCCMD__ISSUE_QUEUE_STATE {

	FT800_COPRCCMD__IQS__GET_RD_REG  = 0,
	FT800_COPRCCMD__IQS__GET_WR_REG  = 1,
	FT800_COPRCCMD__IQS__FIFO_EMPTY  = 2,
	FT800_COPRCCMD__IQS__XFER_QUEUE  = 3,
	FT800_COPRCCMD__IQS__SET_WR_REG  = 4,
	FT800_COPRCCMD__IQS__COMPLETED   = 5

} FT800_COPRCCMD__ISSUE_QUEUE_STATE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	FT800CoProcessorCommand {											// Coprocessr Command Control
																					//
	FT800_COPRCCMD__BUFFER_ACCESS_TYPE			BufferAccessType;					// - Access type defined
																					//
	uint32_t									ulBytesToWrite;						// - Number of bytes to write
	uint32_t									ulBytesWritten;						// - Number of bytes that have been written
	uint8_t*									pucSrcBuffer;						// - Pointer to source buffer of command bytes
																					//
	FT800_DEVICEIF__Buffer						DataBuffer;							// - Data buffer used during transfer
																					//
} FT800CoProcessorCommand;															//
																					//
typedef struct	FT800CoProcessorReadback {											// Coprocessor Readback Control
																					//
	bool										bAction;							// - Flag indicating whether or not readback action is required
	uint32_t									ulOffset;							// - Offset into buffer where read back value kept
	uint16_t									uiWRRegisterValue;					// - Write register value where read back command set
																					//
	FT800_DEVICEIF__Buffer						DataBuffer;							// - Data buffer where read back data placed
																					//
} FT800CoProcessorReadback;															//
																					//
																					//
typedef struct	FT800CoProcessor {													// Coprocessor Control
																					//
	FT800_COPRCCMD__ISSUE_QUEUE_STATE			IssueQueueState;					// - State when 'FT800_COPRCCMD__BAT_QUEUE_XFER' access type used
																					//
	FT800_COPRCCMD__CMD_XFER_STATE				CmdXferState;						// - Command transfer state being processed for 'FT800_COPRCCMD__BAT_CHECK_BUFF' access type
	FT800_COPRCCMD__CMD_XFER_READ_STATE			CmdXferReadState;					// - Command transfer readback state
																					//
	uint16_t									uiRDRegisterValue;					// - Command transfer read register value
	uint16_t									uiWRRegisterValue;					// - Command transfer write register value
																					//
	FT800CoProcessorCommand						Command;							// - Command details
	FT800CoProcessorReadback					Readback;							// - Readback details
																					//
} FT800CoProcessor;																	//
																					//
static FT800CoProcessor	mFT800CoProcessor;											// Container

#define			FT800_COPRCCMD__UM_BUFFER_MAX_BYTES		4096						// Buffer for 'FT800_COPRCCMD__BAT_QUEUE_XFER' access type
static uint8_t	maucFT800CoProcessorUMBuffer[FT800_COPRCCMD__UM_BUFFER_MAX_BYTES];	//

#define			FT800_COPRCCMD__TX_BUFFER_MAX_BYTES		256							// Tx Update Buffer
static uint8_t	maucFT800CoProcessorTxBuffer[FT800_COPRCCMD__TX_BUFFER_MAX_BYTES];	//

#define			FT800_COPRCCMD__RX_BUFFER_MAX_BYTES		128							// Rx Update Buffer
static uint8_t	maucFT800CoProcessorRxBuffer[FT800_COPRCCMD__RX_BUFFER_MAX_BYTES];	//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static FT800_DEVICEIF__XFER_STATUS		CPCmdUpdate_Continuous(FUNC_ACTION vAction);

static FT800_DEVICEIF__XFER_STATUS		CPCmdUpdate_CheckBuffer(FUNC_ACTION vAction);
static void								CPCmdUpdate_CheckBufferWait(FUNC_ACTION vAction);
static void								CPCmdUpdate_CheckBufferActv(FUNC_ACTION vAction);
static void								CPCmdUpdate_CheckBufferWrte(FUNC_ACTION vAction);
static void								CPCmdUpdate_CheckBufferRead(FUNC_ACTION vAction);

static void								CPData_Buffer32BitUValue(uint32_t vulValue);
static void								CPData_Buffer32BitIValue(int32_t vilValue);

static void								CPData_Buffer16BitUValue(uint16_t vuiValue);
static void								CPData_Buffer16BitIValue(int16_t viValue);

static void								CPData_BufferStringValue(const char* pString);
static void								CPData_BufferNDataBytes(uint8_t* pucBuffer);

static uint16_t							uiValueRoundedToNearestModulo4(uint16_t uiValue);
static uint16_t							uiIncrementCMDBufferOffset(uint16_t vuiOffset, uint16_t vuiCmdSize);
static uint32_t							ulRGBColor(uint8_t vucR, uint8_t vucG, uint8_t vucB);

static void								CPCmd__DataBufferInit(void);
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
 * @brief:	Initialises the interface to the co-processor engine.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__Init(void)
{
	mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__IDLE;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function defines the type of command buffer access type required.
 *
 * 			1) When using the 'FT800_COPRCCMD__BAT_CONTINUOUS' access type the function:
 *
 * 							FT800_COPRCCMD__CPCmd__BufferStatus().
 *
 * 			   must be executed first until 'FT800_DEVICEIF__BCS_DONE_EMPTY' is returned.  This forces initialisation of the write register as well as ensuring that the coprocessor
 * 			   command buffer is empty ready for continuous update.
 *
 * 			2) Then update the command buffer as needed.
 * 			3) Finally, execute the following function to instruct the coprocessor to commit the change:
 *
 * 				 			FT800_COPRCCMD__CPCmd__BufferDefine().
 *
 * @param:	vAccessType	- The buffer access type.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmd__DefineAccessType(FT800_COPRCCMD__BUFFER_ACCESS_TYPE vAccessType)
{
	if ( vAccessType == FT800_COPRCCMD__BAT_QUEUE_XFER )
	{
		// Queue the transfer access type
		// - Define pointer to queue buffer
		mFT800CoProcessor.Command.DataBuffer.pucData     = &maucFT800CoProcessorUMBuffer[0];
		// - Define maximum length allowed
		mFT800CoProcessor.Command.DataBuffer.ulMaxLength = FT800_COPRCCMD__UM_BUFFER_MAX_BYTES;
		// - Reset buffer length
		mFT800CoProcessor.Command.DataBuffer.ulLength    = 0;
		// - Reset the IssueQueue state
		mFT800CoProcessor.IssueQueueState = FT800_COPRCCMD__IQS__GET_RD_REG;
	}

	mFT800CoProcessor.Command.BufferAccessType = vAccessType;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function issues the queued data to the FT800 after first waiting for the FT800 command FIFO buffer to be emptied.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmd__IssueQueue(FUNC_ACTION vAction)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;			// Transfer status result

	// NB: 'mFT800CoProcessor.IssueQueueState' initialised in FT800_COPRCCMD__CPCmd__DefineAccessType().
	do
	{
		// Get RD Register?
		if ( mFT800CoProcessor.IssueQueueState == FT800_COPRCCMD__IQS__GET_RD_REG )
		{
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_READ, &mFT800CoProcessor.uiRDRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				mFT800CoProcessor.IssueQueueState = FT800_COPRCCMD__IQS__GET_WR_REG;
			}
		}
		//
		// Get WR Register?
		if ( mFT800CoProcessor.IssueQueueState == FT800_COPRCCMD__IQS__GET_WR_REG )
		{
			if ( ( XferStatus = FT800_DEVICEIF__Rd16(vAction, FT800__REG_CMD_WRITE, &mFT800CoProcessor.uiWRRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				mFT800CoProcessor.IssueQueueState = FT800_COPRCCMD__IQS__FIFO_EMPTY;
			}
		}
		//
		// Check FIFO empty?
		if ( mFT800CoProcessor.IssueQueueState == FT800_COPRCCMD__IQS__FIFO_EMPTY )
		{
			if ( mFT800CoProcessor.uiRDRegisterValue == mFT800CoProcessor.uiWRRegisterValue )
			{
				mFT800CoProcessor.IssueQueueState = FT800_COPRCCMD__IQS__XFER_QUEUE;

			}
			else
			{
				// If not ready just get out - do not loop around
				mFT800CoProcessor.IssueQueueState = FT800_COPRCCMD__IQS__COMPLETED;
			}

			XferStatus = FT800_DEVICEIF__XS_ACTV;	// Mark Xfer status as active for possible loop around
		}
		//
		// Transfer queued data?
		if ( mFT800CoProcessor.IssueQueueState == FT800_COPRCCMD__IQS__XFER_QUEUE )
		{
			if ( ( XferStatus = FT800_DEVICEIF__WrN8( vAction, (uint32_t)FT800__RAM_CMD + (uint32_t)mFT800CoProcessor.uiWRRegisterValue, &mFT800CoProcessor.Command.DataBuffer ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				// Update the write register
				mFT800CoProcessor.uiWRRegisterValue = uiIncrementCMDBufferOffset( mFT800CoProcessor.uiWRRegisterValue, (uint16_t)mFT800CoProcessor.Command.DataBuffer.ulLength );

				mFT800CoProcessor.IssueQueueState = FT800_COPRCCMD__IQS__SET_WR_REG;
			}
		}
		//
		// Set WR rehister?
		if ( mFT800CoProcessor.IssueQueueState == FT800_COPRCCMD__IQS__SET_WR_REG )
		{
			if ( ( XferStatus = FT800_DEVICEIF__Wr16( vAction, FT800__REG_CMD_WRITE, mFT800CoProcessor.uiWRRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				mFT800CoProcessor.IssueQueueState = FT800_COPRCCMD__IQS__COMPLETED;
			}
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( mFT800CoProcessor.IssueQueueState != FT800_COPRCCMD__IQS__COMPLETED ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function retrieves the command coprocessor read/write register values to determine if the command coprocessor buffer is empty.
 * @param:	vAction      - Action to take within function (Loop until complete or break out).
 * 			pBufferEmpty - Pointer to variable indicating whether ot not the command buffer is empty.
 * @retval:	FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmd__BufferStatus(FUNC_ACTION vAction, bool *pBufferEmpty)
{
	static uint8_t				sucCheckValue = 0;	// Used to keep track of action to perform
	FT800_DEVICEIF__XFER_STATUS	XferStatus;			// Transfer status result

	*pBufferEmpty = false;

	do
	{
		if ( sucCheckValue == 0 )
		{
			// Get RD Register
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_READ, &mFT800CoProcessor.uiRDRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				sucCheckValue = 1;
			}
		}

		if ( sucCheckValue == 1 )
		{
			// Get WR Register
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_WRITE, &mFT800CoProcessor.uiWRRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				if ( mFT800CoProcessor.uiWRRegisterValue == mFT800CoProcessor.uiRDRegisterValue )
				{
					*pBufferEmpty = true;
				}

				sucCheckValue = 2;
			}
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( sucCheckValue <= 1 ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	if ( ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR ) || ( sucCheckValue >= 2 ) ) { sucCheckValue = 0; }		// Reset check value for next time

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the command coprocessor write register value.  To be executed after all	required commands have been buffered when the 'FT800_COPRCCMD__BAT_CONTINUOUS' access
 * 			type is being used.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmd__BufferDefine(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__Wr16( vAction, FT800__REG_CMD_WRITE, mFT800CoProcessor.uiWRRegisterValue );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_DLSTART (Sec 5.11).
 *
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_DLSTART(void)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_DLSTART );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_DLSTART (Sec 5.11).
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_DLSTART(FUNC_ACTION vAction)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_DLSTART );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SWAP (Sec 5.12).
 *
 * @param:	vSwapType - The display swap type.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SWAP(FT800_DISPLIST__DL_SWAP_TYPE vSwapType)
{
	/*
	CPData_Buffer32BitUValue(FT800_COPRCCMD__CMD_SWAP);
	*/ // Currently this does a swap by automatically writing to REG_DLSWAP a value of 2
	   // To allow the swap type to be defined this has been modified to write to the register via MEMWRITE command
	/*
	FT800_COPRCCMD__CPCmdQueue__CMD_MEMWRITE(FT800__REG_DLSWAP, 4);
	FT800_COPRCCMD__CPDatQueue__Value(vSwapType);
	*/ // Place in #ifdef to allow option
	   // Use value passed to determine command to queue, emilio 03-11-14
	if ( vSwapType == FT800_DISPLIST__DL_SWAP_TYPE_FRAME )
	{
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SWAP );
	}
	else
	{
		FT800_COPRCCMD__CPCmdQueue__CMD_MEMWRITE( FT800__REG_DLSWAP, 4 );
		FT800_COPRCCMD__CPDatQueue__Value( vSwapType );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SWAP (Sec 5.12).
 * 			When the co-processor engine executes this command, it requests a display list swap immediately after the current display list is scanned out.  Internally the
 * 			co-Processor engine implements this command by writing to REG_DLSWAP.
 *
 * @param:	vAction	  -	Action to take within function (Loop until complete or break out).
 * 			vSwapType - The display swap type.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SWAP(FUNC_ACTION vAction, FT800_DISPLIST__DL_SWAP_TYPE vSwapType)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		/*
		CPData_Buffer32BitUValue(FT800_COPRCCMD__CMD_SWAP);
		*/ // Currently this does a swap by automatically writing to REG_DLSWAP a value of 2
		   // To allow the swap type to be defined this has been modified to write to the register via MEMWRITE command
		/*
		CPData_Buffer32BitUValue(FT800_COPRCCMD__CMD_MEMWRITE);
		CPData_Buffer32BitUValue(FT800__REG_DLSWAP);
		CPData_Buffer32BitUValue(4);
		CPData_Buffer32BitUValue(vSwapType);
		*/ // Place in #ifdef to allow option
		   // Use value passed to determine command to execute, emilio 03-11-14
		if ( vSwapType == FT800_DISPLIST__DL_SWAP_TYPE_FRAME )
		{
			CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SWAP );
		}
		else
		{
			CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMWRITE );
			CPData_Buffer32BitUValue( FT800__REG_DLSWAP 		   );
			CPData_Buffer32BitUValue( 4 						   );
			CPData_Buffer32BitUValue( vSwapType 				   );
		}
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_COLDSTART (Sec 5.13).
 *
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_COLDSTART(void)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_COLDSTART );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_COLDSTART (Sec 5.13).
 * 			This command sets co-processor engine to reset default states.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_COLDSTART(FUNC_ACTION vAction)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_COLDSTART );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_INTERRUPT (Sec 5.14).
 * 			When the co-processor engine executes this command, it triggers interrupt INT_CMDFLAG.
 *
 * @param:	vulDelayMS - The delay before interrupt trigger in milliseconds.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_INTERRUPT(uint32_t vulDelayMS)
{
	CPData_Buffer32BitUValue( 0xFFFFFF02 );
	CPData_Buffer32BitUValue( vulDelayMS );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_INTERRUPT (Sec 5.14).
 * 			When the co-processor engine executes this command, it triggers interrupt INT_CMDFLAG.
 *
 * @param:	vAction	   - Action to take within function (Loop until complete or break out).
 * 			vulDelayMS - The delay before interrupt trigger in milliseconds.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_INTERRUPT(FUNC_ACTION vAction, uint32_t vulDelayMS)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( 0xFFFFFF02 );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulDelayMS );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_APPEND (Sec 5.15).
 * 			Appends a block of memory to the current display list memory address where the offset is specified in REG_CMD_DL.
 *
 * @param:	vulPointer  - Start of source commands in main memory.
 * 			vulNumBytes - Number of bytes to copy (Must be a multiple of 4).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_APPEND(uint32_t vulPointer, uint32_t vulNumBytes)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_APPEND );
	CPData_Buffer32BitUValue( vulPointer 				 );
	CPData_Buffer32BitUValue( vulNumBytes 				 );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_APPEND (Sec 5.15).
 * 			Appends a block of memory to the current display list memory address where the offset is specified in REG_CMD_DL.
 *
 * @param:	vAction		- Action to take within function (Loop until complete or break out).
 * 			vulPointer  - Start of source commands in main memory.
 * 			vulNumBytes - Number of bytes to copy (Must be a multiple of 4).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_APPEND(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_APPEND );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer 				 );
		CPData_Buffer32BitUValue( vulNumBytes 				 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_REGREAD (Sec 5.16).
 * 			Read a register value.  The process in this function waits for the action to complete and as such can only be used with the access type: 'FT800_COPRCCMD__BAT_CHECK_BUFF'
 *
 * @param:	vAction	   - Action to take within function (Loop until complete or break out).
 * 			vulPointer - Address of register to read.
 * 			pulResult  - Pointer to memory location where result to be placed.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_REGREAD(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t* pulRegisterValue)
{
	FT800_DEVICEIF__XFER_STATUS	CmdXferStatus;

	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( 0xFFFFFF19 );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer );
		// - Allow space for result
		CPData_Buffer32BitUValue( 0 		 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction  		   = true;
		mFT800CoProcessor.Readback.ulOffset 		   = 8;
		mFT800CoProcessor.Readback.DataBuffer.pucData  = &maucFT800CoProcessorRxBuffer[0];
		mFT800CoProcessor.Readback.DataBuffer.ulLength = 4;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}
	//
	// Update the transfer
	CmdXferStatus = CPCmdUpdate_CheckBuffer( vAction );
	//
	// Check if done to update result value
	if ( CmdXferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		// Start at offset 1
		uint8_t ucIndex = 1;
		// Get result
		*pulRegisterValue =  (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pulRegisterValue |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pulRegisterValue |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pulRegisterValue |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
	}

	return CmdXferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_MEMWRITE (Sec 5.17).
 * 			Writes the bytes that follow into the FT800 memory.  This command can be used to set register values or to update contents at specific times.
 *
 * @param:	vulPointer  - The memory address to be written.
 * 			vulNumBytes - Number of bytes to be written.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMWRITE(uint32_t vulPointer, uint32_t vulNumBytes)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMWRITE );
	CPData_Buffer32BitUValue( vulPointer 				   );
	CPData_Buffer32BitUValue( vulNumBytes 				   );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_MEMWRITE (Sec 5.17).
 * 			Writes the bytes that follow into the FT800 memory.  This command can be used to set register values or to update contents at specific times.
 *
 * @param:	vAction		- Action to take within function (Loop until complete or break out).
 * 			vulPointer  - The memory address to be written.
 * 			vulNumBytes - Number of bytes to be written.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_MEMWRITE(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMWRITE );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer 				   );
		CPData_Buffer32BitUValue( vulNumBytes 				   );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_INFLATE (Sec 5.18).
 * 			Decompress the following compressed data into the FT800 memory, RAM_G.  The data should have been compressed with the DEFLATE algorithm (eg: using the ZLIB library).
 * 			This is particularly useful for loading graphics data.
 *
 * @param:	vulPointer - Destination address.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_INFLATE(uint32_t vulPointer)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_INFLATE );
	CPData_Buffer32BitUValue( vulPointer 				  );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_INFLATE (Sec 5.18).
 * 			Decompress the following compressed data into the FT800 memory, RAM_G.  The data should have been compressed with the DEFLATE algorithm (eg: using the ZLIB library).
 * 			This is particularly useful for loading graphics data.
 *
 * @param:	vAction	   - Action to take within function (Loop until complete or break out).
 * 			vulPointer - Destination address.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_INFLATE(FUNC_ACTION vAction, uint32_t vulPointer)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_INFLATE );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer 				  );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_LOADIMAGE (Sec 5.19).
 * 			Decompress the following JPEG image data into an FT800 bitmap in main memory.  The image data should be a regular baseline JPEG (JFIF) image.
 * 			The application on the host processor has to parse the JPEG header to get the properties of the JPEG image and decide to decode.
 *
 * @param:	vulPointer	   - Destination address.
 * 			vulOptionsMask - Options mask : FT800_COPRCCMD__OPTION_MASK__OPT_RGB565.
 * 						 	     		    FT800_COPRCCMD__OPTION_MASK__OPT_MONO.
 * 							     		    FT800_COPRCCMD__OPTION_MASK__OPT_NODL.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_LOADIMAGE(uint32_t vulPointer, uint32_t vulOptionsMask)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_LOADIMAGE	);
	CPData_Buffer32BitUValue( vulPointer				  	);
	CPData_Buffer32BitUValue( vulOptionsMask				);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_LOADIMAGE (Sec 5.19).
 * 			Decompress the following JPEG image data into an FT800 bitmap in main memory.  The image data should be a regular baseline JPEG (JFIF) image.
 * 			The application on the host processor has to parse the JPEG header to get the properties of the JPEG image and decide to decode.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			vulPointer	   - Destinatiom address.
 * 			vulOptionsMask - Options mask : FT800_COPRCCMD__OPTION_MASK__OPT_RGB565.
 * 						 	     		    FT800_COPRCCMD__OPTION_MASK__OPT_MONO.
 * 							     		    FT800_COPRCCMD__OPTION_MASK__OPT_NODL.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_LOADIMAGE(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulOptionsMask)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_LOADIMAGE );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer 				    );
		CPData_Buffer32BitUValue( vulOptionsMask 				);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the co-processor command - CMD_MEMCRC (Sec 5.20).
 * 			Computes a CRC-32 for a block of FT800 memory.  The process in this function waits for the action to complete and as such can only be used with the access type:
 *
 * 					'FT800_COPRCCMD__BAT_CHECK_BUFF'
 *
 * @param:	vAction     - Action to take within function (Loop until complete or break out).
 * 			vulPointer  - Starting address of memory block.
 *			vulNumBytes - Number of bytes.
 * 			pulCRC   	- Pointer where calculated checksum is placed.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_MEMCRC(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes, uint32_t* pulCRC)
{
	FT800_DEVICEIF__XFER_STATUS	CmdXferStatus;

	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( 0xFFFFFF18  );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer  );
		CPData_Buffer32BitUValue( vulNumBytes );
		// - Allow space for result
		CPData_Buffer32BitUValue( 0 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction  		   = true;
		mFT800CoProcessor.Readback.ulOffset 		   = 12;
		mFT800CoProcessor.Readback.DataBuffer.pucData  = &maucFT800CoProcessorRxBuffer[0];
		mFT800CoProcessor.Readback.DataBuffer.ulLength = 4;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}
	//
	// Update the transfer
	CmdXferStatus = CPCmdUpdate_CheckBuffer( vAction );
	//
	// Check if done to update result value
	if ( CmdXferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		// Start at offset 1
		uint8_t ucIndex = 1;
		// Get result
		*pulCRC =  (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pulCRC |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pulCRC |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pulCRC |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
	}

	return CmdXferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_MEMZERO (Sec 5.21).
 * 			Write zero to a block of memory.
 *
 * @param:	vulPointer  - Starting address of memory block.
 * 			vulNumBytes - Number of bytes to clear.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMZERO(uint32_t vulPointer, uint32_t vulNumBytes)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMZERO );
	CPData_Buffer32BitUValue( vulPointer 				  );
	CPData_Buffer32BitUValue( vulNumBytes 				  );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_MEMZERO (Sec 5.21).
 * 			Write zero to a block of memory.
 *
 * @param:	vAction		- Action to take within function (Loop until complete or break out).
 * 			vulPointer	- Starting address of memory block.
 * 			vulNumBytes - Number of bytes to clear.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_MEMZERO(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMZERO );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer 				  );
		CPData_Buffer32BitUValue( vulNumBytes 				  );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_MEMSET (Sec 5.22).
 * 			Fills memory with a byte value specified.
 *
 * @param:	vulPointer	- Starting address of memory block.
 * 			vulValue	- Value to set with.
 * 			vulNumBytes - Number of bytes to set.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMSET(uint32_t vulPointer, uint32_t vulValue, uint32_t vulNumBytes)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMSET );
	CPData_Buffer32BitUValue( vulPointer 				 );
	CPData_Buffer32BitUValue( vulValue 					 );
	CPData_Buffer32BitUValue( vulNumBytes 				 );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_MEMSET (Sec 5.22).
 * 			Fills memory with a byte value specified.
 *
 * @param:	vAction		- Action to take within function (Loop until complete or break out).
 * 			vulPointer	- Starting address of memory block.
 * 			vulValue	- Value to set with.
 * 			vulNumBytes - Number of bytes to set.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_MEMSET(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulValue, uint32_t vulNumBytes)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMSET );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer 				 );
		CPData_Buffer32BitUValue( vulValue 					 );
		CPData_Buffer32BitUValue( vulNumBytes 				 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_MEMCPY (Sec 5.23).
 * 			Copy a block of memory.
 *
 * @param:	vulPointerDest	 - Starting address destination memory block.
 * 			vulPointerSource - Starting address source memory block.
 * 			vulNumBytes		 - Number of bytes to copy.
 * @retval:	void.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMCPY(uint32_t vulPointerDest, uint32_t vulPointerSource, uint32_t vulNumBytes)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMCPY );
	CPData_Buffer32BitUValue( vulPointerDest 			 );
	CPData_Buffer32BitUValue( vulPointerSource 			 );
	CPData_Buffer32BitUValue( vulNumBytes 				 );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_MEMCPY (Sec 5.23).
 * 			Copy a block of memory.
 *
 * @param:	vAction			 - Action to take within function (Loop until complete or break out).
 *			vulPointerDest	 - Starting address destination memory block.
 * 			vulPointerSource - Starting address source memory block.
 * 			vulNumBytes		 - Number of bytes to copy.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_MEMCPY(FUNC_ACTION vAction, uint32_t vulPointerDest, uint32_t vulPointerSource, uint32_t vulNumBytes)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_MEMCPY );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointerDest			 );
		CPData_Buffer32BitUValue( vulPointerSource			 );
		CPData_Buffer32BitUValue( vulNumBytes				 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_BUTTON (Sec 5.24).
 * 			Draws a button.
 *
 * @param:	viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW			   - Width in pixels.
 * 			viH			   - Height in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			pstrText:	   - Pointer to null terminated string.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_BUTTON(int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_BUTTON );
	CPData_Buffer16BitIValue( viX						 );
	CPData_Buffer16BitIValue( viY					     );
	CPData_Buffer16BitIValue( viW						 );
	CPData_Buffer16BitIValue( viH						 );
	CPData_Buffer16BitIValue( viFont					 );
	CPData_Buffer16BitUValue( vuiOptionsMask			 );
	CPData_BufferStringValue( pstrText				 	);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_BUTTON (Sec 5.24).
 * 			Draws a button.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX  		   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW 		   - Width in pixels.
 * 			viH			   - Height in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			pstrText 	   - Pointer to null terminated string.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_BUTTON(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_BUTTON );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						 );
		CPData_Buffer16BitIValue( viY						 );
		CPData_Buffer16BitIValue( viW						 );
		CPData_Buffer16BitIValue( viH						 );
		CPData_Buffer16BitIValue( viFont					 );
		CPData_Buffer16BitUValue( vuiOptionsMask			 );
		CPData_BufferStringValue( pstrText				 	 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_CLOCK (Sec 5.25).
 * 			Draws an analog clock.
 *
 * @param:	viX			   - Centre X coordinate in pixels.
 * 			viY 		   - Centre Y coordinate in pixels.
 * 			viR			   - Radius in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOBACK.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOTICKS.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOHM.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOSECS.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOHANDS.
 * 			vuiHr   - Hours.
 * 			vuiMin	- Minutes.
 * 			vuiSec  - Seconds.
 * 			vuiMSec - Milliseconds.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_CLOCK(int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiHr, uint16_t vuiMin, uint16_t vuiSec, uint16_t vuiMSec)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_CLOCK );
	CPData_Buffer16BitIValue( viX						);
	CPData_Buffer16BitIValue( viY						);
	CPData_Buffer16BitIValue( viR						);
	CPData_Buffer16BitUValue( vuiOptionsMask			);
	CPData_Buffer16BitUValue( vuiHr						);
	CPData_Buffer16BitUValue( vuiMin					);
	CPData_Buffer16BitUValue( vuiSec					);
	CPData_Buffer16BitUValue( vuiMSec					);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_CLOCK (Sec 5.25).
 * 			Draws an analog clock.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX			   - Centre X coordinate in pixels.
 * 			viY			   - Centre Y coordinate in pixels.
 * 			viR			   - Radius in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOBACK.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOTICKS.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOHM.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOSECS.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOHANDS.
 * 			vuiHr	- Hours.
 * 			vuiMin	- Minutes.
 * 			vuiSec  - Seconds.
 * 			vuiMSec - Milliseconds.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_CLOCK(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiHr, uint16_t vuiMin, uint16_t vuiSec, uint16_t vuiMSec)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_CLOCK );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						);
		CPData_Buffer16BitIValue( viY						);
		CPData_Buffer16BitIValue( viR						);
		CPData_Buffer16BitUValue( vuiOptionsMask			);
		CPData_Buffer16BitUValue( vuiHr						);
		CPData_Buffer16BitUValue( vuiMin					);
		CPData_Buffer16BitUValue( vuiSec					);
		CPData_Buffer16BitUValue( vuiMSec					);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_FGCOLOR (Sec 5.26).
 * 			Sets the foreground color.
 *
 * @param:	vucR - Red color
 * 			vucG - Green color
 * 			vucB - Blue color
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_FGCOLOR(uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_FGCOLOR 	 );
	CPData_Buffer32BitUValue( ulRGBColor( vucR, vucG, vucB ) );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_FGCOLOR (Sec 5.26).
 * 			Sets the foreground color.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vucR	- Red color
 * 			vucG	- Green color
 * 			vucB	- Blue color
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_FGCOLOR(FUNC_ACTION vAction, uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_FGCOLOR 	 );
		// - Buffer associated data
		CPData_Buffer32BitUValue( ulRGBColor( vucR, vucG, vucB ) );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_BGCOLOR (sec 5.27).
 * 			Sets the background color.
 *
 * @param:	vucR - Red color
 * 			vucG - Green color
 * 			vucB - Blue color
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_BGCOLOR(uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_BGCOLOR    );
	CPData_Buffer32BitUValue( ulRGBColor( vucR, vucG, vucB ) );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_BGCOLOR (sec 5.27).
 * 			Sets the background color.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 *			vucR	- Red color
 * 			vucG	- Green color
 * 			vucB	- Blue color
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_BGCOLOR(FUNC_ACTION vAction, uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_BGCOLOR    );
		// - Buffer associated data
		CPData_Buffer32BitUValue( ulRGBColor( vucR, vucG, vucB ) );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_GRADCOLOR (sec 5.28).
 * 			Sets the 3D button highlight color.
 *
 * @param:	vucR - Red color
 * 			vucG - Green color
 * 			vucB - Blue color
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_GRADCOLOR(uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GRADCOLOR  );
	CPData_Buffer32BitUValue( ulRGBColor( vucR, vucG, vucB ) );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_GRADCOLOR (sec 5.28).
 * 			Sets the 3D button highlight color.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vucR	- Red color
 * 			vucG	- Green color
 * 			vucB	- Blue color
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_GRADCOLOR(FUNC_ACTION vAction, uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GRADCOLOR  );
		// - Buffer associated data
		CPData_Buffer32BitUValue( ulRGBColor( vucR, vucG, vucB ) );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_GAUGE (Sec 5.29).
 * 			Draws a gauge.
 *
 * @param:	viX			   - Centre X coordinate in pixels.
 * 			viY			   - Centre Y coordinate in pixels.
 * 			viR			   - Radius in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOBACK.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOTICKS.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOPOINTER.
 * 			vuiMajor - Major tick.
 * 			vuiMinor - Minor tick.
 * 			vuiValue - Value.
 * 			vuiRange - Range.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_GAUGE(int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiMajor, uint16_t vuiMinor, uint16_t vuiValue, uint16_t vuiRange)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GAUGE );
	CPData_Buffer16BitIValue( viX						);
	CPData_Buffer16BitIValue( viY						);
	CPData_Buffer16BitIValue( viR						);
	CPData_Buffer16BitUValue( vuiOptionsMask			);
	CPData_Buffer16BitUValue( vuiMajor					);
	CPData_Buffer16BitUValue( vuiMinor					);
	CPData_Buffer16BitUValue( vuiValue					);
	CPData_Buffer16BitUValue( vuiRange					);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_GAUGE (Sec 5.29).
 * 			Draws a gauge.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX			   - Centre X coordinate in pixels.
 * 			viY			   - Centre Y coordinate in pixels.
 * 			viR			   - Radius in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOBACK.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOTICKS.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_NOPOINTER.
 * 			vuiMajor - Major tick.
 * 			vuiMinor - Minor tick.
 * 			vuiValue - Value.
 * 			vuiRange - Range.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_GAUGE(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiMajor, uint16_t vuiMinor, uint16_t vuiValue, uint16_t vuiRange)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GAUGE );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						);
		CPData_Buffer16BitIValue( viY						);
		CPData_Buffer16BitIValue( viR						);
		CPData_Buffer16BitUValue( vuiOptionsMask			);
		CPData_Buffer16BitUValue( vuiMajor					);
		CPData_Buffer16BitUValue( vuiMinor					);
		CPData_Buffer16BitUValue( vuiValue					);
		CPData_Buffer16BitUValue( vuiRange					);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_GRADIENT (Sec 5.30).
 * 			Draws a smooth color gradient.
 *
 * @param:	viX0   - Point 0 X coordinate in pixels.
 * 			viY0   - Point 0 Y coordinate in pixels.
 * 			viRGB0 - Color of point 0 - 0x00RRGGBB.
 * 			viX1   - Point 1 X coordinate in pixels.
 * 			viY1   - Point 1 Y coordinate in pixels.
 * 			viRGB1 - Color of point 1 - 0x00RRGGBB.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_GRADIENT(int16_t viX0, int16_t viY0, uint32_t vulRGB0, int16_t viX1, int16_t viY1, uint32_t vulRGB1)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GRADIENT );
	CPData_Buffer16BitIValue( viX0						   );
	CPData_Buffer16BitIValue( viY0						   );
	CPData_Buffer32BitUValue( vulRGB0					   );
	CPData_Buffer16BitIValue( viX1						   );
	CPData_Buffer16BitIValue( viY1						   );
	CPData_Buffer32BitUValue( vulRGB1					   );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_GRADIENT (Sec 5.30).
 * 			Draws a smooth color gradient.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			viX0	- Point 0 X coordinate in pixels.
 * 			viY0	- Point 0 Y coordinate in pixels.
 * 			viRGB0	- Color of point 0 - 0x00RRGGBB.
 * 			viX1	- Point 1 X coordinate in pixels.
 * 			viY1	- Point 1 Y coordinate in pixels.
 * 			viRGB1	- Color of point 1 - 0x00RRGGBB.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_GRADIENT(FUNC_ACTION vAction, int16_t viX0, int16_t viY0, uint32_t vulRGB0, int16_t viX1, int16_t viY1, uint32_t vulRGB1)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GRADIENT );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX0						   );
		CPData_Buffer16BitIValue( viY0						   );
		CPData_Buffer32BitUValue( vulRGB0					   );
		CPData_Buffer16BitIValue( viX1						   );
		CPData_Buffer16BitIValue( viY1						   );
		CPData_Buffer32BitUValue( vulRGB1					   );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_KEYS (Sec 5.31).
 * 			Draw a row of keys.
 *
 * @param:	viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW			   - Width of the keys in pixels.
 * 			viH			   - Height of the keys in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 				 						   FT800_COPRCCMD__OPTION_MASK__OPT_CENTER.
 * 			pstrKeysString - Pointer to null terminated string.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_KEYS(int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrKeysString)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_KEYS );
	CPData_Buffer16BitIValue( viX					   );
	CPData_Buffer16BitIValue( viY					   );
	CPData_Buffer16BitIValue( viW					   );
	CPData_Buffer16BitIValue( viH					   );
	CPData_Buffer16BitIValue( viFont				   );
	CPData_Buffer16BitUValue( vuiOptionsMask		   );
	CPData_BufferStringValue( pstrKeysString		   );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_KEYS (Sec 5.31).
 * 			Draw a row of keys.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW			   - Width of the keys in pixels.
 * 			viH			   - Height of the keys in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 				 						   FT800_COPRCCMD__OPTION_MASK__OPT_CENTER.
 * 			pstrKeysString - Pointer to null terminated string.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_KEYS(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrKeysString)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( 0xFFFFFF0E 	 );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX		 	 );
		CPData_Buffer16BitIValue( viY		 	 );
		CPData_Buffer16BitIValue( viW			 );
		CPData_Buffer16BitIValue( viH			 );
		CPData_Buffer16BitIValue( viFont		 );
		CPData_Buffer16BitUValue( vuiOptionsMask );
		CPData_BufferStringValue( pstrKeysString );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_PROGRESS (Sec 5.32).
 * 			Draws a progress bar.
 *
 * @param:	viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW			   - Width of progress bar in pixels.
 * 			viH			   - Height of progress bar in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue - Displayed value of progress bar between 0 and range inclusive.
 * 			vuiRange - Maximum value
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_PROGRESS(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_PROGRESS );
	CPData_Buffer16BitIValue( viX						   );
	CPData_Buffer16BitIValue( viY						   );
	CPData_Buffer16BitIValue( viW						   );
	CPData_Buffer16BitIValue( viH						   );
	CPData_Buffer16BitUValue( vuiOptionsMask			   );
	CPData_Buffer16BitUValue( vuiValue					   );
	CPData_Buffer16BitUValue( vuiRange					   );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_PROGRESS (Sec 5.32).
 * 			Draws a progress bar.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW			   - Width of progress bar in pixels.
 * 			viH			   - Height of progress bar in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue - Displayed value of progress bar between 0 and range inclusive.
 * 			vuiRange - Maximum value
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_PROGRESS(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_PROGRESS );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						   );
		CPData_Buffer16BitIValue( viY						   );
		CPData_Buffer16BitIValue( viW						   );
		CPData_Buffer16BitIValue( viH						   );
		CPData_Buffer16BitUValue( vuiOptionsMask			   );
		CPData_Buffer16BitUValue( vuiValue					   );
		CPData_Buffer16BitUValue( vuiRange					   );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SCROLLBAR (Sec 5.33).
 * 			Draws a scroll bar.
 *
 * @param:	viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW			   - Width of progress bar in pixels.  If width is greater than height the scroll bar is drawn horizontally.
 * 			viH			   - Height of progress bar in pixels.  If height is greater than width the scroll bar is drawn vertically.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue - Displayed value of scroll bar between 0 and range inclusive.
 * 			vuiSize  - ???.
 * 			vuiRange - Maximum value.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SCROLLBAR(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiSize, uint16_t vuiRange)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SCROLLBAR );
	CPData_Buffer16BitIValue( viX							);
	CPData_Buffer16BitIValue( viY							);
	CPData_Buffer16BitIValue( viW							);
	CPData_Buffer16BitIValue( viH							);
	CPData_Buffer16BitUValue( vuiOptionsMask				);
	CPData_Buffer16BitUValue( vuiValue						);
	CPData_Buffer16BitUValue( vuiSize						);
	CPData_Buffer16BitUValue( vuiRange						);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SCROLLBAR (Sec 5.33).
 * 			Draws a scroll bar.
 *
 * @param:	vAction        - Action to take within function (Loop until complete or break out).
 * 			viX			   - X coordinate top-left in pixels.
 * 			viY 		   - Y coordinate top-left in pixels.
 * 			viW			   - Width of progress bar in pixels.  If width is greater than height the scroll bar is drawn horizontally.
 * 			viH			   - Height of progress bar in pixels.  If height is greater than width the scroll bar is drawn vertically.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue - Displayed value of scroll bar between 0 and range inclusive.
 * 			vuiSize  - ???.
 * 			vuiRange - Maximum value.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SCROLLBAR(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiSize, uint16_t vuiRange)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SCROLLBAR );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX							);
		CPData_Buffer16BitIValue( viY							);
		CPData_Buffer16BitIValue( viW							);
		CPData_Buffer16BitIValue( viH							);
		CPData_Buffer16BitUValue( vuiOptionsMask				);
		CPData_Buffer16BitUValue( vuiValue						);
		CPData_Buffer16BitUValue( vuiSize						);
		CPData_Buffer16BitUValue( vuiRange						);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SLIDER (Sec 5.34).
 * 			Draws a slider.
 *
 * @param:	viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW			   - Width of progress bar in pixels.  If width is greater than height the slider is drawn horizontally.
 * 			viH			   - Height of progress bar in pixels.  If height is greater than width slider is drawn vertically.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue - Displayed value of slider between 0 and range inclusive.
 * 			vuiRange - Maximum value.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SLIDER(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SLIDER );
	CPData_Buffer16BitIValue( viX						 );
	CPData_Buffer16BitIValue( viY						 );
	CPData_Buffer16BitIValue( viW						 );
	CPData_Buffer16BitIValue( viH						 );
	CPData_Buffer16BitUValue( vuiOptionsMask			 );
	CPData_Buffer16BitUValue( vuiValue					 );
	CPData_Buffer16BitUValue( vuiRange 					 );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SLIDER (Sec 5.34).
 * 			Draws a slider.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX            - X coordinate top-left in pixels.
 * 			viY            - Y coordinate top-left in pixels.
 * 			viW            - Width of progress bar in pixels.  If width is greater than height the slider is drawn horizontally.
 * 			viH			   - Height of progress bar in pixels.  If height is greater than width slider is drawn vertically.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue - Displayed value of slider between 0 and range inclusive.
 * 			vuiRange - Maximum value.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SLIDER(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SLIDER );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						 );
		CPData_Buffer16BitIValue( viY						 );
		CPData_Buffer16BitIValue( viW						 );
		CPData_Buffer16BitIValue( viH						 );
		CPData_Buffer16BitUValue( vuiOptionsMask			 );
		CPData_Buffer16BitUValue( vuiValue					 );
		CPData_Buffer16BitUValue( vuiRange					 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_DIAL (Sec 5.35).
 * 			Draws a rotary dial control.
 *
 * @param:	viX			   - X coordinate center in pixels.
 * 			viY			   - Y coordinate center in pixels.
 * 			viR			   - Radius of dial in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue 	   - Dial position : 0x0000 : Point down.
 * 										     0x4000 : Point left.
 * 										     0x8000 : Point up.
 * 										     0xC000 : Point right.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_DIAL(int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiValue)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_DIAL );
	CPData_Buffer16BitIValue( viX					   );
	CPData_Buffer16BitIValue( viY					   );
	CPData_Buffer16BitIValue( viR					   );
	CPData_Buffer16BitUValue( vuiOptionsMask		   );
	CPData_Buffer16BitUValue( vuiValue				   );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_DIAL (Sec 5.35).
 * 			Draws a rotary dial control.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX 		   - X coordinate center in pixels.
 * 			viY 		   - Y coordinate center in pixels.
 * 			viR 		   - Radius of dial in pixels.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiValue  	   - Dial position : 0x0000 : Point down.
 * 										     0x4000 : Point left.
 * 										     0x8000 : Point up.
 * 										     0xC000 : Point right.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_DIAL(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiValue)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_DIAL );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX					   );
		CPData_Buffer16BitIValue( viY					   );
		CPData_Buffer16BitIValue( viR					   );
		CPData_Buffer16BitUValue( vuiOptionsMask		   );
		CPData_Buffer16BitUValue( vuiValue				   );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_TOGGLE (Sec 5.36).
 * 			Draws a toggle switch.
 *
 * @param:	viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW 		   - Width of toggle switch in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiState 	   - Toggle state : 0 is off.
 * 										    65535 is on.
 * 			pstrLabel	   - Label for toggle. "\xff" used to separate the two labels used ("no\xffyes").
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_TOGGLE(int16_t viX, int16_t viY, int16_t viW, int16_t viFont, uint16_t vuiOptionsMask, uint16_t vuiState, const char* pstrLabel)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TOGGLE );
	CPData_Buffer16BitIValue( viX						 );
	CPData_Buffer16BitIValue( viY						 );
	CPData_Buffer16BitIValue( viW						 );
	CPData_Buffer16BitIValue( viFont					 );
	CPData_Buffer16BitUValue( vuiOptionsMask			 );
	CPData_Buffer16BitUValue( vuiState					 );
	CPData_BufferStringValue( pstrLabel				     );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_TOGGLE (Sec 5.36).
 * 			Draws a toggle switch.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX			   - X coordinate top-left in pixels.
 * 			viY			   - Y coordinate top-left in pixels.
 * 			viW 		   - Width of toggle switch in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_3D.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_FLAT.
 * 			vuiState 	   - Toggle state : 0 is off.
 * 										    65535 is on.
 * 			pstrLabel      - Label for toggle. "\xff" used to separate the two labels used ("no\xffyes").
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_TOGGLE(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viFont, uint16_t vuiOptionsMask, uint16_t vuiState, const char* pstrLabel)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TOGGLE );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						 );
		CPData_Buffer16BitIValue( viY					     );
		CPData_Buffer16BitIValue( viW						 );
		CPData_Buffer16BitIValue( viFont					 );
		CPData_Buffer16BitUValue( vuiOptionsMask			 );
		CPData_Buffer16BitUValue( vuiState					 );
		CPData_BufferStringValue( pstrLabel				 	 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_TEXT (Sec 5.37).
 * 			Draws text.
 *
 * @param:	viX			   - X coordinate text base in pixels.
 * 			viY			   - Y coordinate text base in pixels.
 * 			viFont 		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_CENTER.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX.
 * 			pstrText - Text label.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_TEXT(int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TEXT );
	CPData_Buffer16BitIValue( viX					   );
	CPData_Buffer16BitIValue( viY					   );
	CPData_Buffer16BitIValue( viFont				   );
	CPData_Buffer16BitUValue( vuiOptionsMask		   );
	CPData_BufferStringValue( pstrText			   );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_TEXT (Sec 5.37).
 * 			Draws text.
 *
 * @param:	vAction		   - Action to take within function (Loop until complete or break out).
 * 			viX			   - X coordinate text base in pixels.
 * 			viY			   - Y coordinate text base in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_CENTER.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX.
 * 			pstrText - Text label.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_TEXT(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup https://profiles.google.com/?hl=en-GB&tab=mXspecific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TEXT );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX					   );
		CPData_Buffer16BitIValue( viY					   );
		CPData_Buffer16BitIValue( viFont				   );
		CPData_Buffer16BitUValue( vuiOptionsMask		   );
		CPData_BufferStringValue( pstrText				   );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the co-processor command - CMD_NUMBER (Sec 5.38).
 * 			Draws a decimal number.
 *
 * @param:	viX			   - X coordinate text base in pixels.
 * 			viY 		   - Y coordinate text base in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_CENTER.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_SIGNED.
 * 			vilValue 	   - Number to display.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_NUMBER(int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, int32_t vilNumber)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_NUMBER );
	CPData_Buffer16BitIValue( viX						 );
	CPData_Buffer16BitIValue( viY						 );
	CPData_Buffer16BitIValue( viFont					 );
	CPData_Buffer16BitUValue( vuiOptionsMask			 );
	CPData_Buffer32BitIValue( vilNumber					 );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_NUMBER (Sec 5.38).
 * 			Draws a decimal number.
 *
 * @param:	vAction 	   - Action to take within function (Loop until complete or break out).
 * 			viX			   - X coordinate text base in pixels.
 * 			viY 		   - Y coordinate text base in pixels.
 * 			viFont		   - Font handle.
 * 			vuiOptionsMask - Option mask : FT800_COPRCCMD__OPTION_MASK__OPT_CENTER.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX.
 * 										   FT800_COPRCCMD__OPTION_MASK__OPT_SIGNED.
 * 			vilValue 	   - Number to display.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_NUMBER(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, int32_t vilNumber)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_NUMBER );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						 );
		CPData_Buffer16BitIValue( viY						 );
		CPData_Buffer16BitIValue( viFont					 );
		CPData_Buffer16BitUValue( vuiOptionsMask			 );
		CPData_Buffer32BitIValue( vilNumber					 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_LOADIDENTITY (Sec 5.39).
 * 			Sets the current matrix to the identity matrix.
 *
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_LOADIDENTITY(void)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_LOADIDENTITY );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_LOADIDENTITY (Sec 5.39).
 * 			Sets the current matrix to the identity matrix.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_LOADIDENTITY(FUNC_ACTION vAction)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_LOADIDENTITY );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SETMATRIX (Sec 5.40).
 * 			Writes the current matrix to the display list.
 *
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SETMATRIX(void)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SETMATRIX );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the co-processor command - CMD_SETMATRIX (Sec 5.40).
 * 			Writes the current matrix to the display list.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SETMATRIX(FUNC_ACTION vAction)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SETMATRIX );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_GETMATRIX (Sec 5.41).
 * 			Retrieves the current matrix coefficients.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			pilA	- Pointer to where to place matrix coefficient A.
 * 			pilB	- Pointer to where to place matrix coefficient B.
 * 			pilC	- Pointer to where to place matrix coefficient C.
 * 			pilD	- Pointer to where to place matrix coefficient D.
 * 			pilE	- Pointer to where to place matrix coefficient E.
 * 			pilF	- Pointer to where to place matrix coefficient F.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_GETMATRIX(FUNC_ACTION vAction, int32_t* pilA, int32_t* pilB, int32_t* pilC, int32_t* pilD, int32_t* pilE, int32_t* pilF)
{
	FT800_DEVICEIF__XFER_STATUS	CmdXferStatus;
	uint8_t						ucIndex;

	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GETMATRIX );
		// - Allow space for result
		CPData_Buffer32BitUValue( 0 );
		CPData_Buffer32BitUValue( 0 );
		CPData_Buffer32BitUValue( 0 );
		CPData_Buffer32BitUValue( 0 );
		CPData_Buffer32BitUValue( 0 );
		CPData_Buffer32BitUValue( 0 );
		// - Define readback details
		mFT800CoProcessor.Readback.bAction  		   = true;
		mFT800CoProcessor.Readback.ulOffset 		   = 4;
		mFT800CoProcessor.Readback.DataBuffer.pucData  = &maucFT800CoProcessorRxBuffer[0];
		mFT800CoProcessor.Readback.DataBuffer.ulLength = 24;
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}
	//
	// Update the transfer
	CmdXferStatus = CPCmdUpdate_CheckBuffer(vAction);
	//
	// Check if done to update result value
	if ( CmdXferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		// Start at offset 1
		ucIndex = 1;
		// Get CoeffA
		*pilA =  (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pilA |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pilA |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pilA |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
		// Get CoeffB
		*pilB =  (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pilB |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pilB |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pilB |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
		// Get CoeffC
		*pilC =  (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pilC |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pilC |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pilC |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
		// Get CoeffD
		*pilD =  (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pilD |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pilD |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pilD |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
		// Get CoeffE
		*pilE =  (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pilE |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pilE |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pilE |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
		// Get CoeffF
		*pilF =  (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pilF |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pilF |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pilF |= (int32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
	}

	return CmdXferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_GETPTR (Sec 5.42).
 * 			Get the end memory address of inflated data.
 *
 * @param:	vAction	   - Action to take within function (Loop until complete or break out).
 * 			pulPointer - Pointer to where the retrieved end memory address is placed.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_GETPTR(FUNC_ACTION vAction, uint32_t* pulPointer)
{
	FT800_DEVICEIF__XFER_STATUS	CmdXferStatus;

	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GETPTR );
		// - Allow space for result
		CPData_Buffer32BitUValue( 0 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction  		   = true;
		mFT800CoProcessor.Readback.ulOffset 		   = 4;
		mFT800CoProcessor.Readback.DataBuffer.pucData  = &maucFT800CoProcessorRxBuffer[0];
		mFT800CoProcessor.Readback.DataBuffer.ulLength = 4;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}
	//
	// Update the transfer
	CmdXferStatus = CPCmdUpdate_CheckBuffer(vAction);
	//
	// Check if done to update result value
	if ( CmdXferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		// Start at offset 1
		uint8_t ucIndex = 1;
		// Get result
		*pulPointer =  (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pulPointer |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pulPointer |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pulPointer |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
	}

	return CmdXferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_GETPTR (Sec 5.43).
 * 			Get the image properties swcompressed by CMD_LOADIMAGE.
 *
 * @param:	vAction	   - Action to take within function (Loop until complete or break out).
 * 			pulPointer - The address of image in RAM_G which was decompressed by last CMD_LOADIMAGE.
 * 			pulWidth   - The width of the image which was decompressed.
 * 			pulHeight  - The height of the image which was decompressed.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_GETPROPS(FUNC_ACTION vAction, uint32_t* pulPointer, uint32_t* pulWidth, uint32_t* pulHeight)
{
	FT800_DEVICEIF__XFER_STATUS	CmdXferStatus;

	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_GETPROPS );
		// - Allow space for result
		CPData_Buffer32BitUValue( 0 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction  		   = true;
		mFT800CoProcessor.Readback.ulOffset 		   = 4;
		mFT800CoProcessor.Readback.DataBuffer.pucData  = &maucFT800CoProcessorRxBuffer[0];
		mFT800CoProcessor.Readback.DataBuffer.ulLength = 12;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}
	//
	// Update the transfer
	CmdXferStatus = CPCmdUpdate_CheckBuffer(vAction);
	//
	// Check if done to update result value
	if ( CmdXferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		// Start at offset 1
		uint8_t ucIndex = 1;
		// Get result
		// - Pointer
		*pulPointer =  (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pulPointer |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pulPointer |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pulPointer |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
		// - Width
		*pulWidth =  (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pulWidth |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pulWidth |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pulWidth |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
		// - Height
		*pulHeight =  (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pulHeight |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pulHeight |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pulHeight |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
	}

	return CmdXferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SCALE (Sec 5.44).
 * 			Apply a scale to the current matrix.
 *
 * @param:	vilSx - X scale factor in signed 16, 16 bit fixed-point form.
 * 			vilSy - Y scale factor in signed 16, 16 bit fixed-point form.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SCALE(int32_t vilSx, int32_t vilSy)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SCALE );
	CPData_Buffer32BitIValue( vilSx						);
	CPData_Buffer32BitIValue( vilSy						);
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SCALE (Sec 5.44).
 * 			Apply a scale to the current matrix.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vilSx	- X scale factor in signed 16, 16 bit fixed-point form.
 * 			vilSy 	- Y scale factor in signed 16, 16 bit fixed-point form.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SCALE(FUNC_ACTION vAction, int32_t vilSx, int32_t vilSy)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SCALE );
		// - Buffer associated data
		CPData_Buffer32BitIValue( vilSx						);
		CPData_Buffer32BitIValue( vilSy						);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_ROTATE (Sec 5.45).
 * 			Apply a rotation to the current matrix.
 *
 * @param:	vilA - Clockwise rotation angle in units of 1/65536 of a circle.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_ROTATE(int32_t vilA)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_ROTATE );
	CPData_Buffer32BitIValue( vilA						 );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_ROTATE (Sec 5.45).
 * 			Apply a rotation to the current matrix.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vilA 	- Clockwise rotation angle in units of 1/65536 of a circle.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_ROTATE(FUNC_ACTION vAction, int32_t vilA)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue(FT800_COPRCCMD__CMD_ROTATE);
		// - Buffer associated data
		CPData_Buffer32BitIValue(vilA);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_TRANSLATE (Sec 5.46).
 * 			Apply a translation to the current matrix.
 *
 * @param:	vilTx -	X translate factor in signed 16, 16 bit fixed-point form.
 * 			vilTy -	Y translate factor in signed 16, 16 bit fixed-point form.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_TRANSLATE(int32_t vilTx, int32_t vilTy)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TRANSLATE );
	CPData_Buffer32BitIValue( vilTx							);
	CPData_Buffer32BitIValue( vilTy							);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_TRANSLATE (Sec 5.46).
 * 			Apply a translation to the current matrix.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vilTx	- X translate factor in signed 16, 16 bit fixed-point form.
 * 			vilTy	- Y translate factor in signed 16, 16 bit fixed-point form.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_TRANSLATE(FUNC_ACTION vAction, int32_t vilTx, int32_t vilTy)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TRANSLATE );
		// - Buffer associated data
		CPData_Buffer32BitIValue( vilTx							);
		CPData_Buffer32BitIValue( vilTy							);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the co-processor command - CMD_CALIBRATE (Sec 5.47).
 * 			Execute the touch screen calibration routine.
 * 			The calibration procedure collects three touches from the touch screen, the computes and loads an appropriate matrix into REG_TOUCH_TRANSFORM_A-F.  To use it create a
 * 			display list and then use CMD_CALIBRATE.  The co-processor engine overlays the touch targets on the current display list, gathers the calibration input and
 * 			updates REG_TOUCH_TRANSFORM_A-F.
 *
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_CALIBRATE(void)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_CALIBRATE );
	CPData_Buffer32BitUValue( 0 );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the co-processor command - CMD_CALIBRATE (Sec 5.47).
 * 			Execute the touch screen calibration routine.
 * 			The calibration procedure collects three touches from the touch screen, the computes and loads an appropriate matrix into REG_TOUCH_TRANSFORM_A-F.  To use it create a
 * 			display list and then use CMD_CALIBRATE.  The co-processor engine overlays the touch targets on the current display list, gathers the calibration input and
 * 			updates REG_TOUCH_TRANSFORM_A-F.
 *
 * @param:	vAction	  -	Action to take within function (Loop until complete or break out).
 * 			pulResult -	Pointer to where the result is placed on completion.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_CALIBRATE(FUNC_ACTION vAction, uint32_t* pulResult)
{
	FT800_DEVICEIF__XFER_STATUS	CmdXferStatus;

	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_CALIBRATE );
		// - Allow space for result
		CPData_Buffer32BitUValue( 0 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction  		   = true;
		mFT800CoProcessor.Readback.ulOffset 		   = 4;
		mFT800CoProcessor.Readback.DataBuffer.pucData  = &maucFT800CoProcessorRxBuffer[0];
		mFT800CoProcessor.Readback.DataBuffer.ulLength = 4;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}
	//
	// Update the transfer
	CmdXferStatus = CPCmdUpdate_CheckBuffer( vAction );
	//
	// Check if done to update result value
	if ( CmdXferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		// Start at offset 1
		uint8_t ucIndex = 1;
		// Get result
		*pulResult =  (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++];
		*pulResult |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 8;
		*pulResult |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 16;
		*pulResult |= (uint32_t)maucFT800CoProcessorRxBuffer[ucIndex++] << 24;
	}

	return CmdXferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SPINNER (Sec 5.48).
 * 			Start an animated spinner.
 *
 * @param:	viX		- X coordinate top left of spinner in pixels.
 * 			viY	    - Y coordinate top left of spinner in pixels.
 * 			viStyle - Style where valid range is 0 to 3.
 * 			viScale - Scaling coefficient of spinner (0 is no scaling).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SPINNER(int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiStyle, uint16_t vuiScale)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SPINNER );
	CPData_Buffer16BitIValue( viX						  );
	CPData_Buffer16BitIValue( viY						  );
	CPData_Buffer16BitUValue( vuiStyle					  );
	CPData_Buffer16BitUValue( vuiScale					  );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SPINNER (Sec 5.48).
 * 			Start an animated spinner.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			viX		- X coordinate top left of spinner in pixels.
 * 			viY		- Y coordinate top left of spinner in pixels.
 * 			viStyle	- Style where valid range is 0 to 3.
 * 			viScale	- Scaling coefficient of spinner (0 is no scaling).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SPINNER(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiStyle, uint16_t vuiScale)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SPINNER );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						  );
		CPData_Buffer16BitIValue( viY						  );
		CPData_Buffer16BitUValue( vuiStyle					  );
		CPData_Buffer16BitUValue( vuiScale					  );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SCREENSAVER (Sec 5.49).
 * 			Start an animated screen saver.
 * 			After the screensaver command, the co-processor engine continuously updates REG_MACRO_0 with VERTEX2F with varying (x,y) coordinates.  With an appropriate display list
 * 			this causes a bitmap to move around the screen without any MCU work.  Command CMD_STOP stops the update process.
 *
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SCREENSAVER(void)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SCREENSAVER );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the co-processor command - CMD_SCREENSAVER (Sec 5.49).
 * 			Start an animated screen saver.
 * 			After the screensaver command, the co-processor engine continuously updates REG_MACRO_0 with VERTEX2F with varying (x,y) coordinates.  With an appropriate display list
 * 			this causes a bitmap to move around the screen without any MCU work.  Command CMD_STOP stops the update process.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SCREENSAVER(FUNC_ACTION vAction)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue(FT800_COPRCCMD__CMD_SCREENSAVER);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SKETCH (Sec 5.50).
 * 			Start a continuous sketch update.
 * 			After the sketch command, the co-processor engine continuously samples the touch inputs and paints pixels into a bitmap, according to touch (x,y).  This means that the
 * 			user touch inputs are always drawn into the bitmap without any need for MUX work.  Command CMD_STOP stops the sketch process.
 *
 * @param:	viX		  - X coordinate top left of sketch area in pixels.
 * 			viY		  - Y coordinate top left of sketch area in pixels.
 * 			vuiW	  - Width of sketch area in pixels.
 * 			vuiH	  - Height of sketch area in pixels.
 * 			vulPtr	  -	Base address of sketch bitmap.
 * 			vuiFormat -	Format of sketch bitmap (L1 or L8).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SKETCH(int16_t viX, int16_t viY, uint16_t vuiW, uint16_t vuiH, uint32_t vulPtr, uint16_t vuiFormat)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SKETCH );
	CPData_Buffer16BitIValue( viX						 );
	CPData_Buffer16BitIValue( viY						 );
	CPData_Buffer16BitUValue( vuiW						 );
	CPData_Buffer16BitUValue( vuiH						 );
	CPData_Buffer32BitUValue( vulPtr					 );
	CPData_Buffer16BitUValue( vuiFormat 				 );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SKETCH (Sec 5.50).
 * 			Start a continuous sketch update.
 * 			After the sketch command, the co-processor engine continuously samples the touch inputs and paints pixels into a bitmap, according to touch (x,y).  This means that the
 * 			user touch inputs are always drawn into the bitmap without any need for MUX work.  Command CMD_STOP stops the sketch process.
 *
 * @param:	vAction	  - Action to take within function (Loop until complete or break out).
 * 			viX		  - X coordinate top left of sketch area in pixels.
 * 			viY		  - Y coordinate top left of sketch area in pixels.
 * 			vuiW	  - Width of sketch area in pixels.
 * 			vuiH	  - Height of sketch area in pixels.
 * 			vulPtr    - Base address of sketch bitmap.
 * 			vuiFormat -	Format of sketch bitmap (L1 or L8).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SKETCH(FUNC_ACTION vAction, int16_t viX, int16_t viY, uint16_t vuiW, uint16_t vuiH, uint32_t vulPtr, uint16_t vuiFormat)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SKETCH );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						 );
		CPData_Buffer16BitIValue( viY						 );
		CPData_Buffer16BitUValue( vuiW						 );
		CPData_Buffer16BitUValue( vuiH						 );
		CPData_Buffer32BitUValue( vulPtr					 );
		CPData_Buffer16BitUValue( vuiFormat					 );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_STOP (Sec 5.51).
 * 			Stop any spinner, screensaver or sketch.
 *
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_STOP(void)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_STOP );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_STOP (Sec 5.51).
 * 			Stop any spinner, screensaver or sketch.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_STOP(FUNC_ACTION vAction)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_STOP );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SETFONT (Sec 5.52).
 * 			Set up a custom FONT.
 * 			CMD_SETFONT is used to register one custom defined bitmap font into the FT800 co-processor engine.  After registration,	the FT800 co-processor engine is able to use
 * 			the bitmap font with its co-processor command.
 *
 * @param:	vulFont - The bitmap handle from 0 to 14.
 * 			vulPtr  - The metric block address in RAM.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SETFONT(uint32_t vulFont, uint32_t vulPtr)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SETFONT );
	CPData_Buffer32BitUValue( vulFont					  );
	CPData_Buffer32BitUValue( vulPtr					  );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SETFONT (Sec 5.52).
 * 			Set up a custom FONT.
 * 			CMD_SETFONT is used to register one custom defined bitmap font into the FT800 co-processor engine.  After registration, the FT800 co-processor engine is able to use the
 * 			bitmap font with its co-processor command.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vulFont - The bitmap handle from 0 to 14.
 * 			vulPtr  - The metric block address in RAM.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SETFONT(FUNC_ACTION vAction, uint32_t vulFont, uint32_t vulPtr)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SETFONT );
		CPData_Buffer32BitUValue( vulFont					  );
		CPData_Buffer32BitUValue( vulPtr					  );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_TRACK (Sec 5.53).
 * 			Track touches for a graphics object.
 * 			This command will enable co-processor engine to track the touch on the particular graphics object with one valid tag value assigned.  Then co-processor engine will
 * 			update the REG_TRACKER periodically with the frame rate of LCD display panel.
 *
 * 			Co-processor engine tracks the graphics object in rotary tracker mode and linear tracker mode:
 *
 * 			Rotary Tracker Mode:
 * 			- Track the angle between the touching point and the center of graphics object specified by tag value.  The value is
 * 			  in units of 1/65536 of a circle : 0x0000 - Down.
 * 			  									0x4000 - Left.
 * 			  									0x8000 - Up.
 * 			  									0xC000 - Right
 *
 *			Linear Tracker Mode:
 *			- If parameter W is greater than H, track the relative distance of touching point to the width of graphics object specified by tag value.  If parameter W is not
 *			  greater than H, track the relative distance of touching point to the height of graphics object specified by tag value.  The value is in units of 1/65536 of the
 *			  width or height of the graphics object.  The distance of touching point refers to the distance from the top left pixel of graphics object to the coordinate of
 *			  touching point.
 *
 * @param:	viX:   - For linear tracker functionality - X coordinate of track area top-left in pixels.
 *				 	 For rotary tracker functionality - X coordinate of track area center in pixels.
 * 			viY    - For linear tracker functionality - Y coordinate of track area top-left in pixels.
 *				 	 For rotary tracker functionality - Y coordinate of track area center in pixels.
 *			viW:   - Width of track area in pixels.
 *			viH	   - Height of track area in pixels.  A W and H of (1,1) means that the tracker is rotary and reports an angle value in	REG_TRACKER.  A W and H of (0,0) disables
 *					 the track functionality.
 *			vucTag - Tag of the graphics object to be tracked (1-255)
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_TRACK(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint8_t vucTag)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TRACK );
	CPData_Buffer16BitIValue( viX						);
	CPData_Buffer16BitIValue( viY						);
	CPData_Buffer16BitIValue( viW						);
	CPData_Buffer16BitIValue( viH						);
	CPData_Buffer16BitUValue( (uint16_t)vucTag			);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_TRACK (Sec 5.53).
 * 			Track touches for a graphics object.
 * 			This command will enable co-processor engine to track the touch on the particular graphics object with one valid tag value assigned.  Then co-processor engine will
 * 			update the REG_TRACKER periodically with the frame rate of LCD display panel.
 *
 * 			Co-processor engine tracks the graphics object in rotary tracker mode and linear tracker mode:
 *
 * 			Rotary Tracker Mode:
 * 			- Track the angle between the touching point and the center of graphics object specified by tag value.  The value is
 * 			  in units of 1/65536 of a circle : 0x0000 - Down.
 * 			  									0x4000 - Left.
 * 			  									0x8000 - Up.
 * 			  									0xC000 - Right
 *
 *			Linear Tracker Mode:
 *			- If parameter W is greater than H, track the relative distance of touching point to the width of graphics object specified by tag value.  If parameter W is not
 *			  greater than H, track the relative distance of touching point to the height of graphics object specified by tag value.  The value is in units of 1/65536 of the
 *			  width or height of the graphics object.  The distance of touching point refers to the distance from the top left pixel of graphics object to the coordinate of
 *			  touching point.
 *
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			viX:    - For linear tracker functionality - X coordinate of track area top-left in pixels.
 *				 	  For rotary tracker functionality - X coordinate of track area center in pixels.
 * 			viY     - For linear tracker functionality - Y coordinate of track area top-left in pixels.
 *				 	  For rotary tracker functionality - Y coordinate of track area center in pixels.
 *			viW:    - Width of track area in pixels.
 *			viH	    - Height of track area in pixels.  A W and H of (1,1) means that the tracker is rotary and reports an angle value in	REG_TRACKER.  A W and H of (0,0) disables
 *					  the track functionality.
 *			vucTag  - Tag of the graphics object to be tracked (1-255)
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_TRACK(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint8_t vucTag)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_TRACK );
		// - Buffer associated data
		CPData_Buffer16BitIValue( viX						);
		CPData_Buffer16BitIValue( viY						);
		CPData_Buffer16BitIValue( viW						);
		CPData_Buffer16BitIValue( viH						);
		CPData_Buffer16BitUValue( (uint16_t)vucTag			);
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue the co-processor command - CMD_SNAPSHOT (Sec 5.54).
 * 			Take a snapshot of the current screen.
 * 			This command causes the co-processor engine to take a snapshot of the current screen and write it into graphics memory as a ARGB4 bitmap.  The size of the bitmap is
 * 			the size of the screen, given by REG_HSIZE and REG_VSIZE registers.  During the snapshot process the display should be disabled by setting REG_PCLK to 0.
 *
 * @param:	vulPointer - Snapshot destination address in main memory.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPCmdQueue__CMD_SNAPSHOT(uint32_t vulPointer)
{
	CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SNAPSHOT );
	CPData_Buffer32BitUValue( vulPointer				   );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue the co-processor command - CMD_SNAPSHOT (Sec 5.54).
 * 			Take a snapshot of the current screen.
 * 			This command causes the co-processor engine to take a snapshot of the current screen and write it into graphics memory as a ARGB4 bitmap.  The size of the bitmap is
 * 			the size of the screen, given by REG_HSIZE and REG_VSIZE registers.  During the snapshot process the display should be disabled by setting REG_PCLK to 0.
 *
 * @param:	vAction    - Action to take within function (Loop until complete or break out).
 * 			vulPointer - Snapshot destination address in main memory.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPCmdIssue__CMD_SNAPSHOT(FUNC_ACTION vAction, uint32_t vulPointer)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer the command
		CPData_Buffer32BitUValue( FT800_COPRCCMD__CMD_SNAPSHOT );
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulPointer				   );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queue data for the co-processor buffer.
 * @param:	vulValue:	Value to assign
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_COPRCCMD__CPDatQueue__Value(uint32_t vulValue)
{
	CPData_Buffer32BitUValue( vulValue );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue data to the co-processor buffer.
 * @param:	vAction  - Action to take within function (Loop until complete or break out).
 * 			vulValue - Value to assign
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPDatIssue__Value(FUNC_ACTION vAction, uint32_t vulValue)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer associated data
		CPData_Buffer32BitUValue( vulValue );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
		// - Define bytes to write and number written
		mFT800CoProcessor.Command.ulBytesToWrite  = mFT800CoProcessor.Command.DataBuffer.ulLength;
		mFT800CoProcessor.Command.ulBytesWritten  = mFT800CoProcessor.Command.DataBuffer.ulLength;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return CPCmdUpdate_Continuous( vAction );
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Issue data to the co-processor buffer.  Note that this function can ONLY be used under the mode:
 *
 * 											'FT800_COPRCCMD__BAT_CHECK_BUFF'
 *
 * @param:	vAction     - Action to take within function (Loop until complete or break out).
 * 			pucBuffer   - Pointer to buffer of data.
 * 			vuiNumBytes - Number of bytes.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CPDatIssue__Buffer(FUNC_ACTION vAction, uint8_t* pucBuffer, uint32_t vulNumBytes)
{
	if ( mFT800CoProcessor.CmdXferState == FT800_COPRCCMD__CXS__IDLE )
	{
		// Setup specific data - will happen only once
		// - Define total number of bytes to write
		mFT800CoProcessor.Command.ulBytesWritten = 0;
		mFT800CoProcessor.Command.ulBytesToWrite = vulNumBytes;
		mFT800CoProcessor.Command.pucSrcBuffer   = pucBuffer;
		// - Buffer initialise
		CPCmd__DataBufferInit();
		// - Buffer associated data
		CPData_BufferNDataBytes( mFT800CoProcessor.Command.pucSrcBuffer );
		// - Flag if readback action
		mFT800CoProcessor.Readback.bAction = false;
	}

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_CHECK_BUFF )
	{
		return CPCmdUpdate_CheckBuffer( vAction );
	}
	else
	{
		return FT800_DEVICEIF__XS_DONE_ERROR;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks to see if the co-processor engine FIFO buffer is empty (all commands processed).
 * @param:	vAction       - Action to take within function (Loop until complete or break out).
 * 			pbFIFOIsEmpty -	Pointer to where result is placed.
 * @retval:	Command Transfer Status (FT800_DEVICEIF__XFER_STATUS).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_COPRCCMD__CheckCPFIFOEmpty(FUNC_ACTION vAction, bool* pbFIFOIsEmpty)
{
	static uint16_t				suiWRRegisterValue, suiRDRegisterValue;
	static uint8_t				sucCheckState = 0;

	FT800_DEVICEIF__XFER_STATUS	XferStatus = FT800_DEVICEIF__XS_ACTV;

	do
	{
		if ( sucCheckState == 0 )
		{
			// Get REG_CMD_WRITE
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_WRITE, &suiWRRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				sucCheckState++;
			}
		}

		if ( sucCheckState == 1 )
		{
			// Get REG_CMD_READ
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_READ, &suiRDRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				*pbFIFOIsEmpty = (bool)(suiWRRegisterValue == suiRDRegisterValue);

				sucCheckState++;
			}
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( sucCheckState <= 1 ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	if ( ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR ) || ( sucCheckState > 1 ) )	{ sucCheckState = 0; }

	return XferStatus;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update of the co-processor command transfer action - Continuous access type.
 * @param:	None.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static FT800_DEVICEIF__XFER_STATUS		CPCmdUpdate_Continuous(FUNC_ACTION vAction)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus = FT800_DEVICEIF__XS_ACTV;

	do
	{
		XferStatus = FT800_DEVICEIF__WrN8( vAction, (uint32_t)FT800__RAM_CMD + (uint32_t)mFT800CoProcessor.uiWRRegisterValue, &mFT800CoProcessor.Command.DataBuffer );
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	switch ( XferStatus )
	{
		case FT800_DEVICEIF__XS_DONE_VALID:
			// Update the WR register value
			mFT800CoProcessor.uiWRRegisterValue = uiIncrementCMDBufferOffset( mFT800CoProcessor.uiWRRegisterValue, (uint16_t)mFT800CoProcessor.Command.DataBuffer.ulLength );
			// Revert Idle
			mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__IDLE;
			break;

		case FT800_DEVICEIF__XS_DONE_ERROR:
			// Revert Idle
			mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__IDLE;
			break;

		case FT800_DEVICEIF__XS_ACTV:
			// Indicate writing
			mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__WRTE;
			break;
	}

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update of the co-processor command transfer action - Check buffer access type.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static FT800_DEVICEIF__XFER_STATUS		CPCmdUpdate_CheckBuffer(FUNC_ACTION vAction)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus = FT800_DEVICEIF__XS_ACTV;

	do
	{
		// Check enough space availabe to write to co-processor engine
		switch ( mFT800CoProcessor.CmdXferState )
		{
			case FT800_COPRCCMD__CXS__NEXT:
				// Setup specific data - will happen only once
				// - Buffer initialise
				mFT800CoProcessor.Command.DataBuffer.pucData  = &maucFT800CoProcessorTxBuffer[0];
				mFT800CoProcessor.Command.DataBuffer.ulLength = 0;
				// - Buffer associated data
				CPData_BufferNDataBytes( mFT800CoProcessor.Command.pucSrcBuffer );
				// break left out intentionally
			case FT800_COPRCCMD__CXS__IDLE:
			case FT800_COPRCCMD__CXS__WAIT:
				CPCmdUpdate_CheckBufferWait( vAction );
				break;

			default:
				break;
		}
		//
		// Check to write command and data to co-processor engine
		switch ( mFT800CoProcessor.CmdXferState )
		{
			case FT800_COPRCCMD__CXS__ACTV:
				CPCmdUpdate_CheckBufferActv( vAction );
				break;

			default:
				break;
		}
		//
		// Check to update write register of co-processor engine
		switch ( mFT800CoProcessor.CmdXferState )
		{
			case FT800_COPRCCMD__CXS__WRTE:
				CPCmdUpdate_CheckBufferWrte( vAction );
				break;

			default:
				break;
		}
		//
		// Check for a register readback action
		switch ( mFT800CoProcessor.CmdXferState )
		{
			case FT800_COPRCCMD__CXS__READ:
				CPCmdUpdate_CheckBufferRead( vAction );
				break;

			default:
				break;
		}

		switch ( mFT800CoProcessor.CmdXferState )
		{
			case FT800_COPRCCMD__CXS__DONE_VALID:
				XferStatus = FT800_DEVICEIF__XS_DONE_VALID; mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__IDLE;
				break;

			case FT800_COPRCCMD__CXS__DONE_ERROR:
				XferStatus = FT800_DEVICEIF__XS_DONE_ERROR; mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__IDLE;
				break;

			default:
				XferStatus = FT800_DEVICEIF__XS_ACTV;
				break;
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the 'WAIT' state when updating the co-processor command transfer action.
 * 			State changes when there is enough command memory to write the command.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPCmdUpdate_CheckBufferWait(FUNC_ACTION vAction)
{
	static uint8_t				sucCheckState = 0;

	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	uint16_t					uiSpaceAvailable;
	bool						bSpaceAvailable = false;

	mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__WAIT;

	do
	{
		if ( sucCheckState == 0 )
		{
			// Get REG_CMD_WRITE
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_WRITE, &mFT800CoProcessor.uiWRRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				// Retain write register in case readback needed
				mFT800CoProcessor.Readback.uiWRRegisterValue = mFT800CoProcessor.uiWRRegisterValue;
				// Next check state
				sucCheckState = 1;
			}
		}

		if ( sucCheckState == 1 )
		{
			// Get REG_CMD_READ
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_READ , &mFT800CoProcessor.uiRDRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				// Determine space available
				uiSpaceAvailable = (mFT800CoProcessor.uiWRRegisterValue - mFT800CoProcessor.uiRDRegisterValue) % 4096;
				uiSpaceAvailable = (4096 - 4) - uiSpaceAvailable;
				// Next check state
				if ( uiSpaceAvailable >= uiValueRoundedToNearestModulo4( (uint16_t)mFT800CoProcessor.Command.DataBuffer.ulLength ) )
				{
					bSpaceAvailable = true;
				}
				// Back to start check
				sucCheckState = 0;
			}
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( !bSpaceAvailable ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR )
	{
		// Reset for next time around
		sucCheckState = 0;
		// Flag command transfer error
		mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__DONE_ERROR;
	}
	else
	if ( bSpaceAvailable )
	{
		// Reset for next time around
		sucCheckState = 0;
		// Next cmd xfer state
		mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__ACTV;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the 'ACTV' state when updating the co-processor command transfer action.
 * 			This completes when the command and any associated data is written to the device.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPCmdUpdate_CheckBufferActv(FUNC_ACTION vAction)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	do
	{
		if ( ( XferStatus = FT800_DEVICEIF__WrN8( vAction, (uint32_t)FT800__RAM_CMD + (uint32_t)mFT800CoProcessor.uiWRRegisterValue, &mFT800CoProcessor.Command.DataBuffer ) ) ==  FT800_DEVICEIF__XS_DONE_VALID )
		{
			mFT800CoProcessor.uiWRRegisterValue = uiIncrementCMDBufferOffset( mFT800CoProcessor.uiWRRegisterValue, (uint16_t)mFT800CoProcessor.Command.DataBuffer.ulLength );
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__WRTE;
	}
	else
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR )
	{
		mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__DONE_ERROR;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the 'WRTE' state when updating the co-processor command transfer action.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPCmdUpdate_CheckBufferWrte(FUNC_ACTION vAction)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	do
	{
		XferStatus = FT800_DEVICEIF__Wr16( vAction, FT800__REG_CMD_WRITE, mFT800CoProcessor.uiWRRegisterValue );
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		if ( mFT800CoProcessor.Command.ulBytesWritten < mFT800CoProcessor.Command.ulBytesToWrite )
		{
			mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__NEXT;
		}
		else
		if ( mFT800CoProcessor.Readback.bAction )
		{
			mFT800CoProcessor.CmdXferReadState = FT800_COPRCCMD__CXRS__GET_WRREG;
			mFT800CoProcessor.CmdXferState 	   = FT800_COPRCCMD__CXS__READ;
		}
		else
		{
			mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__DONE_VALID;
		}
	}
	else
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR )
	{
		mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__DONE_ERROR;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Execute the 'READ' state when updating the co-processor command transfer action.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPCmdUpdate_CheckBufferRead(FUNC_ACTION vAction)
{
	static uint16_t				suiWRRegisterValue;
	static uint16_t				suiRDRegisterValue;
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	// NB: 'mFT800CoProcessor.CmdXferReadState' initialised in CPCmdUpdate_CheckBufferWrte().

	do
	{
		if ( mFT800CoProcessor.CmdXferReadState == FT800_COPRCCMD__CXRS__GET_WRREG )
		{
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_WRITE, &suiWRRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				mFT800CoProcessor.CmdXferReadState = FT800_COPRCCMD__CXRS__GET_RDREG;
			}
		}

		if ( mFT800CoProcessor.CmdXferReadState == FT800_COPRCCMD__CXRS__GET_RDREG )
		{
			if ( ( XferStatus = FT800_DEVICEIF__Rd16( vAction, FT800__REG_CMD_READ, &suiRDRegisterValue ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				if ( suiWRRegisterValue == suiRDRegisterValue ) { mFT800CoProcessor.CmdXferReadState = FT800_COPRCCMD__CXRS__GET_VALUE; }
			}
		}

		if ( mFT800CoProcessor.CmdXferReadState == FT800_COPRCCMD__CXRS__GET_VALUE )
		{
			if ( ( XferStatus = FT800_DEVICEIF__RdN8( vAction, FT800__RAM_CMD + mFT800CoProcessor.Readback.uiWRRegisterValue + mFT800CoProcessor.Readback.ulOffset, &mFT800CoProcessor.Readback.DataBuffer ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				mFT800CoProcessor.CmdXferReadState = FT800_COPRCCMD__CXRS__COMPLETED;
			}
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( mFT800CoProcessor.CmdXferReadState != FT800_COPRCCMD__CXRS__COMPLETED ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_ERROR )
	{
		mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__DONE_ERROR;
	}
	else
	if ( mFT800CoProcessor.CmdXferReadState == FT800_COPRCCMD__CXRS__COMPLETED )
	{
		mFT800CoProcessor.CmdXferState = FT800_COPRCCMD__CXS__DONE_VALID;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Buffers a 32 bit unsigned value.
 * @param:	vulValue - The value to buffer.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPData_Buffer32BitUValue(uint32_t vulValue)
{
	uint8_t		ucIndex = 4;
	uint8_t		ucShift = 0;
	uint32_t	ulMask  = 0x000000FF;

	while ( ucIndex != 0 )
	{
		if ( mFT800CoProcessor.Command.DataBuffer.ulLength < mFT800CoProcessor.Command.DataBuffer.ulMaxLength )
		{
			mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = ( ( vulValue  & ulMask ) >> ucShift );
		}

		ucIndex--; ulMask <<= 8; ucShift += 8;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Buffers a 32 bit signed value.
 * @param:	vilValue - The value to buffer.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPData_Buffer32BitIValue(int32_t vilValue)
{
	uint8_t		ucIndex = 4;
	uint8_t		ucShift = 0;
	uint32_t	ulMask  = 0x000000FF;

	while ( ucIndex != 0 )
	{
		if ( mFT800CoProcessor.Command.DataBuffer.ulLength < mFT800CoProcessor.Command.DataBuffer.ulMaxLength )
		{
			mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = ( ( vilValue  & ulMask ) >> ucShift );
		}

		ucIndex--; ulMask <<= 8; ucShift += 8;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Buffers a 16 bit unsigned value.
 * @param:	vuiValue - The value to buffer.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPData_Buffer16BitUValue(uint16_t vuiValue)
{
	uint8_t		ucIndex = 2;
	uint8_t		ucShift = 0;
	uint16_t	ulMask  = 0x00FF;

	while ( ucIndex != 0 )
	{
		if ( mFT800CoProcessor.Command.DataBuffer.ulLength < mFT800CoProcessor.Command.DataBuffer.ulMaxLength )
		{
			mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = ( ( vuiValue  & ulMask ) >> ucShift );
		}

		ucIndex--; ulMask <<= 8; ucShift += 8;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Buffers a 16 bit signed value.
 * @param:	viValue - The value to buffer.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPData_Buffer16BitIValue(int16_t viValue)
{
	uint8_t		ucIndex = 2;
	uint8_t		ucShift = 0;
	uint16_t	ulMask  = 0x00FF;

	while ( ucIndex != 0 )
	{
		if ( mFT800CoProcessor.Command.DataBuffer.ulLength < mFT800CoProcessor.Command.DataBuffer.ulMaxLength )
		{
			mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = ( ( viValue  & ulMask ) >> ucShift );
		}

		ucIndex--; ulMask <<= 8; ucShift += 8;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Buffers null terminated string.
 * @param:	pString - Pointer to null terminated string.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPData_BufferStringValue(const char* pString)
{
	uint8_t		ucIndex = 0;
	char		cChar;

	do
	{
		cChar = pString[ucIndex++];

		if ( mFT800CoProcessor.Command.DataBuffer.ulLength < mFT800CoProcessor.Command.DataBuffer.ulMaxLength )
		{
			mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = cChar;
		}
	}
	while ( cChar != '\0' );

	while ( ( ( mFT800CoProcessor.Command.DataBuffer.ulLength % 4 ) != 0 ) && ( mFT800CoProcessor.Command.DataBuffer.ulLength < mFT800CoProcessor.Command.DataBuffer.ulMaxLength ) )
	{
		mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = '\0';
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Buffers N data bytes, where N is limited to either:
 *
 * 				- FT800_COPRCCMD__TX_BUFFER_MAX_BYTES   compared against mFT800CoProcessor.Command.DataBuffer.ulLength.
 * 				- mFT800CoProcessor.Command.ulBytesToWrite compared against mFT800CoProcessor.Command.ulBytesWritten
 *
 * @param:	pucBuffer -	Pointer to buffer holding data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPData_BufferNDataBytes(uint8_t* pucBuffer)
{
	uint16_t	uiIndex = 0;

	if ( mFT800CoProcessor.Command.BufferAccessType == FT800_COPRCCMD__BAT_QUEUE_XFER )
	{
		while ( ( uiIndex < mFT800CoProcessor.Command.ulBytesToWrite ) && ( mFT800CoProcessor.Command.DataBuffer.ulLength < mFT800CoProcessor.Command.DataBuffer.ulMaxLength ) )
		{
			mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = pucBuffer[uiIndex++];
		}
	}
	else
	{
		while ( ( mFT800CoProcessor.Command.DataBuffer.ulLength < FT800_COPRCCMD__TX_BUFFER_MAX_BYTES ) && ( mFT800CoProcessor.Command.ulBytesWritten < mFT800CoProcessor.Command.ulBytesToWrite ) )
		{
			mFT800CoProcessor.Command.DataBuffer.pucData[mFT800CoProcessor.Command.DataBuffer.ulLength++] = pucBuffer[mFT800CoProcessor.Command.ulBytesWritten++];
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rounds the value passed to the next value divisible by 4.
 * @param:	uiValue - Value to round.
 * @retval:	uint16_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static uint16_t							uiValueRoundedToNearestModulo4(uint16_t uiValue)
{
	while ( (uiValue % 4 ) != 0 )
	{
		uiValue++;
	}
	return uiValue;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the offset into RAM_CMD checking for wrap around.
 * @param:	vuiOffset  - Offset to update.
 * 			vuiCmdSize - The size of the command data.
 * @retval:	uint16_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static uint16_t							uiIncrementCMDBufferOffset(uint16_t vuiOffset, uint16_t vuiCmdSize)
{
    uint16_t	uiNewOffset;													// Used to hold new offset

    vuiCmdSize = uiValueRoundedToNearestModulo4( vuiCmdSize );					// Make sure command size divisible by 4

    uiNewOffset = vuiOffset + vuiCmdSize;										// Calculate new offset

    if ( uiNewOffset > 4095 )													// If new offset past boundary...
    {
    	uiNewOffset = (uiNewOffset - 4096);										// ... roll over pointer
    }
    return uiNewOffset;															// Return new offset
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the formatted RGB color as a 32 bit value
 * @param:	None.
 * @retval:	uint32_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static uint32_t							ulRGBColor(uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	return (uint32_t)( (uint32_t)vucR << 16 ) | ( (uint32_t)vucG << 8 ) | ( (uint32_t)vucB << 0 );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises the command buffer for the update modes : FT800_COPRCCMD__BAT_CONTINUOUS
 *																  FT800_COPRCCMD__BAT_CHECK_BUFF
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								CPCmd__DataBufferInit(void)
{
	mFT800CoProcessor.Command.DataBuffer.pucData     = &maucFT800CoProcessorTxBuffer[0];
	mFT800CoProcessor.Command.DataBuffer.ulMaxLength = FT800_COPRCCMD__TX_BUFFER_MAX_BYTES;
	mFT800CoProcessor.Command.DataBuffer.ulLength    = 0;

	return;
}
//*********************************************************************************************************************************************************************************
