//*********************************************************************************************************************************************************************************
// app_main.c
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Release Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * Version:		N-C7 	   - v090
 * Date:		01/10/2020
 * Modifier:	Emilio R. La Greca
 *
 * Original release.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include		<com_defn.h>
#include		<uitiming.h>
#include		<uiscncap.h>
#include		<uitftdsp.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<genstrdt.h>
#include		<gentimdt.h>
#include		<app_time.h>
#include		<nws_access.h>
#include		<zoning_control.h>
#include		<usbifmgr.h>
#include		<tstifmgr.h>
#include		<nvmifmgr.h>
#include		<i2cclmgr.h>
#include		<twbif502.h>
#include		<nwsutils.h>
#include		<nwsnwkcl.h>
#include		<tftdspif.h>
#include		<rtmclkif.h>
#include		<tempmsif.h>
#include		<pwrmgtif.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	APP_MAIN__DISPLAY_SHUTDOWN_STATE {				// System Display Shutdown State
																//
	APP_MAIN__DSS__IDLE        = 0,								// - Idle
	APP_MAIN__DSS__IS_ACTIVE   = 1,								// - Active
	APP_MAIN__DSS__WAIT_WAKEUP = 2								// - Wait for wakeup

} APP_MAIN__DISPLAY_SHUTDOWN_STATE;
//
// Enable when NW addx to be forced to 1 when loaded as 0
typedef enum	APP_MAIN__SYSTEM_STARTUP_STATE {

	APP_MAIN__SSS__WAIT_POWERUP = 0,
	APP_MAIN__SSS__TFT_SETUP    = 1,
	APP_MAIN__SSS__EEP_PARAM    = 2,
	APP_MAIN__SSS__RTC_SETUP    = 3,
	APP_MAIN__SSS__THM_SETUP    = 4,
	APP_MAIN__SSS__CAL_TOUCH    = 5,
	APP_MAIN__SSS__COMPLETED    = 6

} APP_MAIN__SYSTEM_STARTUP_STATE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	SystemDetails {										// System control data
																	//
	APP_MAIN__SYSTEM_STARTUP_STATE		CurrStartupState;			// - Startup state : Current
	APP_MAIN__SYSTEM_STARTUP_STATE		PrevStartupState;			// - Startup state : Previous
																	//
	bool								bControlUpdateOccurred;		// - Control system update flagged
	bool								bUpdateWatchdog;			// - Flag to allow watchdog update
																	//
	bool								bResetDueToWatchdog;		// - Flag indicating whether or not reset was due to watchdog timeout
	bool								bTestActive;				// - Flag when test is active
																	//
	APP_MAIN__DISPLAY_SHUTDOWN_STATE	DisplayShutdownState;		// - Display shutdown state
																	//
} SystemDetails;													//
																	//
