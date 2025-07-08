//*********************************************************************************************************************************************************************************
// uiscncap.c
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
#include		<uiscncap.h>
#include		<uitftdsp.h>
#include		<FT800_deviceif.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			UISCNCAP__LINE_END 		272		// Lines 0 - 271 valid : 272 is end
#define			UISCNCAP__MAX_BUFF		2		// Max line buffers

typedef enum	UISCNCAP__CAPTURE_STATE {		// Screen Capture State
												//
	UISCNCAP__CS__IDLE 		  = 0,				// - Idle
	UISCNCAP__CS__INIT	      = 1,				// - Init ready for capture
	UISCNCAP__CS__ACTIVE	  = 2,				// - Active
	UISCNCAP__CS__COMPLETED   = 3,				// - Completed
	UISCNCAP__CS__ABORT   	  = 4 				// - Abort capture

} UISCNCAP__CAPTURE_STATE;

typedef enum	UISCNCAP__SCAN_STATE {			// Scan State
												//
	UISCNCAP__SS__SCAN_IDLE = 0,				// - Idle
	UISCNCAP__SS__SCAN_LINE = 1,				// - Tell device to scan the line
	UISCNCAP__SS__SCAN_WAIT	= 2,				// - Wait for completion
	UISCNCAP__SS__SCAN_READ	= 3,				// - Read RGB data out
	UISCNCAP__SS__UPLD_IDLE	= 4,				// - Upload idel
	UISCNCAP__SS__UPLD_WAIT = 5,				// - Wait for line data to be uploaded
	UISCNCAP__SS__UPLD_ACTV = 6,				// - Line data being uploaded
	UISCNCAP__SS__UPLD_DONE = 7					// - Line data has been uploaded

} UISCNCAP__SCAN_STATE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	ScreenCaptureLineBufferDetails {										// Line Buffer Details
																						//
	UISCNCAP__SCAN_STATE			ScanState;											// - Buffer line scan state
	uint8_t							ucIndex;											// - Index
	uint8_t							aucData[UISCNCAP__MAX_LINE_BUFFER_ELEMENTS];	// - Line buffer data : RGBRGBRGBRGB...
	uint16_t						uiYPos;												// - Current line being scanned : 0 based

} ScreenCaptureLineBufferDetails;

typedef struct	ScreenCaptureControlDetails {											// Screen Capture Control Details
																						//
	UISCNCAP__CAPTURE_STATE			State;												// - Overall state
	bool							bRequest;											// - Request capture
	bool							bAbort;												// - Abort capture
																						//
	bool							bIsValid;											// - Capture is valid flag
	GENTIMDT__TimingCounter			WaitForImageTiming;									// - General timing
																						//
	uint16_t						uiYLineCount;										// - Current line being scanned
																						//
																						//
	ScreenCaptureLineBufferDetails	aLineBuffer[UISCNCAP__MAX_BUFF];					// - Line buffer data x2 to speed up scan
	uint8_t							ucLineBufferScan;									// - Scan line buffer
	uint8_t							ucLineBufferUpld;									// - Upload line buffer

} ScreenCaptureControlDetails;

static ScreenCaptureControlDetails	mScreenCapture;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	UISCNCAP__kScreenCaptureWaitForImageTiming = { &mScreenCapture.WaitForImageTiming };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void						ScreenCaptureUpdate_Init(void);
static void						ScreenCaptureUpdate_Active(void);
static void						ScreenCaptureUpdate_Abort(void);

