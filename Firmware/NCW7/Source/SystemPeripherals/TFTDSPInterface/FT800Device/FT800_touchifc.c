//*********************************************************************************************************************************************************************************
// FT800_touchifc.c
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
#include		<GL050TN33_dispif.h>
#include		<FT800_touchifc.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<gentimdt.h>
#include		<gensavul.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef	enum	FT800_TOUCHIFC__TOUCHCAL_STATE {					// Touch Screen Calibration States
																	//
	FT800_TOUCHIFC__TCS_IDLE          		= 0,					// - Calibration idle
	FT800_TOUCHIFC__TCS_UPDATE_FT800_VALUES = 1,					// - Update FT800 with cal transform values
	FT800_TOUCHIFC__TCS_SAVE_NEW_CAL_VALUES = 2,					// - Retrieve from FT800 new cal transform values defined
	FT800_TOUCHIFC__TCS_ISSUE_CAL_CMD       = 3,					// - Issue cal command to FT800
	FT800_TOUCHIFC__TCS_WAIT_CAL_DONE       = 4						// - Continual check to see if cal command done

} FT800_TOUCHIFC__TOUCHCAL_STATE;

typedef	enum	FT800_TOUCHIFC__TOUCHCAL_STATUS {					// Touch Screen Calibration Status
																	//
	FT800_TOUCHIFC__TCSTS_IDLE     	  		= 0,					// - Idle
	FT800_TOUCHIFC__TCSTS_ACTV     	  		= 1,					// - Active
	FT800_TOUCHIFC__TCSTS_DONE 				= 2,					// - Done
	FT800_TOUCHIFC__TCSTS_DONE_SAVE_VALUES 	= 3,					// - Done and values need to be saved
	FT800_TOUCHIFC__TCSTS_DONE_ERROR_FATAL 	= 4						// - Done but fatal error occurred

} FT800_TOUCHIFC__TOUCHCAL_STATUS;

typedef	enum	FT800_TOUCHIFC__PRESS_STATE {						// Touch Press State
																	//
	FT800_TOUCHIFC__PS_NONE_PRESSED = 0,							// - No press
	FT800_TOUCHIFC__PS_WAIT_DEBONCE = 1,							// - Press detected : in debounce
	FT800_TOUCHIFC__PS_WAIT_PROCESS = 2,							// - Press detected : Wait for process
	FT800_TOUCHIFC__PS_WAIT_RELEASE = 3								// - Press detected : Wait for release

} FT800_TOUCHIFC__PRESS_STATE;

#define    FT800_TOUCHIFC__DEBOUNCE_TIME       			1			// Press debounce time - Based on 20ms GUI timebase
//
// Rapid Update Delay Times - Based on GUI timebase
const	uint8_t	FT800_TOUCHIFC__mkucTouchTagRapidInterval[] = {		// Index to this array using : mFT800Touch.Tag.Rapid.ucStepCount

	(24 / GENTIMDT__kucUI_TIMEBASE_10MS),
	(20 / GENTIMDT__kucUI_TIMEBASE_10MS),
	(14 / GENTIMDT__kucUI_TIMEBASE_10MS),
	(10 / GENTIMDT__kucUI_TIMEBASE_10MS),
	( 8 / GENTIMDT__kucUI_TIMEBASE_10MS),
	( 6 / GENTIMDT__kucUI_TIMEBASE_10MS),
	( 4 / GENTIMDT__kucUI_TIMEBASE_10MS),
	( 2 / GENTIMDT__kucUI_TIMEBASE_10MS)
};

#define			FT800_TOUCHIFC__MAX_RAPID_STEP_COUNT	8
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	FT800TouchCal {										// Touch Calibration Details
																	//
	FT800_TOUCHIFC__TOUCHCAL_STATE		State;						// - Touchscreen Calibration State
	FT800_TOUCHIFC__TOUCHCAL_STATUS		Status;						// - Touchscreen Calibration Status
	GENTIMDT__TimingCounter				WaitBeforeReadbackTiming;	// - Interval timing
	uint32_t							ulTransformA;				// - Cal Transform A (Saved in nvm)
	uint32_t							ulTransformB;				// - Cal Transform B (Saved in nvm)
	uint32_t							ulTransformC;				// - Cal Transform C (Saved in nvm)
	uint32_t							ulTransformD;				// - Cal Transform D (Saved in nvm)
	uint32_t							ulTransformE;				// - Cal Tramsform E (Saved in nvm)
	uint32_t							ulTransformF;				// - Cal Transform F (Saved in nvm)

} FT800TouchCal;