static SystemDetails	mSystem;									// Container variable
//
// Device serial number -> TODO : Place in proper module later on
// - Maximum length
#define			APP_MAIN__SERIAL_NUMBER_MAX_LENGTH		16
// - Default (no serial number)
static const char		mkstrDefaultSerialNumber[] = "";
// - Where data kept
static char*			mstrDeviceSerialNumber[APP_MAIN__SERIAL_NUMBER_MAX_LENGTH+1];
// - String info
static GENSTRDT__String	mDeviceSerialNumber;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENSTRDT__StringData	APP_MAIN__kDeviceSerialNumber = { (char*)&mkstrDefaultSerialNumber[0], &mDeviceSerialNumber };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void			StartupStateUpdate_WaitPowerup(void);
static void			StartupStateUpdate_TFTSetup(void);
static void			StartupStateUpdate_EEPParam(void);
static void			StartupStateUpdate_RTCSetup(void);
static void			StartupStateUpdate_THMSetup(void);
static void			StartupStateUpdate_CALTouch(void);
static void			StartupStateUpdate_Completed(void);
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
 * @brief:  This is the entry point function after the reset vector is fetched upon a reset occurring.  It initialises all system peripherals and data, enables all interrupts and
 * 			then enters an endless loop.
 * @param:  None.
 * @retval:	None - Function Never Exits.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int								main(void)
{
	// SYSTEM INITIALISATION
	// - Microcontroller
 	MCUAXESS__MicrocontrollerInitialisation();
	//
	// SERIAL NUMBER INIT -> TODO : Place in proper module later on
	// - Pointer to data holder
	mDeviceSerialNumber.pstrText    = (char*)&mstrDeviceSerialNumber[0]; mstrDeviceSerialNumber[0] = '\0';
	// - Max length and length
	mDeviceSerialNumber.uiMaxLength = APP_MAIN__SERIAL_NUMBER_MAX_LENGTH;
	mDeviceSerialNumber.uiLength    = 0;
	//
	// MODULES
	// - Zone control
	ZONING_CONTROL__Initialise();
	// - UI TFT Display
	UITFTDSP__Initialise();
	// - UI Screen capture
	UISCNCAP__ScreenCapture_Init();
	// - NVM Interface
	NVMIFMGR__Initialise();
	// - USB-To-Serial Interface Manager
	USBIFMGR__Initialise();
	// - Test interface : No TEST!
	TSTIFMGR__Initialise();
	// - I2C Control
	I2CCLMGR_Initialise();
	// - TFT Display
	TFTDSPIF__Initialise();
	// - Power management interface
	PWRMGTIF__Initialise();
	//
	// System data init
	// - System in startup mode
	mSystem.CurrStartupState 	 = APP_MAIN__SSS__WAIT_POWERUP;
	mSystem.DisplayShutdownState = APP_MAIN__DSS__IDLE;
	mSystem.bTestActive			 = false;
	// - Start the dog
#if (BUILD_INFO__WATCHDOG == COM_DEFN__ENABLE)
	MCUAXESS__StartWatchdogCounter(); mSystem.bUpdateWatchdog = true;
#endif
	//
	// SYSTEM RUN
	do
	{
		// System update controlled by 10ms interrupt
		// - Check for restart
		if ( NVMIFMGR__bForceRestart() )
		{
#if (BUILD_INFO__WATCHDOG == COM_DEFN__ENABLE)
			mSystem.bUpdateWatchdog = false;
#else
			MCUAXESS__ResetMicrocontroller();
#endif
		}

		if ( NWSNWKCL__SlaveRegisterMonitor_bForceRestart() )	// Check for restart, NC7091
		{
#if (BUILD_INFO__WATCHDOG == COM_DEFN__ENABLE)
			mSystem.bUpdateWatchdog = false;
#else
			MCUAXESS__ResetMicrocontroller();
#endif
		}

		if ( !TWBIFUTL__TWBSystemCheck_bAllowWatchdogUpdate() )	// Check for restart via WD, NC7093
		{
#if (BUILD_INFO__WATCHDOG == COM_DEFN__ENABLE)
			mSystem.bUpdateWatchdog = false;
#else
			MCUAXESS__ResetMicrocontroller();
#endif
		}
		//
		// Watchdog update here
#if (BUILD_INFO__WATCHDOG == COM_DEFN__ENABLE)
		if ( ( mSystem.bUpdateWatchdog ) && ( mSystem.bControlUpdateOccurred ) )
		{
			MCUAXESS__RefreshWatchdogCounter(); mSystem.bControlUpdateOccurred = false;
		}
#endif
		// Check for test active
		if ( ( mSystem.bTestActive = TSTIFMGR__bATestIsBeingPerformed() ) == true )
		{

		}
		else
		if ( mSystem.CurrStartupState == APP_MAIN__SSS__COMPLETED )			// Any UI utility update done here...
		{
			if ( TFTDSPIF__bDisplayIsReady() ) { NWS_ACCESS__UIUtilityUpdate(); }
		}
	}
	while ( true );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the device serial number and forces it to be saved to eeprom.
 * @param:  vuiLength - The length of the serial number.
 * 			pucBuffer - Pointer to the buffer holding the serial number.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							APP_MAIN__UpdateAndSaveDeviceSerialNumber(uint16_t vuiLength, uint8_t* pucBuffer)
{
	uint16_t		uiIndex;

	if ( vuiLength > mDeviceSerialNumber.uiMaxLength )
	{
		return;
	}

	for ( uiIndex = 0; uiIndex < vuiLength; uiIndex++ )
	{
		mDeviceSerialNumber.pstrText[uiIndex] = pucBuffer[uiIndex];
	}
	mDeviceSerialNumber.uiLength = vuiLength;

	NVMIFMGR__BeginSaveProcess_SerialNumber();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Retrieves the device serial number.
 * @param:  pSerialNumber - Pointer to where serial number info to be placed.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							APP_MAIN__RetrieveSerialNumber(GENSTRDT__String* pSerialNumber)
{
	pSerialNumber->pstrText    = mDeviceSerialNumber.pstrText;
	pSerialNumber->uiLength    = mDeviceSerialNumber.uiLength;
	pSerialNumber->uiMaxLength = mDeviceSerialNumber.uiMaxLength;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update of 1ms based processes - Keep them short!!
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							APP_MAIN__Update01ms(void)
{
	TFTDSPIF__Update01ms();

	PWRMGTIF__Update01ms();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update of 10ms based processes - System update flag set if allowed to.  If not allowed to no dog update occurs.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							APP_MAIN__Update10ms(void)
{
	// Common Processes - Every 10ms
	// - Check for time and day update
	RTMCLKIF__UpdateRTCTimeKeepingControlData( TWBIF502__bIsMasterType02Device );
	// - Timing update : System
	APP_TIME__UpdateTimersx10ms();
	// - Timing update : NC7 UI Based Timers
	UITIMING__UpdateTimers();
	// - Temperature measurement update
	TEMPMSIF__UpdateTemperatureMeasurement();
	// - USB-To-Serial Interface Manager, Pass startup state, emilio 14-01-16
	USBIFMGR__Update();
	// - I2C Control
	I2CCLMGR_Update();
	// - Power management
	PWRMGTIF__Update();

	switch ( mSystem.CurrStartupState )
	{
		case APP_MAIN__SSS__WAIT_POWERUP:	StartupStateUpdate_WaitPowerup();	break;
		case APP_MAIN__SSS__TFT_SETUP:		StartupStateUpdate_TFTSetup();		break;
		case APP_MAIN__SSS__EEP_PARAM:		StartupStateUpdate_EEPParam();		break;
		case APP_MAIN__SSS__RTC_SETUP:		StartupStateUpdate_RTCSetup();		break;
		case APP_MAIN__SSS__THM_SETUP:		StartupStateUpdate_THMSetup();		break;
		case APP_MAIN__SSS__CAL_TOUCH:		StartupStateUpdate_CALTouch();		break;
		case APP_MAIN__SSS__COMPLETED:		StartupStateUpdate_Completed();		break;

		default:
			MCUAXESS__ResetMicrocontroller();
			break;
	}
	//
	// Control update flagged
	mSystem.bControlUpdateOccurred = true;

	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//																	MAIN UPDATE STATE FUNCTIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Process main startup state - Wait powerup.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartupStateUpdate_WaitPowerup(void)
{
	// Wait for power stabilisation
	if ( !PWRMGTIF__bPowerSupplyVoltageStabilized() )
	{
		return;
	}
	mSystem.PrevStartupState = mSystem.CurrStartupState;
	//
	// Set startup state
	mSystem.CurrStartupState = APP_MAIN__SSS__TFT_SETUP;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Process main startup state - TFT driver and display setup.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartupStateUpdate_TFTSetup(void)
{
	// Check for a change in state
	if ( mSystem.PrevStartupState != mSystem.CurrStartupState )
	{
		TFTDSPIF__StartInitialisation();
	}
	mSystem.PrevStartupState = mSystem.CurrStartupState;
	//
	// Update State
	// - Perform update, Moved from main loop as only needed during init, NC7 - v66
	TFTDSPIF__Update();
	// - Check if ready
	if ( TFTDSPIF__bDisplayIsReady() )
	{
		mSystem.CurrStartupState = APP_MAIN__SSS__EEP_PARAM;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Process main startup state - EEP param load.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartupStateUpdate_EEPParam(void)
{
	if ( mSystem.bTestActive )
	{
		mSystem.CurrStartupState = APP_MAIN__SSS__RTC_SETUP;
		return;
	}
	//
	// Check for a change in state
	if (mSystem.PrevStartupState != mSystem.CurrStartupState)
	{
#if (BUILD_INFO__CLEAR_EEPROM == COM_DEFN__ENABLE)
		NVMIFMGR__BeginClearProcess_All();
#else
		NVMIFMGR__LoadProcess_Start();		// Start param load
#endif
	}
	mSystem.PrevStartupState = mSystem.CurrStartupState;
	//
	// Update state
#if (BUILD_INFO__CLEAR_EEPROM == COM_DEFN__ENABLE)
	if ( NVMIFMGR__bClearProcessComplete() )
#else
	if ( NVMIFMGR__bLoadProcessComplete() )						// Check if param load done
#endif
	{
		mSystem.CurrStartupState = APP_MAIN__SSS__RTC_SETUP;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Process main startup state - RTC setup.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartupStateUpdate_RTCSetup(void)
{
	// Check for a change in state
	if ( mSystem.PrevStartupState != mSystem.CurrStartupState )
	{
		RTMCLKIF__Initialise();
	}
	mSystem.PrevStartupState = mSystem.CurrStartupState;
	//
	// Update state - Check RTC ready
	if ( RTMCLKIF__bClockIsReadyForTimeKeeping() )
	{
		mSystem.CurrStartupState = APP_MAIN__SSS__THM_SETUP;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Process main startup state - Thermistor interface setup.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartupStateUpdate_THMSetup(void)
{
	// Check for a change in state
	if ( mSystem.PrevStartupState != mSystem.CurrStartupState )
	{
		TEMPMSIF__Initialise();
	}
	mSystem.PrevStartupState = mSystem.CurrStartupState;
	//
	// Temperature measurement update
	TEMPMSIF__UpdateTemperatureMeasurement();
	//
	// Check for temperature reading calculated
	if ( TEMPMSIF__bTemperatureMeasurementReadingCalculated() )
	{
		if ( mSystem.bTestActive )
		{
			mSystem.CurrStartupState = APP_MAIN__SSS__COMPLETED;
		}
		else
		{
			mSystem.CurrStartupState = APP_MAIN__SSS__CAL_TOUCH;
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Process main startup state - Touch calibration.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartupStateUpdate_CALTouch(void)
{
#if defined(ALLOW_DUMMY_TEST)		// Include here before cal done
	//TSTIFMGR__StartDummyTest_BTN();
#endif
	if ( mSystem.bTestActive )						// When test being performed get out as touch will be calibrated and test as part of test sequence
	{
		// Reset the FT800
		mSystem.CurrStartupState = APP_MAIN__SSS__WAIT_POWERUP;
		return;
	}
	//
	// Check for a change in state
	if ( mSystem.PrevStartupState != mSystem.CurrStartupState )
	{
		TFTDSPIF__TouchCalInit();	// Start touch cal
	}
	mSystem.PrevStartupState = mSystem.CurrStartupState;
	//
	// Update state
	// - Touch cal update
	TFTDSPIF__TouchUpdate();
	// - Touch cal status check
	if ( TFTDSPIF__bTouchCalDoneValid() )
	{
		mSystem.CurrStartupState = APP_MAIN__SSS__COMPLETED;
	}
	else
	if ( TFTDSPIF__bTouchCalDoneValidSaveValues() )
	{
		NVMIFMGR__BeginSaveProcess_TouchCalValues( false );

		mSystem.CurrStartupState = APP_MAIN__SSS__COMPLETED;
	}
	else
	if ( TFTDSPIF__bTouchCalDoneFatal() )
	{
		MCUAXESS__ResetMicrocontroller();
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Process main startup state - Completed (Normal Processing)
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartupStateUpdate_Completed(void)
{
	static bool			sbUtilityAccessToUI = false;
	static uint16_t		suiDisplayTimeoutTimer;
	//
	// Check for a change in state
	if ( mSystem.PrevStartupState != mSystem.CurrStartupState )
	{
		// INITIALISE ALL CONTROL DATA
		// ---------------------------
		// - UI timing init
		UITIMING__Initialise();
	    // - Initialise the control interface
		NWS_ACCESS__Initialise();
	    // - Need to know now if this is the master
		TWBIF502__UpdateMasterSlaveStatus();
	    //
	    // OVERRIDE INITIALISATION ACCORDINGLY - Master Only
	    // -------------------------------------------------
	    NWS_ACCESS__OverrideInitialConfigurationAfterPowerup();
#if defined(ALLOW_DUMMY_TEST)
	    TSTIFMGR__StartDummyTest_BTN();
#endif
	}
	mSystem.PrevStartupState = mSystem.CurrStartupState;
	//
	// Doing a test - Control and UI update, emilio 03-07-17?
	if ( mSystem.bTestActive )
	{
		TSTIFMGR__TestSequence_UpdateControl(); TSTIFMGR__TestSequence_UpdateUserif();
		return;
	}
	//
	// Timer added to time any display re-init failure
	if ( suiDisplayTimeoutTimer != 0 ) { suiDisplayTimeoutTimer -= 10; }
	//
	// Process system update - Control System : 10ms based for two-wire bus comms update
	MCUAXESS__SetPC3Hi( true );

	// Control system update
	NWS_ACCESS__ControlUpdate();
	//
	// UI Update - This coding placed here so UI update occurs always after control update
	// - UI Utility update flag (main loop access to FT800)
	/*
	if ( ( NWS_ACCESS__bUtilityAccessToUI() ) && ( !sbUtilityAccessToUI ) )
	*/ // Always restart timer if screen capture utility running, NC7090_SCNCAP
	if ( ( ( NWS_ACCESS__bUtilityAccessToUI() ) && ( !sbUtilityAccessToUI ) ) || NWS_ACCESS__bScreenCaptureInProgress() )
	{
		suiDisplayTimeoutTimer = 2500;									// UI utility update must complete in this time - 2500ms
	}
	sbUtilityAccessToUI = NWS_ACCESS__bUtilityAccessToUI();
	// - Display init or FT800 access error?
	if ( !TFTDSPIF__bDisplayIsReady() )									// Display init underway?
	{
		TFTDSPIF__Update();												// Yes so update
	}
	else
	if ( !TFTDSPIF__bConfiguredForOperation( sbUtilityAccessToUI ) )	// Display configured for operation?
	{
		TFTDSPIF__StartInitialisation();								// No so re-init with full reset...

		suiDisplayTimeoutTimer = 2500;									// Must be initialized within this time - 2500ms
	}
	//
	// - Display access
	if ( sbUtilityAccessToUI )											// Display utility access?
	{
		// Yes so no FT800 access...
		if ( suiDisplayTimeoutTimer == 0 ) { MCUAXESS__ResetMicrocontroller(); }
	}
	else
	if ( !TFTDSPIF__bDisplayIsReady() )									// Display ready for access?
	{
		// No so no FT800 access...
		if ( suiDisplayTimeoutTimer == 0 ) { MCUAXESS__ResetMicrocontroller(); }
	}
	else
	{
		MCUAXESS__SetPB10Hi( true );
		//
		// Display accessible so perform low-level updates
		// - Flash control update
		UITFTDSP__FlashControl_Update();
		// - Touch control update
		UITFTDSP__TouchControl_Update();
		// - Backlight operation update
		UITFTDSP__BacklightControl_Update( PWRMGTIF__bOperateInLowPowerMode(), false, 0);

		MCUAXESS__SetPB10Hi( false );
	}
	//
	// Display shutdown control
	switch ( mSystem.DisplayShutdownState )
	{
		case APP_MAIN__DSS__IS_ACTIVE:
			// Display shutdown is active --> Look for a touch press
			if ( TFTDSPIF__bTouchPressDetected() )
			{
				mSystem.DisplayShutdownState = APP_MAIN__DSS__WAIT_WAKEUP;
			}
			break;

		case APP_MAIN__DSS__WAIT_WAKEUP:
			// Display shutdown wakeup
			UITFTDSP__BacklightOperation_Wakeup();

			mSystem.DisplayShutdownState = APP_MAIN__DSS__IDLE;
			break;

		default:
			// Check for display shutdown
			if ( UITFTDSP__bDisplayShutdownIsActive() )
			{
				mSystem.DisplayShutdownState = APP_MAIN__DSS__IS_ACTIVE;
			}
			break;
	}
	//
	// Main UI update -> Always to be performed (Internally FT800 access will be blocked when access is not allowed)
	NWS_ACCESS__UserifUpdate();

	MCUAXESS__SetPC3Hi( false );

	return;
}
//*********************************************************************************************************************************************************************************
