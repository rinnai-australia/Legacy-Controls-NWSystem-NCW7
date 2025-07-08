//*********************************************************************************************************************************************************************************
// uitftdsp.c
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
#include		<uitftdsp.h>
#include		<uitftdsp_button.h>
#include		<uitftdsp_image.h>
#include        <uitftdsp_shape.h>
#include		<uitftdsp_color.h>
#include		<tftdspif.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<gentimdt.h>
#include		<genprmdt.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			UITFTDSP__BL_CONTROL_RAMP_UP_INCREMENT				8
#define			UITFTDSP__BL_CONTROL_RAMP_UP_INTERVAL_UITICK		1

#define			UITFTDSP__BL_CONTROL_RAMP_DN_DECREMENT				2
#define			UITFTDSP__BL_CONTROL_RAMP_DN_INTERVAL_UITICK		2
//
// Backlight Levels
// - Dimmed
#define			UITFTDSP__DIMMED_BL_LEVEL							10
// - Active min
#define			UITFTDSP__ACTIVE_BL_LEVEL_MIN						29
// - Active max
#define			UITFTDSP__ACTIVE_BL_LEVEL_MAX						128
//
// Backlight Control
typedef enum	UITFTDSP__BACKLIGHT_STATE {				// Backlight state
														//
	UITFTDSP__BS__WAIT_STARTUP 	  		= 0,			// - Wait for startup to complete
	UITFTDSP__BS__OFF          	  		= 1,			// - Off
	UITFTDSP__BS__RAMP_UP      	 		= 2,			// - Ramping up
	UITFTDSP__BS__ON           	  		= 3,			// - On
	UITFTDSP__BS__RAMP_DOWN          	= 4,			// - Ramping down
	UITFTDSP__BS__DISPLAY_SHUTDOWN   	= 5,			// - Display shutdown

} UITFTDSP__BACKLIGHT_STATE;
//
// Flash control time intervals assuming 10ms timebase
static const uint8_t	mkucFlashControlTimeInterval[] = {

	( 100 / GENTIMDT__kucUI_TIMEBASE_10MS ),
	(  50 / GENTIMDT__kucUI_TIMEBASE_10MS ),
	(  24 / GENTIMDT__kucUI_TIMEBASE_10MS )
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	TFTDisplayFlashControl {												// TFT Flash control
																						//
	bool	     						bShowItem;										// - Flag indicating to display item
	GENTIMDT__TimingCounter				TimingCounter;									// - Timing counter for flashing

} TFTDisplayFlashControl;

typedef struct	TFTDisplayBacklightControl {											// TFT Display backlight control
																						//
	UITFTDSP__BACKLIGHT_STATE			State;											// - Current state
																						//
	uint8_t								ucRequiredLevel;								// - Level required to maintain at
	uint8_t								ucOnFullLevel;									// - Level when switched on full
	uint8_t								ucDimmedLevel;									// - Level when switched on dimmed
																						//
	uint8_t								ucOnFullTimeoutInterval;						// - Timeout interval when backlight on full (secs)
	uint8_t								ucDimmedTimeoutInterval;						// - Timeout interval when backlight dimmed (secs)
	GENTIMDT__TimingCounter				OnTimeoutTimingCounter;							// - Backlight on timeout
																						//
	GENTIMDT__TimingCounter				RampUpDownTimingCounter;						// - Ramp up/down timing counter
	uint8_t								ucRampLevel;									// - Level to hold while ramping up/down
																						//
	uint8_t								ucOperatingLevel;								// - Level to operate backlight at
	bool								bKeepAtFullBrightness;							// - Flag to keep at full brightness
																						//
	bool								bShutdownIsActive;								// - Shutdown active flag, emilio 03-07-17

} TFTDisplayBacklightControl;

typedef struct	TFTDisplayControl {														// Display Control
																						//
	TFTDisplayBacklightControl		Backlight;											// - Backlight control
	TFTDisplayFlashControl			Flash[UI_TFTDISPLAY__MAX_FLASH_INTERVALS];			// - Flash control
																						//
} TFTDisplayControl;																	//
																						//
static TFTDisplayControl	mTFTDisplayControl;											// Container
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENPRMDT__ParameterValue		UITFTDSP__kTFTControlBacklightOnFullTimeoutInterval = { 30, 10, 60, 1, &mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval };		// User Setting
const struct	GENPRMDT__ParameterValue		UITFTDSP__kTFTControlBacklightDimmedTimeoutInterval = { 30, 10, 60, 1, &mTFTDisplayControl.Backlight.ucDimmedTimeoutInterval };		// User Setting
const struct	GENPRMDT__ParameterValue		UITFTDSP__kTFTControlBacklightOnFullLevel 			= {  10, 1, 10, 1, &mTFTDisplayControl.Backlight.ucOnFullLevel			 };		// User Setting
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	UITFTDSP__kTFTControlBacklightOnTimeoutTiming  = { &mTFTDisplayControl.Backlight.OnTimeoutTimingCounter 		};
const struct	GENTIMDT__Timing	UITFTDSP__kTFTControlBacklightRampUpDownTiming = { &mTFTDisplayControl.Backlight.RampUpDownTimingCounter		};
const struct	GENTIMDT__Timing	UITFTDSP__kTFTControlFlash1HzTiming 		   = { &mTFTDisplayControl.Flash[UITFTDSP__FR__1HZ].TimingCounter	};
const struct	GENTIMDT__Timing	UITFTDSP__kTFTControlFlash2HzTiming 		   = { &mTFTDisplayControl.Flash[UITFTDSP__FR__2HZ].TimingCounter	};
const struct	GENTIMDT__Timing	UITFTDSP__kTFTControlFlash4HzTiming 		   = { &mTFTDisplayControl.Flash[UITFTDSP__FR__4HZ].TimingCounter	};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void					BacklightControl_InitRampUp(void);
static void					BacklightControl_InitRampDown(void);
static uint8_t				ucActiveBacklightLevel(void);
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
 * @brief:	TFT display initialisation.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__Initialise(void)
{
	uint8_t		ucIndex;
	//
	// Flash Control
	for ( ucIndex = UITFTDSP__FR__1HZ; ucIndex < UI_TFTDISPLAY__MAX_FLASH_INTERVALS; ucIndex++ )
	{
		// Clear show flag
		mTFTDisplayControl.Flash[ucIndex].bShowItem = false;
		// Start timers
		GENTIMDT__StartTimer_TB_UIBased( mkucFlashControlTimeInterval[ucIndex], &mTFTDisplayControl.Flash[ucIndex].TimingCounter );
	}
	//
	// Backlight Control
	// - Default values
	mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval = UITFTDSP__kTFTControlBacklightOnFullTimeoutInterval.ucDefault;
	mTFTDisplayControl.Backlight.ucDimmedTimeoutInterval = UITFTDSP__kTFTControlBacklightDimmedTimeoutInterval.ucDefault;
	mTFTDisplayControl.Backlight.ucOnFullLevel		     = UITFTDSP__kTFTControlBacklightOnFullLevel.ucDefault;
	// - Startup state
	mTFTDisplayControl.Backlight.State             = UITFTDSP__BS__WAIT_STARTUP;
	mTFTDisplayControl.Backlight.bShutdownIsActive = false;
	//
	// Image system init
	UITFTDSP_IMAGE__Initialise();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Clears backlight shutdown flag to wake display up.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__BacklightOperation_Wakeup(void)
{
	mTFTDisplayControl.Backlight.bShutdownIsActive = false;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates whether the display shutdown is active.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UITFTDSP__bDisplayShutdownIsActive(void)
{
	return mTFTDisplayControl.Backlight.bShutdownIsActive;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates flash control info for the TFT display.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__FlashControl_Update(void)
{
	if ( mTFTDisplayControl.Flash[UITFTDSP__FR__1HZ].TimingCounter.ulValue == 0 )
	{
		mTFTDisplayControl.Flash[UITFTDSP__FR__1HZ].bShowItem = !mTFTDisplayControl.Flash[UITFTDSP__FR__1HZ].bShowItem;

		GENTIMDT__StartTimer_TB_UIBased( mkucFlashControlTimeInterval[UITFTDSP__FR__1HZ], &mTFTDisplayControl.Flash[UITFTDSP__FR__1HZ].TimingCounter );
	}

	if ( mTFTDisplayControl.Flash[UITFTDSP__FR__2HZ].TimingCounter.ulValue == 0 )
	{
		mTFTDisplayControl.Flash[UITFTDSP__FR__2HZ].bShowItem = !mTFTDisplayControl.Flash[UITFTDSP__FR__2HZ].bShowItem;

		GENTIMDT__StartTimer_TB_UIBased( mkucFlashControlTimeInterval[UITFTDSP__FR__2HZ], &mTFTDisplayControl.Flash[UITFTDSP__FR__2HZ].TimingCounter );
	}

	if ( mTFTDisplayControl.Flash[UITFTDSP__FR__4HZ].TimingCounter.ulValue == 0 )
	{
		mTFTDisplayControl.Flash[UITFTDSP__FR__4HZ].bShowItem = !mTFTDisplayControl.Flash[UITFTDSP__FR__4HZ].bShowItem;

		GENTIMDT__StartTimer_TB_UIBased( mkucFlashControlTimeInterval[UITFTDSP__FR__4HZ], &mTFTDisplayControl.Flash[UITFTDSP__FR__4HZ].TimingCounter );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Touch control update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__TouchControl_Update(void)
{
	TFTDSPIF__TouchUpdate();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Backlight control update.
 * @param:	vbLoPowerMode    - Operating in low power mode.
 * 			vbTestMode	     - Test mode active
 * 			vucTestModeLevel - Backlight level in test mode.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__BacklightControl_Update(bool vbLoPowerMode, bool vbTestMode, uint8_t vucTestModeLevel)
{
	// Check if invalid
	if ( mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval == 0 ) { mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval = 30; }
	if ( mTFTDisplayControl.Backlight.ucDimmedTimeoutInterval == 0 ) { mTFTDisplayControl.Backlight.ucDimmedTimeoutInterval = 30; }
	if ( mTFTDisplayControl.Backlight.ucOnFullLevel == 0 ) 			 { mTFTDisplayControl.Backlight.ucOnFullLevel = 10; 		  }

	switch ( mTFTDisplayControl.Backlight.State )
	{
		case UITFTDSP__BS__WAIT_STARTUP:
			// Wait for start up to complete - Once here startup has completed!
			// - Start timeout timer
			GENTIMDT__StartTimer_TB_UIBased_1SEC( mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval, &mTFTDisplayControl.Backlight.OnTimeoutTimingCounter );
			// - Backlight on after startup
			mTFTDisplayControl.Backlight.State = UITFTDSP__BS__ON;
			break;

		case UITFTDSP__BS__RAMP_UP:
			// Backlight ramping up
			if ( mTFTDisplayControl.Backlight.RampUpDownTimingCounter.ulValue != 0 )
			{
				// Wait for timeup
			}
			else
			if ( mTFTDisplayControl.Backlight.ucRampLevel < mTFTDisplayControl.Backlight.ucRequiredLevel )
			{
				// Keep ramping down
				// - Update the ramp level
				mTFTDisplayControl.Backlight.ucRampLevel += UITFTDSP__BL_CONTROL_RAMP_UP_INCREMENT;
				// - Clip it if maximum exceeded (+4 for fast ramp up)
				if ( mTFTDisplayControl.Backlight.ucRampLevel >= mTFTDisplayControl.Backlight.ucRequiredLevel )
				{
					mTFTDisplayControl.Backlight.ucRampLevel = mTFTDisplayControl.Backlight.ucRequiredLevel;
				}
				// - Set the new level
				mTFTDisplayControl.Backlight.ucOperatingLevel = mTFTDisplayControl.Backlight.ucRampLevel;
				// - Restart delay timer
				GENTIMDT__StartTimer_TB_UIBased( UITFTDSP__BL_CONTROL_RAMP_UP_INTERVAL_UITICK, &mTFTDisplayControl.Backlight.RampUpDownTimingCounter );
			}
			else
			{
				// Restart the timeout timer
				GENTIMDT__StartTimer_TB_UIBased_1SEC( mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval, &mTFTDisplayControl.Backlight.OnTimeoutTimingCounter );
				// Flag as on
				mTFTDisplayControl.Backlight.State = UITFTDSP__BS__ON;
			}
			break;

		case UITFTDSP__BS__RAMP_DOWN:
			if ( ( TFTDSPIF__bTouchPressDetected() ) || ( vbTestMode ) )
			{
				// Got a touch press so ramp back up
				BacklightControl_InitRampUp();
			}
			else
			if ( mTFTDisplayControl.Backlight.RampUpDownTimingCounter.ulValue != 0 )
			{
				// Wait for timeup
			}
			else
			if ( mTFTDisplayControl.Backlight.ucRampLevel > mTFTDisplayControl.Backlight.ucRequiredLevel )
			{
				// Keep ramping down
				// - Update the ramp level
				mTFTDisplayControl.Backlight.ucRampLevel -= UITFTDSP__BL_CONTROL_RAMP_DN_DECREMENT;
				// - Set the new level
				mTFTDisplayControl.Backlight.ucOperatingLevel = mTFTDisplayControl.Backlight.ucRampLevel;
				// - Restart delay timer
				GENTIMDT__StartTimer_TB_UIBased( UITFTDSP__BL_CONTROL_RAMP_DN_INTERVAL_UITICK, &mTFTDisplayControl.Backlight.RampUpDownTimingCounter );
			}
			else
			{
				GENTIMDT__StartTimer_TB_UIBased_1SEC( mTFTDisplayControl.Backlight.ucDimmedTimeoutInterval, &mTFTDisplayControl.Backlight.OnTimeoutTimingCounter );

				mTFTDisplayControl.Backlight.State = UITFTDSP__BS__OFF;
			}
			break;

		case UITFTDSP__BS__OFF:
			// Backlight off - Check for a tap on the display
			if ( ( TFTDSPIF__bTouchPressDetected() ) || ( vbTestMode ) )
			{
				// A touch press has been detected - Begin ramp up
				BacklightControl_InitRampUp();
			}
			else
			if ( mTFTDisplayControl.Backlight.OnTimeoutTimingCounter.ulValue == 0 )
			{
				mTFTDisplayControl.Backlight.ucOperatingLevel  = 0;
				mTFTDisplayControl.Backlight.bShutdownIsActive = true;

				mTFTDisplayControl.Backlight.State = UITFTDSP__BS__DISPLAY_SHUTDOWN;
			}
			break;

		case UITFTDSP__BS__DISPLAY_SHUTDOWN:
			// Backlight - Display is shutdown so wait for wakeup of FT800
			if ( ( !mTFTDisplayControl.Backlight.bShutdownIsActive ) || ( vbTestMode ) )
			{
				// A touch press has been detected - Begin ramp up
				BacklightControl_InitRampUp();
			}
			break;

		default:
#ifndef		LEAVE_BACKLIGHT_ON
			// Backlight on  - Check for tag pressed
			if ( ( TFTDSPIF__bTouchPressDetected() ) || ( vbTestMode ) || ( mTFTDisplayControl.Backlight.bKeepAtFullBrightness ) )
			{
				// Touch tag value detected - Start timeout timer
				GENTIMDT__StartTimer_TB_UIBased_1SEC( mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval, &mTFTDisplayControl.Backlight.OnTimeoutTimingCounter );
			}
			else
			if ( mTFTDisplayControl.Backlight.OnTimeoutTimingCounter.ulValue == 0 )
			{
				// Start ramp down process
				BacklightControl_InitRampDown();
			}
			else
			{
				mTFTDisplayControl.Backlight.ucOperatingLevel = mTFTDisplayControl.Backlight.ucRequiredLevel = ucActiveBacklightLevel();
			}
#else
			mTFTDisplayControl.Backlight.ucOperatingLevel = mTFTDisplayControl.Backlight.ucRequiredLevel = ucActiveBacklightLevel();
#endif
			break;
	}
	//
	// Only allow tags when the backlight is on
	FT800_TOUCHIFC__SetTagMask( (bool)( mTFTDisplayControl.Backlight.State == UITFTDSP__BS__ON ) );
	//
	// Update the backlight with the operating level
	if ( vbTestMode )
	{
		TFTDSPIF__SwitchBacklight( FUNC_LOOP, vbLoPowerMode, vucTestModeLevel );
	}
	else
	{
		TFTDSPIF__SwitchBacklight( FUNC_LOOP, vbLoPowerMode, mTFTDisplayControl.Backlight.ucOperatingLevel );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the time interval for how long the backlight is kept on at full intensity.
 * @param:	None.
 * @retval:	uint8_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							UITFTDSP__Backlight_ucOnFullTimeoutInterval(void)
{
	return mTFTDisplayControl.Backlight.ucOnFullTimeoutInterval;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Forces backlight to be maintained at full brightness or not.
 * @param:	vbKeepAtFullBrightness - True/False flag.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__Backlight_KeepAtFullBrightness(bool vbKeepAtFullBrightness)
{
	mTFTDisplayControl.Backlight.bKeepAtFullBrightness = vbKeepAtFullBrightness;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks the backlight control state to determine whether or not the tag mask should be set to allow tag processing.
 * @param:	None.
 * @retval:	1 if backlight is on, 0 otherwise.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							UITFTDSP__TouchControl_ucTagMaskValue(void)
{
	// Check backlight control state
	switch ( mTFTDisplayControl.Backlight.State )
	{
		case UITFTDSP__BS__ON:
			return 1;

		default:
			return 0;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises the touch tag value details for rapid update processing - The initial values result in no rapid update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__TouchControl_RapidUpdateDataInit(void)
{
	TFTDSPIF__RapidUpdateDataInit();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Provides application level access to define the value details to permit rapid value update.
 * @param:	pTouchTagValue - Pointer to the touch tag value details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__TouchControl_RapidUpdateDataDefine(FT800_TOUCHIFC__FT800TouchTagValue* pTouchTagValue)
{
	TFTDSPIF__RapidUpdateDataDefine(pTouchTagValue);
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the current active tag value.
 * @param:	None.
 * @retval:	uint8_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							UITFTDSP__TouchControl_ucActiveTag(void)
{
	uint8_t		ucTagValue = 0;

	if ( ( ucTagValue = UITFTDSP_IMAGE__ucActiveImageTag() ) != 0 )
	{
		return ucTagValue;
	}

	if ( ( ucTagValue = UITFTDSP_BUTTON__ucActiveButtonTag() ) != 0 )
	{
		return ucTagValue;
	}

	return ucTagValue;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions to allow start/stop of PCLK.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__StartDisplayRefresh(void)
{
	GL050TN33_DISPIF__StartDisplayRefresh( FUNC_LOOP );
	return;
}
void							UITFTDSP__StopDisplayRefresh(void)
{
	GL050TN33_DISPIF__StopDisplayRefresh( FUNC_LOOP );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Enables the screenshot function - Prior to enabling this the screen is frozen.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UITFTDSP__ScreenShot_bEnabled(void)
{
	FT800_DEVICEIF__XFER_STATUS		XferStatus = FT800_DEVICEIF__XS_DONE_VALID;

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )					// Stop PCLK
	{
		XferStatus = GL050TN33_DISPIF__StopDisplayRefresh( FUNC_LOOP );
	}

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )					// Enable screenshot
	{
		XferStatus = GL050TN33_DISPIF__ScreenShotEnable( FUNC_LOOP );
	}

	return (bool)( XferStatus == FT800_DEVICEIF__XS_DONE_VALID );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Disables the screenshot function - Screen is unfrozen.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UITFTDSP__ScreenShot_bDisabled(void)
{
	FT800_DEVICEIF__XFER_STATUS		XferStatus = FT800_DEVICEIF__XS_DONE_VALID;

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )					// Disable screenshot
	{
		XferStatus = GL050TN33_DISPIF__ScreenShotDisable( FUNC_LOOP );
	}

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )					// Start PCLK
	{
		XferStatus = GL050TN33_DISPIF__StartDisplayRefresh( FUNC_LOOP );
	}

	return (bool)( XferStatus == FT800_DEVICEIF__XS_DONE_VALID );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Starts the capture of the selected line.
 * @param:	vuiYLineCount - The line ot scan : 0 based.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UITFTDSP__ScreenShot_bStarted(uint16_t vuiYLineCount)
{
	FT800_DEVICEIF__XFER_STATUS		XferStatus = FT800_DEVICEIF__XS_DONE_VALID;

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )					// Set line
	{
		XferStatus = GL050TN33_DISPIF__ScreenShotSetYPosition( FUNC_LOOP, vuiYLineCount );
	}

	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )					// Start
	{
		XferStatus = GL050TN33_DISPIF__ScreenShotStart( FUNC_LOOP );
	}

	return (bool)( XferStatus == FT800_DEVICEIF__XS_DONE_VALID );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks the capture busy status.
 * @param:	pbIsBusy - Pointer to busy flag.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UITFTDSP__ScreenShot_bBusyStatusCheckValid(bool* pbIsBusy)
{
	return (bool)( GL050TN33_DISPIF__ScreenShotCheckBusyStatus( FUNC_LOOP, pbIsBusy ) == FT800_DEVICEIF__XS_DONE_VALID );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Screenshot RGB line data retrieved and placed into buffer as : RGBRGBRGBRGB...
 * @param:	pucRGBLineBuffer - Pointer to memory location to hold RGB data : Must b 480x3 bytes long at least.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UITFTDSP__ScreenShot_bRGBDataRetrieved(uint8_t* pucRGBLineBuffer)
{
	FT800_DEVICEIF__XFER_STATUS		XferStatus = FT800_DEVICEIF__XS_DONE_VALID;
	FT800_DEVICEIF__RGBA			RGBA;
	uint16_t						uiXPos, uiBuffIndex;
	//
	// READ : Start
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		XferStatus = GL050TN33_DISPIF__ScreenShotReadStart( FUNC_LOOP );
	}
	// READ : Get RGBA data
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		uiBuffIndex = 0;

	    for ( uiXPos = 0; uiXPos < 480; uiXPos++ )
	    {
	    	if ( ( XferStatus = GL050TN33_DISPIF__ScreenShotReadRGBAValues( FUNC_LOOP, uiXPos, &RGBA ) ) == FT800_DEVICEIF__XS_DONE_VALID )
	    	{
	    		pucRGBLineBuffer[uiBuffIndex++] = RGBA.ucR;
	    		pucRGBLineBuffer[uiBuffIndex++] = RGBA.ucG;
	    		pucRGBLineBuffer[uiBuffIndex++] = RGBA.ucB;
	    	}
	    	else
	    	{
	    		break;
	    	}
	    }
	}
	//
	// READ : Stop
	if ( XferStatus == FT800_DEVICEIF__XS_DONE_VALID )
	{
		XferStatus = GL050TN33_DISPIF__ScreenShotReadStop( FUNC_LOOP );
	}

	return (bool)( XferStatus == FT800_DEVICEIF__XS_DONE_VALID );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the image flashing at the specified rate.
 * @param:	vFlashRate		  - The flash rate.
 * 			pImageAppSpecific - Pointer to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__FlashImage(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	if ( vFlashRate > UITFTDSP__FR__4HZ ) { vFlashRate = UITFTDSP__FR__4HZ;	}

	if ( mTFTDisplayControl.Flash[vFlashRate].bShowItem )
	{
		UITFTDSP_IMAGE__ShowImage( pImageAppSpecific );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the image specified never assigning any tag.
 * @param:	pImageAppSpecific - Pointer to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowImage(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	UITFTDSP_IMAGE__ShowImage( pImageAppSpecific );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the image specified assigning tag if one is defined.
 * @param:	pImageAppSpecific - Pointer to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowImageWithAssociatedTag(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	UITFTDSP_IMAGE__ShowImageWithAssociatedTag( pImageAppSpecific );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all evaporative cooling specific images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		UITFTDSP__UpdateFT800GRAM_CommonImages(FUNC_ACTION vAction)
{
	return UITFTDSP_IMAGE__UpdateFT800GRAM_CommonImages( vAction );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all heating specific images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		UITFTDSP__UpdateFT800GRAM_HeatingImages(FUNC_ACTION vAction)
{
	return UITFTDSP_IMAGE__UpdateFT800GRAM_HeatingImages( vAction );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all evaporative cooling specific images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		UITFTDSP__UpdateFT800GRAM_EvpCoolImages(FUNC_ACTION vAction)
{
	return UITFTDSP_IMAGE__UpdateFT800GRAM_EvpCoolImages( vAction );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all cooling specific images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		UITFTDSP__UpdateFT800GRAM_CoolingImages(FUNC_ACTION vAction)
{
	return UITFTDSP_IMAGE__UpdateFT800GRAM_CoolingImages( vAction );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a button on screen - Flash control allows toggling of active/passive color of screen.
 * @param:	vFlashRate - Flash rate of button color.
 * 			pButton	   - Pointer to button definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__FlashButton(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UITFTDSP_BUTTON__ButtonDefinition* pButton)
{
	if ( vFlashRate > UITFTDSP__FR__4HZ ) { vFlashRate = UITFTDSP__FR__4HZ;	}

	UITFTDSP_BUTTON__ShowButton( mTFTDisplayControl.Flash[vFlashRate].bShowItem, pButton );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a button on screen.
 * @param:	pButton - Pointer to button definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP__ShowButton(UITFTDSP_BUTTON__ButtonDefinition* pButton)
{
	UITFTDSP_BUTTON__ShowButton( true, pButton );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a button with user defined color on screen.
 * @param:	pButton - Pointer to button definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP__ShowButtonWithColor(UITFTDSP_BUTTON__ButtonDefinition* pButton)
{
	UITFTDSP_BUTTON__ShowButtonWithColor( true, pButton );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a button with two texts on screen.
 * @param:	pButtonWithTwoTexts	- Pointer to button with two texts definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP__ShowButtonWithTwoTexts(UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition* pButtonWithTwoTexts)
{
	UITFTDSP_BUTTON__ShowButtonWithTwoTexts( true, pButtonWithTwoTexts );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a rectangle on the screen with accompanying text flashing.
 * @param:	vFlashRate - Flash rate of text.
 * 			pRectangle - Pointer to rectangle application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowRectangleWithFlashingText(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UI_TFTSHAPE__RectangleWithTextDetails* pRectangle)
{
	if ( vFlashRate > UITFTDSP__FR__4HZ ) { vFlashRate = UITFTDSP__FR__4HZ;	}

	UITFTDSP_SHAPE__ShowRectangleWithText( mTFTDisplayControl.Flash[vFlashRate].bShowItem, pRectangle );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a rectangle on the screen with accompanying text.
 * @param:	pRectangle	- Pointer to rectangle application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowRectangleWithText(UI_TFTSHAPE__RectangleWithTextDetails* pRectangle)
{
	UITFTDSP_SHAPE__ShowRectangleWithText( true, pRectangle );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flashes a rectangle on the screen.
 * @param:	vFlashRate - Flash rate of text.
 * 			pRectangle - Pointer to rectangle application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__FlashRectangle(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UI_TFTSHAPE__Rectangle* pRectangle)
{
	if ( vFlashRate > UITFTDSP__FR__4HZ ) { vFlashRate = UITFTDSP__FR__4HZ;	}

	if ( mTFTDisplayControl.Flash[vFlashRate].bShowItem) { UITFTDSP_SHAPE__ShowRectangle(pRectangle ); }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a rectangle on the screen.
 * @param:	pRectangle - Pointer to rectangle application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowRectangle(UI_TFTSHAPE__Rectangle* pRectangle)
{
	UITFTDSP_SHAPE__ShowRectangle( pRectangle );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows bar graph background and levels on screen.
 * @param:	vucLevel  - Actual bar graph level.
 * 			pBarGraph - Pointer to bar graph definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP__ShowBarGraph(uint8_t vucLevel, UI_TFTSHAPE__BarGraphDetails* pBarGraph)
{
	UITFTDSP_SHAPE__ShowBarGraph( vucLevel, pBarGraph );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows the set temperature, fan speed or comfort level buttons with text and background on screen.
 * @param:	pLevelButtonAppSpecific - Pointer to level button definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP__ShowLevelButtons(UITFTDSP_BUTTON__LevelButtonDetails* pLevelButtons)
{
	UITFTDSP_BUTTON__ShowLevelButtons( pLevelButtons );
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the fan blade animation according to fan speed level specified.
 * @param:	vFanSpeedLevel    - Actual fan speed level.
 * 			pImageAppSpecific - Pointer to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel(TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	UITFTDSP_IMAGE__UpdateFanBladeAnimationAccordingToFanSpeedLevel( vFanSpeedLevel, pImageAppSpecific );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the text specified.
 * @param:	pText - Pointer to text application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowTextAbsolute(UITFTDSP_TEXT__TextDetails* pText)
{
	UITFTDSP_TEXT__ShowTextAbsolute( pText );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flashes the text specified.
 * @param:	vFlashRate - The flash rate.
 * 			pText	   - Pointer to text application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__FlashTextAbsolute(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UITFTDSP_TEXT__TextDetails* pText)
{
	if ( mTFTDisplayControl.Flash[vFlashRate].bShowItem )
	{
		UITFTDSP_TEXT__ShowTextAbsolute(pText);
	}
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the text specified with Y posn centered.
 * @param:	pText - Pointer to text application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowTextAbsoluteWithCenteredY(UITFTDSP_TEXT__TextDetails* pText)
{
	UITFTDSP_TEXT__ShowTextAbsoluteWithCenteredY( pText );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flashes the text specified with Y posn centered.
 * @param:	vFlashRate - The flash rate.
 * 			pText	   - Pointer to text application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__FlashTextAbsoluteWithCenteredY(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UITFTDSP_TEXT__TextDetails* pText)
{
	if ( mTFTDisplayControl.Flash[vFlashRate].bShowItem )
	{
		UITFTDSP__ShowTextAbsoluteWithCenteredY( pText );
	}
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:  Adds a flashing character to the string structure pointed to by pString.
 * @param:  vFlashRate - The flash rate.
 * 			vcChar	   - Character to add.
 * 			pString	   - Pointer to the string structure to be updated.
 * @retval: None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__AddFlashingCharacterToString(UI_TFTDISPLAY__FLASH_RATE vFlashRate, char vcChar, GENSTRDT__String* pString)
{
	if ( vFlashRate > UITFTDSP__FR__4HZ ) { vFlashRate = UITFTDSP__FR__4HZ;	}

	if ( mTFTDisplayControl.Flash[vFlashRate].bShowItem )
	{
		GENSTRDT__AddCharToString( vcChar, pString );
	}
	else
	{
		GENSTRDT__AddCharToString( ' ', pString );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the number specified - always has Y posn centered.
 * @param:	pNumber	- Pointer to number definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowNumber(UI_TFTNUMBER__NumberDetails* pNumber)
{
	UITFTDSP_NUMBER__ShowNumber( pNumber );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows the degree symbol specified.
 * @param:	pOuterPoint	- Pointer to outer point definition details.
 * 			pInnerPoint	- Pointer to inner point definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__ShowDegreeSymbol(UI_TFTSHAPE__PointDetails* pOuterPoint, UI_TFTSHAPE__PointDetails* pInnerPoint)
{
	// Draw outer point
	UITFTDSP_SHAPE__ShowPoint( pOuterPoint );
	// Draw inner point
	UITFTDSP_SHAPE__ShowPoint( pInnerPoint );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Sets the current color..
 * @param:	vulColor - Color to set to.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__SetCurrentColor(uint32_t vulColor)
{
	// Set current color
	UITFTDSP_COLOR__SetCurrentColor( vulColor );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rotates the image by any angle specified assigning tag if a valid one is defined.
 * @param:	viRotatingAngle	  - Variable to pass rotation angle.
 * 			pImageAppSpecific - Pointer to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP__RotateImage(int16_t viRotationAngle, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	UITFTDSP_IMAGE__RotateImage( viRotationAngle, pImageAppSpecific );
	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initiates the backlight control ramp up process.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						BacklightControl_InitRampUp(void)
{
	// - Set initial ramp level
	mTFTDisplayControl.Backlight.ucRampLevel     = mTFTDisplayControl.Backlight.ucRequiredLevel;
	// - Define the new required level
	mTFTDisplayControl.Backlight.ucRequiredLevel = ucActiveBacklightLevel();
	// - Start timer
	GENTIMDT__StartTimer_TB_UIBased( UITFTDSP__BL_CONTROL_RAMP_UP_INTERVAL_UITICK, &mTFTDisplayControl.Backlight.RampUpDownTimingCounter );
	// - Ramp up state
	mTFTDisplayControl.Backlight.State = UITFTDSP__BS__RAMP_UP;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initiates the backlight control ramp down process.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						BacklightControl_InitRampDown(void)
{
	// No touch tag value detected after timeout time
	// - Set initial ramp level
	mTFTDisplayControl.Backlight.ucRampLevel     = mTFTDisplayControl.Backlight.ucRequiredLevel;
	// - Define the new required level
	mTFTDisplayControl.Backlight.ucRequiredLevel = UITFTDSP__DIMMED_BL_LEVEL;
	// - Start timer
	GENTIMDT__StartTimer_TB_UIBased( UITFTDSP__BL_CONTROL_RAMP_DN_INTERVAL_UITICK, &mTFTDisplayControl.Backlight.RampUpDownTimingCounter );
	// - Ramp down state
	mTFTDisplayControl.Backlight.State = UITFTDSP__BS__RAMP_DOWN;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the backlight level after conversion.
 * @param:	None.
 * @retval:	uint8_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static uint8_t					ucActiveBacklightLevel(void)
{
	uint8_t		ucOffset, ucLevel;

	if ( ( mTFTDisplayControl.Backlight.ucOnFullLevel == 0 ) || ( mTFTDisplayControl.Backlight.ucOnFullLevel > 10 ) )
	{
		mTFTDisplayControl.Backlight.ucOnFullLevel = 10;
	}

	ucOffset = mTFTDisplayControl.Backlight.ucOnFullLevel - 1;
	ucLevel  = UITFTDSP__ACTIVE_BL_LEVEL_MIN + (ucOffset * 11);

	return ucLevel;
}
//*********************************************************************************************************************************************************************************