typedef struct	FT800TouchTagRapid {								// Touch Tag Rapid Processing Details
																	//
	GENTIMDT__TimingCounter				IntervalTiming;				// - Interval timing
	uint8_t								ucStepCount;				// - Step count defines interval time
	bool								bAllowProcess;				// - Flag when recorded touch to be processed

} FT800TouchTagRapid;

typedef struct	FT800TouchTag {										// Touch Tag Processing Details
																	//
	FT800_TOUCHIFC__PRESS_STATE			State;						// - State of touch tag
	uint8_t								ucMask;						// - Mask value for tag setting
	uint8_t								ucBeingProcessed;			// - Tag being processed
	uint8_t								ucProcess;					// - Tag to process
	uint8_t								ucPressed;					// - Tag pressed
	GENTIMDT__TimingCounter				DebounceTiming;				// - Debounce time
																	//
	FT800_TOUCHIFC__FT800TouchTagValue	Value;						// - Value control details
	FT800TouchTagRapid					Rapid;						// - Rapid control details

} FT800TouchTag;

typedef struct	FT800Touch {										// Touch Details
																	//
	uint16_t							uiDisplayHeight;			// - Display height for initial cal message
	uint16_t							uiDisplayWidth;				// - Display width for initial cal message
																	//
	FT800TouchCal						Cal;						// - Cal control
	FT800TouchTag						Tag;						// - Tag control
																	//
	bool								bPressDetected;				// - Touch press detected
																	//
} FT800Touch;														//
																	//
static FT800Touch	mFT800Touch;									// Container
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformA = { 0, &mFT800Touch.Cal.ulTransformA };
const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformB = { 0, &mFT800Touch.Cal.ulTransformB };
const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformC = { 0, &mFT800Touch.Cal.ulTransformC };
const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformD = { 0, &mFT800Touch.Cal.ulTransformD };
const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformE = { 0, &mFT800Touch.Cal.ulTransformE };
const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformF = { 0, &mFT800Touch.Cal.ulTransformF };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	FT800_TOUCHIFC__kTouchTagDebounceTiming 		  = { &mFT800Touch.Tag.DebounceTiming			};
const struct	GENTIMDT__Timing	FT800_TOUCHIFC__kTouchTagRapidIntervalTiming 	  = { &mFT800Touch.Tag.Rapid.IntervalTiming 	};
const struct	GENTIMDT__Timing	FT800_TOUCHIFC__kTouchCalWaitBeforeReadbackTiming = { &mFT800Touch.Cal.WaitBeforeReadbackTiming };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void							CalibrationUpdate(void);
static void							CalibrationUpdate_PerformCalProcess(void);
static void							CalibrationUpdate_TransformValues_Wrte(void);
static void							CalibrationUpdate_TransformValues_Read(void);

static void							UpdateTouchPressDetectedFlag(void);
static void							TagUpdate(void);
static bool							bAllowRapidUpdate(void);