void							ScreenCapture_LineScanUpdate(ScreenCaptureLineBufferDetails* pLineBuffer);
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
 * @brief:	Screen capture module - Init.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UISCNCAP__ScreenCapture_Init(void)
{
	mScreenCapture.State    = UISCNCAP__CS__IDLE;
	mScreenCapture.bRequest = false;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Update state.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UISCNCAP__ScreenCapture_Update(void)
{
	if ( mScreenCapture.bRequest )
	{
		mScreenCapture.State    = UISCNCAP__CS__INIT;
		mScreenCapture.bRequest = false;
	}

	if ( mScreenCapture.bAbort )
	{
		mScreenCapture.State    = UISCNCAP__CS__ABORT;
		mScreenCapture.bAbort  = false;
	}

	switch ( mScreenCapture.State )
	{
		case UISCNCAP__CS__INIT:
			// Screen capture - Init
			ScreenCaptureUpdate_Init();
			break;

		case UISCNCAP__CS__ACTIVE:
			// Screen capture - Active
			ScreenCaptureUpdate_Active();
			break;

		case UISCNCAP__CS__COMPLETED:
			// Screen capture - Completed
			break;

		case UISCNCAP__CS__ABORT:
			// Screen capture - Abort
			ScreenCaptureUpdate_Abort();
			break;

		default:
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Flag that a capture has been requested.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UISCNCAP__ScreenCapture_FlagRequested(void)
{
	mScreenCapture.bRequest = true;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Line uploaded.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UISCNCAP__ScreenCapture_FlagLineUploaded(void)
{
	if ( mScreenCapture.aLineBuffer[0].ScanState == UISCNCAP__SS__UPLD_ACTV )
	{
		mScreenCapture.aLineBuffer[0].ScanState = UISCNCAP__SS__UPLD_DONE;
	}
	else
	{
		mScreenCapture.aLineBuffer[1].ScanState = UISCNCAP__SS__UPLD_DONE;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Abort.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UISCNCAP__ScreenCapture_FlagAbort(void)
{
	mScreenCapture.State = UISCNCAP__CS__ABORT;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Indicate when a capture is active.
 * @param:	None.
 * @retval:	bool - True if active.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UISCNCAP__ScreenCapture_bProcessActive(void)
{
	/*
	return (bool)( mScreenCapture.State != UISCNCAP__CS__IDLE );
	*/ // Need to get the process started so check for request flag also, NC7090_SCNCAP
	return ( (bool)( mScreenCapture.State != UISCNCAP__CS__IDLE ) || mScreenCapture.bRequest );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Scan of a line is in progress
 * @param:	None.
 * @retval:	bool - True if line scan in progress.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UISCNCAP__ScreenCapture_bLineScanInProgress(void)
{
	bool		bInProgress = false;

	switch ( mScreenCapture.aLineBuffer[0].ScanState )
	{
		case UISCNCAP__SS__SCAN_LINE:
		case UISCNCAP__SS__SCAN_WAIT:
		case UISCNCAP__SS__SCAN_READ:
			bInProgress = true;
			break;

		default:
			break;
	}

	switch ( mScreenCapture.aLineBuffer[1].ScanState )
	{
		case UISCNCAP__SS__SCAN_LINE:
		case UISCNCAP__SS__SCAN_WAIT:
		case UISCNCAP__SS__SCAN_READ:
			bInProgress = true;
			break;

		default:
			break;
	}

	return bInProgress;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Image captured ready for upload.
 * @param:	None.
 * @retval:	bool - True if ready to upload.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t*						UISCNCAP__ScreenCapture_pucLineUploadBuffer(uint16_t* puiTLineCount)
{
	uint8_t* pucLineBuffer = 0;

	if ( mScreenCapture.aLineBuffer[0].ScanState == UISCNCAP__SS__UPLD_WAIT )
	{
		*puiTLineCount = mScreenCapture.aLineBuffer[0].uiYPos+1;
		pucLineBuffer = &mScreenCapture.aLineBuffer[0].aucData[0];

		mScreenCapture.aLineBuffer[0].ScanState = UISCNCAP__SS__UPLD_ACTV;
	}
	else
	if ( mScreenCapture.aLineBuffer[1].ScanState == UISCNCAP__SS__UPLD_WAIT )
	{
		*puiTLineCount = mScreenCapture.aLineBuffer[1].uiYPos+1;
		pucLineBuffer = &mScreenCapture.aLineBuffer[1].aucData[0];

		mScreenCapture.aLineBuffer[1].ScanState = UISCNCAP__SS__UPLD_ACTV;
	}

	return pucLineBuffer;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Image captured ready for upload.
 * @param:	None.
 * @retval:	bool - True if ready to upload.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UISCNCAP__ScreenCapture_bCompleted(void)
{
	return (bool)( mScreenCapture.State == UISCNCAP__CS__COMPLETED );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Indicate if image capture valid.
 * @param:	None.
 * @retval:	bool - True if valid.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UISCNCAP__ScreenCapture_bIsValid(void)
{
	return mScreenCapture.bIsValid;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen capture module - Maximum lines.
 * @param:	None.
 * @retval:	uint16_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint16_t						UISCNCAP__ScreenCapture_uiMaxLineCount(void)
{
	return UISCNCAP__LINE_END;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen Capture Update - Init Capture State
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						ScreenCaptureUpdate_Init(void)
{
	if ( UITFTDSP__ScreenShot_bEnabled() )
	{
		// Assume scan will be valid
		mScreenCapture.bIsValid 	    = true;
		// Reset line count
		mScreenCapture.uiYLineCount     = 0;
		// Buffer indexing
		mScreenCapture.ucLineBufferScan = 0;
		mScreenCapture.ucLineBufferUpld = 0;
		// No delay
		GENTIMDT__StartTimer_TB_10MS( 0, &mScreenCapture.WaitForImageTiming );
		// Next capture state
		mScreenCapture.State = UISCNCAP__CS__ACTIVE;
		//
		// Line buffer init
		// - Bank 0
		mScreenCapture.aLineBuffer[0].ScanState   = UISCNCAP__SS__SCAN_LINE;
		mScreenCapture.aLineBuffer[0].uiYPos      = mScreenCapture.uiYLineCount;
		// - Bank 1
		mScreenCapture.aLineBuffer[1].ScanState   = UISCNCAP__SS__SCAN_IDLE;
		mScreenCapture.aLineBuffer[1].uiYPos      = 0;
	}
	else
	{
		// Error
		mScreenCapture.bIsValid = false;
		// Next state
		mScreenCapture.State = UISCNCAP__CS__COMPLETED;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen Capture Update - Active State
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						ScreenCaptureUpdate_Active(void)
{
	uint8_t		ucIndex;

	for ( ucIndex = 0; ucIndex < UISCNCAP__MAX_BUFF; ucIndex++ )
	{
		mScreenCapture.aLineBuffer[ucIndex].ucIndex = ucIndex; ScreenCapture_LineScanUpdate( &mScreenCapture.aLineBuffer[ucIndex] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen Capture Update - Abort State
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						ScreenCaptureUpdate_Abort(void)
{
	if ( UITFTDSP__ScreenShot_bDisabled() )
	{

	}
	else
	{
		// Something went wrong so force a reset

	}
	// Go idle
	mScreenCapture.State = UISCNCAP__CS__IDLE;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screen Capture Update - Scan Line State
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ScreenCapture_LineScanUpdate(ScreenCaptureLineBufferDetails* pLineBuffer)
{
	ScreenCaptureLineBufferDetails*	pOtherLineBuffer;
	bool		bIsBusy = true;

	if ( pLineBuffer->ucIndex == 0 ) { pOtherLineBuffer = &mScreenCapture.aLineBuffer[1]; } else { pOtherLineBuffer = &mScreenCapture.aLineBuffer[0]; }
	//
	// Line scan Idle?
	if ( pLineBuffer->ScanState == UISCNCAP__SS__SCAN_IDLE )
	{
		// Need to make sure the 'other' buffer is not being scanned and line count has not reached end!
		switch ( pOtherLineBuffer->ScanState )
		{
			case UISCNCAP__SS__SCAN_IDLE:
			case UISCNCAP__SS__UPLD_ACTV:
				if ( mScreenCapture.uiYLineCount < UISCNCAP__LINE_END )
				{
					pLineBuffer->ScanState = UISCNCAP__SS__SCAN_LINE;
					pLineBuffer->uiYPos    = mScreenCapture.uiYLineCount;
				}
				break;

			default:
				break;
		}
	}
	//
	// Scan the line?
	if ( pLineBuffer->ScanState == UISCNCAP__SS__SCAN_LINE )
	{
		if ( mScreenCapture.WaitForImageTiming.ulValue != 0 )
		{

		}
		else
		if ( UITFTDSP__ScreenShot_bStarted( pLineBuffer->uiYPos ) )
		{
			// Wait for capture to complete
			pLineBuffer->ScanState = UISCNCAP__SS__SCAN_WAIT;
		}
		else
		{
			// Error
			mScreenCapture.bIsValid = false;
			// Next state
			mScreenCapture.State = UISCNCAP__CS__COMPLETED;
		}
	}
	//
	// Wait busy cleared?
	if ( pLineBuffer->ScanState == UISCNCAP__SS__SCAN_WAIT )
	{
		// Check busy status
		if ( UITFTDSP__ScreenShot_bBusyStatusCheckValid( &bIsBusy ) )
		{
			if ( !bIsBusy ) { pLineBuffer->ScanState = UISCNCAP__SS__SCAN_READ; }
		}
	}
	//
	// Read RGB values of every pixel on line?
	if ( pLineBuffer->ScanState == UISCNCAP__SS__SCAN_READ )
	{
		if ( UITFTDSP__ScreenShot_bRGBDataRetrieved( &pLineBuffer->aucData[0] ) )
		{
			// Next line to be scanned
			mScreenCapture.uiYLineCount++;
			// Start delay timer
			GENTIMDT__StartTimer_TB_10MS( 5, &mScreenCapture.WaitForImageTiming );
			// Flag ready to upload current line
			pLineBuffer->ScanState = UISCNCAP__SS__UPLD_IDLE;
		}
		else
		{
			// Error
			mScreenCapture.bIsValid = false;
			// Next state
			mScreenCapture.State = UISCNCAP__CS__COMPLETED;
		}
	}
	//
	// Waiting for upload?
	if ( pLineBuffer->ScanState == UISCNCAP__SS__UPLD_IDLE )
	{
		// Need to make sure the 'other' buffer is not being uploaded!
		switch ( pOtherLineBuffer->ScanState )
		{
			case UISCNCAP__SS__UPLD_ACTV:
			case UISCNCAP__SS__UPLD_WAIT:
				// Wait till other finishes
				break;

			default:
				pLineBuffer->ScanState = UISCNCAP__SS__UPLD_WAIT;
				break;
		}
	}
	//
	// Waiting for upload?
	if ( pLineBuffer->ScanState == UISCNCAP__SS__UPLD_WAIT )
	{
	}
	//
	// Upload active
	if ( pLineBuffer->ScanState == UISCNCAP__SS__UPLD_ACTV )
	{
	}
	//
	// Upload done
	if ( pLineBuffer->ScanState == UISCNCAP__SS__UPLD_DONE )
	{
		// Next line
		if ( mScreenCapture.uiYLineCount >= UISCNCAP__LINE_END )
		{
			mScreenCapture.State = UISCNCAP__CS__COMPLETED;
		}
		else
		{
			pLineBuffer->ScanState = UISCNCAP__SS__SCAN_IDLE;
		}
	}

	return;
}
//*********************************************************************************************************************************************************************************