static FT800_DEVICEIF__XFER_STATUS	TransformValues_Wrte(void);
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
 * @brief:	Touch interface initialisation.
 * @param:	vuiDisplayHeight - The display height.
 * 			vuiDisplayWidth  - The display width.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__Init(uint16_t vuiDisplayHeight, uint16_t vuiDisplayWidth)
{
	// Initialisations
	// - Display height/width
	mFT800Touch.uiDisplayHeight 	 	= vuiDisplayHeight;
	mFT800Touch.uiDisplayWidth  	 	= vuiDisplayWidth;
	mFT800Touch.bPressDetected 			= false;
	// - Touch Cal
	mFT800Touch.Cal.State 	    	 	= FT800_TOUCHIFC__TCS_IDLE;
	mFT800Touch.Cal.Status      	 	= FT800_TOUCHIFC__TCSTS_IDLE;
	// - Touch Tag
	mFT800Touch.Tag.ucBeingProcessed 	= 0;
	mFT800Touch.Tag.ucPressed   	 	= 0;
	mFT800Touch.Tag.ucProcess   	 	= 0;
	mFT800Touch.Tag.ucMask   	 	    = 1;
	mFT800Touch.Tag.State       	 	= FT800_TOUCHIFC__PS_NONE_PRESSED;
	// - Rapid tag processing
	mFT800Touch.Tag.Rapid.bAllowProcess = false;
	// - Rapid tag value init
	FT800_TOUCHIFC__RapidUpdateDataInit();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises the touch tag value details for rapid update processing - The initial values result in no rapid update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__RapidUpdateDataInit(void)
{
	FT800_TOUCHIFC__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled   = false;
	TouchTagValue.bIncreasing 		   = false;
	TouchTagValue.ucMinimum 		   = 0;
	TouchTagValue.ucMaximum 		   = 0;
	TouchTagValue.ucActual 		 	   = 0;

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Provides application level access to define the value details to permit rapid value update.
 * @param:	pTouchTagValue - Pointer to the touch tag value details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__RapidUpdateDataDefine(FT800_TOUCHIFC__FT800TouchTagValue* pTouchTagValue)
{
	mFT800Touch.Tag.Value.bWrapAroundEnabled = pTouchTagValue->bWrapAroundEnabled;
	mFT800Touch.Tag.Value.bIncreasing 		 = pTouchTagValue->bIncreasing;
	mFT800Touch.Tag.Value.ucMinimum 		 = pTouchTagValue->ucMinimum;
	mFT800Touch.Tag.Value.ucMaximum 		 = pTouchTagValue->ucMaximum;
	mFT800Touch.Tag.Value.ucActual 		 	 = pTouchTagValue->ucActual;

	if ( ( mFT800Touch.Tag.Value.ucMinimum == 0 ) && ( mFT800Touch.Tag.Value.ucMaximum == 0 ) )
	{
		mFT800Touch.Tag.Value.bAllowForRapidUpdate = false;
	}
	else
	{
		mFT800Touch.Tag.Value.bAllowForRapidUpdate = true;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Touch interface update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__Update(void)
{
	if ( mFT800Touch.Cal.State != FT800_TOUCHIFC__TCS_IDLE ) { CalibrationUpdate(); }

	if ( mFT800Touch.Cal.State == FT800_TOUCHIFC__TCS_IDLE )
	{
		switch ( mFT800Touch.Cal.Status )		// Only allow touch processing after touch calibrated
		{
			case FT800_TOUCHIFC__TCSTS_DONE_SAVE_VALUES:
			case FT800_TOUCHIFC__TCSTS_DONE:
				UpdateTouchPressDetectedFlag();			// Added here for the one access
				TagUpdate();
				break;

			default:
				break;
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Touch interface tag that requires processing.
 * @param:	None.
 * @retval:	uint8_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							FT800_TOUCHIFC__ucTagToProcess(void)
{
	return mFT800Touch.Tag.ucProcess;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Feedback indicating tag has been processed.  Must be called to release touch state machine.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__FlagTagProcessed(void)
{
	if ( mFT800Touch.Tag.State == FT800_TOUCHIFC__PS_WAIT_PROCESS )
	{
		// Start interval counter in event rapid update enabled
		GENTIMDT__StartTimer_TB_UIBased( FT800_TOUCHIFC__mkucTouchTagRapidInterval[mFT800Touch.Tag.Rapid.ucStepCount], &mFT800Touch.Tag.Rapid.IntervalTiming );
		// Wait for release
		mFT800Touch.Tag.State = FT800_TOUCHIFC__PS_WAIT_RELEASE;
	}
	else
	if ( mFT800Touch.Tag.State == FT800_TOUCHIFC__PS_WAIT_RELEASE )
	{
		mFT800Touch.Tag.Rapid.bAllowProcess = false;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks for a touch press being detected somewhere on the screen returning true if so.
 * 			The RZ value is retrieved and compared against the threshokd defined for the touch panel.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_TOUCHIFC__bTouchPressDetected(void)
{
	return mFT800Touch.bPressDetected;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags when a touch press has been detected
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__FlagTouchPressDetected(void)
{
	mFT800Touch.bPressDetected = true;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags whether or not touch tag values can be assigned.
 * @param:	vbAllow - Flag to allow/disallow touch tag values.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__SetTagMask(bool vbAllow)
{
	if ( vbAllow ) { mFT800Touch.Tag.ucMask = 1; } else { mFT800Touch.Tag.ucMask = 0; }
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the current tag mask value.
 * @param:	None.
 * @retval:	0 or 1.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							FT800_TOUCHIFC__ucTagMask(void)
{
	return mFT800Touch.Tag.ucMask;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Begin touch screen initialisation process.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__CalInit(void)
{
	if ( mFT800Touch.Cal.ulTransformA == 0 )
	{
		// Cal values retrieved from eeprom no defined
		// --> User needs to cal the touch screen
		mFT800Touch.Cal.State = FT800_TOUCHIFC__TCS_ISSUE_CAL_CMD;
	}
	else
	{
		// Cal values retrieved from eeprom already defined
		// --> Just write these values to the FT800
		mFT800Touch.Cal.State = FT800_TOUCHIFC__TCS_UPDATE_FT800_VALUES;
	}
	// Init status - Active
	mFT800Touch.Cal.Status = FT800_TOUCHIFC__TCSTS_ACTV;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Redo the touch screen initialisation process.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__CalRedo(void)
{
	// Reset all transform values
	FT800_TOUCHIFC__ResetTransformValues();
	// Force cal
	FT800_TOUCHIFC__CalInit();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates cal process completed and that transform values need to be saved.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_TOUCHIFC__bCalDoneValidSaveValues(void)
{
	return (bool)( mFT800Touch.Cal.Status == FT800_TOUCHIFC__TCSTS_DONE_SAVE_VALUES );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates cal process completed but fatal error occurred.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_TOUCHIFC__bCalDoneFatal(void)
{
	return (bool)( mFT800Touch.Cal.Status == FT800_TOUCHIFC__TCSTS_DONE_ERROR_FATAL );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates cal process completed.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							FT800_TOUCHIFC__bCalDoneValid(void)
{
	return (bool)( mFT800Touch.Cal.Status == FT800_TOUCHIFC__TCSTS_DONE );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reset all touch screen transform values.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_TOUCHIFC__ResetTransformValues(void)
{
	// Reset all transform values
	mFT800Touch.Cal.ulTransformA = 0;
	mFT800Touch.Cal.ulTransformB = 0;
	mFT800Touch.Cal.ulTransformC = 0;
	mFT800Touch.Cal.ulTransformD = 0;
	mFT800Touch.Cal.ulTransformE = 0;
	mFT800Touch.Cal.ulTransformF = 0;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Transform values A-F..
 * @param:	None.
 * @retval:	uint32_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_TOUCHIFC__ulTransformAValue(void)
{
	return mFT800Touch.Cal.ulTransformA;
}
uint32_t						FT800_TOUCHIFC__ulTransformBValue(void)
{
	return mFT800Touch.Cal.ulTransformB;
}
uint32_t						FT800_TOUCHIFC__ulTransformCValue(void)
{
	return mFT800Touch.Cal.ulTransformC;
}
uint32_t						FT800_TOUCHIFC__ulTransformDValue(void)
{
	return mFT800Touch.Cal.ulTransformD;
}
uint32_t						FT800_TOUCHIFC__ulTransformEValue(void)
{
	return mFT800Touch.Cal.ulTransformE;
}
uint32_t						FT800_TOUCHIFC__ulTransformFValue(void)
{
	return mFT800Touch.Cal.ulTransformF;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks whether a calibration of the touch screen has to be performed.  If touch screen cal needs to be done the calibrate command is executed with the transform
 *			a to f values generated retrieved from the FT800 and kept in module level variables.  If touch screen cal values already loaded from eeprom then these are saved
 *			to the FT800 registers.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							CalibrationUpdate(void)
{
	// Process cal state
	switch ( mFT800Touch.Cal.State )
	{
		case FT800_TOUCHIFC__TCS_UPDATE_FT800_VALUES:
			CalibrationUpdate_TransformValues_Wrte();
			break;

		case FT800_TOUCHIFC__TCS_SAVE_NEW_CAL_VALUES:
			CalibrationUpdate_TransformValues_Read();
			break;

		case FT800_TOUCHIFC__TCS_ISSUE_CAL_CMD:
		case FT800_TOUCHIFC__TCS_WAIT_CAL_DONE:
			CalibrationUpdate_PerformCalProcess();
			break;

		default:
			break;
	}

	if (mFT800Touch.Cal.Status != FT800_TOUCHIFC__TCSTS_ACTV)
	{
		mFT800Touch.Cal.State = FT800_TOUCHIFC__TCS_IDLE;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Performs the calibration process waiting for a response from the FT800.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							CalibrationUpdate_PerformCalProcess(void)
{
	bool		bFIFOIsEmpty = false;

	if ( mFT800Touch.Cal.State == FT800_TOUCHIFC__TCS_ISSUE_CAL_CMD )
	{
		// TFT Touch Calibration Command Sequence
		// - Queue and then xfer
		FT800_COPRCCMD__CPCmd__DefineAccessType( FT800_COPRCCMD__BAT_QUEUE_XFER );
		// - Start Frame
		FT800_COPRCCMD__CPCmdQueue__CMD_DLSTART();
		// - Set colors
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB( 64, 64, 64 ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR( 1, 1, 1 ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__COLOR_RGB( 0xff, 0xff, 0xff ) );
		// - Show text in middle of screen : 'Please Tap on the dot'
		FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( ( mFT800Touch.uiDisplayWidth / 2 ), ( mFT800Touch.uiDisplayHeight / 2 ), 27, FT800_COPRCCMD__OPTION_MASK__OPT_CENTER, "Please tap on the dot" );
		// - Perform cal procedure
		FT800_COPRCCMD__CPCmdQueue__CMD_CALIBRATE();
		//
		// Transfer queued data out
		switch ( FT800_COPRCCMD__CPCmd__IssueQueue( FUNC_LOOP ) )
		{
			case FT800_DEVICEIF__XS_DONE_VALID:		// Valid xfer - Wait for cal to be done
				mFT800Touch.Cal.State = FT800_TOUCHIFC__TCS_WAIT_CAL_DONE;
				break;

			case FT800_DEVICEIF__XS_DONE_ERROR:		// Error xfer - Flag that cal failed
				mFT800Touch.Cal.Status = FT800_TOUCHIFC__TCSTS_DONE_ERROR_FATAL;
				break;

			default:
				break;
		}
	}
	else
	if ( FT800_COPRCCMD__CheckCPFIFOEmpty( FUNC_PASS, &bFIFOIsEmpty ) == FT800_DEVICEIF__XS_DONE_VALID )
	{
		if ( bFIFOIsEmpty )
		{
			// Just clear the display
			// - Queue and then xfer
			FT800_COPRCCMD__CPCmd__DefineAccessType( FT800_COPRCCMD__BAT_QUEUE_XFER );
			// - Start Frame
			FT800_COPRCCMD__CPCmdQueue__CMD_DLSTART();
			// - Set color
			FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB( 0, 0, 0 ) );
			FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR( 1, 1, 1 ) );
			// - Swap it
			FT800_COPRCCMD__CPCmdQueue__CMD_SWAP( FT800_DISPLIST__DL_SWAP_TYPE_FRAME );
			// - Issue the queue
			FT800_COPRCCMD__CPCmd__IssueQueue( FUNC_LOOP );
			// - Wait for FT800 to do calc - about 1000ms
			GENTIMDT__StartTimer_TB_UIBased_1SEC( 1, &mFT800Touch.Cal.WaitBeforeReadbackTiming );
			//
			// Next state
			mFT800Touch.Cal.State = FT800_TOUCHIFC__TCS_SAVE_NEW_CAL_VALUES;
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the FT800 with the touch cal transform values retrieved from eeprom.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							CalibrationUpdate_TransformValues_Wrte(void)
{
	FT800_DEVICEIF__XFER_STATUS XferStatus = TransformValues_Wrte();

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		mFT800Touch.Cal.Status = FT800_TOUCHIFC__TCSTS_DONE;
	}
	else
	{
		mFT800Touch.Cal.Status = FT800_TOUCHIFC__TCSTS_DONE_ERROR_FATAL;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Retrieves from the FT800 the touch cal transform values.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							CalibrationUpdate_TransformValues_Read(void)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus = FT800_DEVICEIF__XS_DONE_VALID;

	if ( mFT800Touch.Cal.WaitBeforeReadbackTiming.ulValue != 0 ) { return; }
	//
	// Touchscreen cal values just set - Read from FT800 and save to eeprom
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Rd32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_A, &mFT800Touch.Cal.ulTransformA ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Rd32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_B, &mFT800Touch.Cal.ulTransformB ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Rd32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_C, &mFT800Touch.Cal.ulTransformC ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Rd32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_D, &mFT800Touch.Cal.ulTransformD ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Rd32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_E, &mFT800Touch.Cal.ulTransformE ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Rd32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_F, &mFT800Touch.Cal.ulTransformF ); }

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		mFT800Touch.Cal.Status = FT800_TOUCHIFC__TCSTS_DONE_SAVE_VALUES;
	}
	else
	{
		mFT800Touch.Cal.Status = FT800_TOUCHIFC__TCSTS_DONE;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks for a touch press being detected somewhere on the screen setting module flag true if so.
 * 			The RZ value is retrieved and compared against the threshokd defined for the touch panel.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							UpdateTouchPressDetectedFlag(void)
{
//#define			TOUCH_USE_RZ
#ifdef		TOUCH_USE_RZ
	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	uint16_t					uiValue;

	XferStatus = FT800_DEVICEIF__Rd16(FUNC_LOOP, FT800__REG_TOUCH_RZ, &uiValue);

	if (XferStatus != FT800_DEVICEIF__XS_DONE_VALID)
	{
		mFT800Touch.bPressDetected = false;
	}
	else
	if (uiValue >= GL050TN33_DISPIF__TOUCH_RZTHRESH)
	{
		mFT800Touch.bPressDetected = false;
	}
	else
	{
		mFT800Touch.bPressDetected = true;
	}
#else
	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	uint32_t					ulValue;
	//
	// Refresh Cal values in FT800, emilio 27-10-15
	TransformValues_Wrte();

	XferStatus = FT800_DEVICEIF__Rd32( FUNC_LOOP, FT800__REG_TOUCH_RAW_XY, &ulValue );

	if ( XferStatus != FT800_DEVICEIF__XS_DONE_VALID )
	{
		mFT800Touch.bPressDetected = false;
	}
	else
	if ( ulValue == (uint32_t)0xFFFFFFFFL )
	{
		mFT800Touch.bPressDetected = false;
	}
	else
	{
		mFT800Touch.bPressDetected = true;
	}
#endif
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates touch tag detection.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							TagUpdate(void)
{
	uint8_t		ucTouchTag;
	//
	// Read the touch tag register
	FT800_DEVICEIF__Rd08( FUNC_LOOP, FT800__REG_TOUCH_TAG, &ucTouchTag );
	//
	// Process state
	if ( mFT800Touch.Tag.State == FT800_TOUCHIFC__PS_WAIT_DEBONCE )
	{
		if ( ucTouchTag != mFT800Touch.Tag.ucPressed )
		{
			if ( ucTouchTag != 0 )
			{
				// Change in tag being pressed - Remember it and start debounce timer
	        	mFT800Touch.Tag.ucPressed = ucTouchTag;	GENTIMDT__StartTimer_TB_UIBased( FT800_TOUCHIFC__DEBOUNCE_TIME, &mFT800Touch.Tag.DebounceTiming );  // 50ms debounce timer
	        	// Wait for debounce time
	    		mFT800Touch.Tag.State = FT800_TOUCHIFC__PS_WAIT_DEBONCE;
			}
			else
			{
				// No tag so flag none pressed
				mFT800Touch.Tag.State = FT800_TOUCHIFC__PS_NONE_PRESSED;
			}
		}
		else
		if ( mFT800Touch.Tag.DebounceTiming.ulValue == 0 )
		{
			// Debounce time up
			// - Reset step count
			mFT800Touch.Tag.Rapid.ucStepCount = 0;
			// - Flag to process tag
			mFT800Touch.Tag.State = FT800_TOUCHIFC__PS_WAIT_PROCESS;
		}
	}
	else
	if ( mFT800Touch.Tag.State == FT800_TOUCHIFC__PS_WAIT_PROCESS )
	{
		if ( ucTouchTag != mFT800Touch.Tag.ucPressed )
		{
			// Change in tag being pressed - Remember it and start debounce timer
        	mFT800Touch.Tag.ucPressed = ucTouchTag;	GENTIMDT__StartTimer_TB_UIBased( FT800_TOUCHIFC__DEBOUNCE_TIME, &mFT800Touch.Tag.DebounceTiming );  // 50ms debounce timer
        	// Wait for debounce time
    		mFT800Touch.Tag.State = FT800_TOUCHIFC__PS_WAIT_DEBONCE;
		}
	}
	else
	if ( mFT800Touch.Tag.State == FT800_TOUCHIFC__PS_WAIT_RELEASE )
	{
		if ( ucTouchTag == 0 )
		{
			mFT800Touch.Tag.ucPressed = 0; mFT800Touch.Tag.State = FT800_TOUCHIFC__PS_NONE_PRESSED;
		}
		else
		if ( mFT800Touch.Tag.ucBeingProcessed == ucTouchTag )
		{
			// Tag that was being pressed is still being pressed - Check if rapid update required
			if ( bAllowRapidUpdate() )
			{
				mFT800Touch.Tag.Rapid.bAllowProcess = true; mFT800Touch.Tag.ucProcess = mFT800Touch.Tag.ucBeingProcessed;
			}
			else
			{
				mFT800Touch.Tag.Rapid.bAllowProcess = false;
			}
		}
	}
	else
	{
		if ( ucTouchTag != 0 )
		{
			// Tag press detected - Remember it and start debounce timer
        	mFT800Touch.Tag.ucPressed = ucTouchTag;	GENTIMDT__StartTimer_TB_UIBased( FT800_TOUCHIFC__DEBOUNCE_TIME, &mFT800Touch.Tag.DebounceTiming );  // 50ms debounce timer
        	// Wait for debounce time
    		mFT800Touch.Tag.State = FT800_TOUCHIFC__PS_WAIT_DEBONCE;
		}
	}
	//
	// Define variable that is used by application
	if ( mFT800Touch.Tag.State == FT800_TOUCHIFC__PS_WAIT_PROCESS )
	{
		mFT800Touch.Tag.ucProcess = mFT800Touch.Tag.ucBeingProcessed = mFT800Touch.Tag.ucPressed;
	}
	else
	if ( mFT800Touch.Tag.Rapid.bAllowProcess )
	{
		mFT800Touch.Tag.ucProcess = mFT800Touch.Tag.ucBeingProcessed;
	}
	else
	{
		mFT800Touch.Tag.ucProcess = 0;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks if rapid update required returning true if needed.  The data within the 'mFT800Touch.Tag.Value' structure must be defined for the check to be performed.  This
 * 			data can be set using the function:
 *
 * 					FT800_TOUCHIFC__RapidUpdateDataDefine().
 *
 * 			If no rapid update required just call the function:
 *
 * 					FT800_TOUCHIFC__RapidUpdateDataInit().
 *
 * @param:	None:
 * @retval:	BOOLRAN.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static bool							bAllowRapidUpdate(void)
{
	uint8_t		ucDifference;
	bool		bIncreaseSpeed, bAllowUpdate = false;

	if ( !mFT800Touch.Tag.Value.bAllowForRapidUpdate )
	{
		return bAllowUpdate;
	}
	//
	// If here - Rapid update allowed!
	//
	if ( mFT800Touch.Tag.Value.bWrapAroundEnabled )
	{
		// When wrap around enabled allow rapid update always
		bIncreaseSpeed = true;
	}
	else
	{
		// Check value against limits to determine difference
		if ( mFT800Touch.Tag.Value.bIncreasing )
		{
			if ( mFT800Touch.Tag.Value.ucMaximum > mFT800Touch.Tag.Value.ucActual )
			{
				ucDifference = mFT800Touch.Tag.Value.ucMaximum - mFT800Touch.Tag.Value.ucActual;
			}
			else
			{
				ucDifference = mFT800Touch.Tag.Value.ucActual - mFT800Touch.Tag.Value.ucMaximum;
			}
		}
		else
		{
			if ( mFT800Touch.Tag.Value.ucMinimum > mFT800Touch.Tag.Value.ucActual )
			{
				ucDifference = mFT800Touch.Tag.Value.ucMinimum - mFT800Touch.Tag.Value.ucActual;
			}
			else
			{
				ucDifference = mFT800Touch.Tag.Value.ucActual - mFT800Touch.Tag.Value.ucMinimum;
			}
		}

		bIncreaseSpeed = (ucDifference >= FT800_TOUCHIFC__MAX_RAPID_STEP_COUNT);
	}

	if ( mFT800Touch.Tag.Rapid.IntervalTiming.ulValue == 0 )
	{
		if ( bIncreaseSpeed )
		{
			if ( mFT800Touch.Tag.Rapid.ucStepCount < ( FT800_TOUCHIFC__MAX_RAPID_STEP_COUNT-1 ) )
			{
				mFT800Touch.Tag.Rapid.ucStepCount++;
			}
		}
		else
		{
			if ( mFT800Touch.Tag.Rapid.ucStepCount > 0 )
			{
				mFT800Touch.Tag.Rapid.ucStepCount--;
			}
		}

		GENTIMDT__StartTimer_TB_UIBased( FT800_TOUCHIFC__mkucTouchTagRapidInterval[mFT800Touch.Tag.Rapid.ucStepCount], &mFT800Touch.Tag.Rapid.IntervalTiming );

		bAllowUpdate = true;
	}

	return bAllowUpdate;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes cal transform values to FT800.
 * @param:	None.
 * @retval:	FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static FT800_DEVICEIF__XFER_STATUS	TransformValues_Wrte(void)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus = FT800_DEVICEIF__XS_DONE_VALID;

	// Only write values when non-zro, emilio 05-04-17
	if ( mFT800Touch.Cal.ulTransformA == 0 ) { return XferStatus; }
	if ( mFT800Touch.Cal.ulTransformB == 0 ) { return XferStatus; }
	if ( mFT800Touch.Cal.ulTransformC == 0 ) { return XferStatus; }
	if ( mFT800Touch.Cal.ulTransformD == 0 ) { return XferStatus; }
	if ( mFT800Touch.Cal.ulTransformE == 0 ) { return XferStatus; }
	if ( mFT800Touch.Cal.ulTransformF == 0 ) { return XferStatus; }
	//
	// Touchscreen cal values already defined - Just update the FT800
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Wr32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_A, mFT800Touch.Cal.ulTransformA ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Wr32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_B, mFT800Touch.Cal.ulTransformB ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Wr32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_C, mFT800Touch.Cal.ulTransformC ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Wr32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_D, mFT800Touch.Cal.ulTransformD ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Wr32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_E, mFT800Touch.Cal.ulTransformE ); }
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID ) { XferStatus = FT800_DEVICEIF__Wr32( FUNC_LOOP, FT800__REG_TOUCH_TRANSFORM_F, mFT800Touch.Cal.ulTransformF ); }

	return XferStatus;
}
//*********************************************************************************************************************************************************************************
