//*********************************************************************************************************************************************************************************
// nws_userif_NC7.c
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
#include		<nws_access_common.h>
#include		<nws_cfgmnu_control.h>
#include		<nws_isetup_control.h>
#include		<nws_csetup_control.h>
#include		<nws_pinent_control.h>
#include		<nws_dspset_control.h>
#include		<nws_sysset_control.h>
#include		<nws_usrprf_control.h>
#include		<nws_zonenm_control.h>
#include		<nws_prmaxs_control.h>
#include		<nws_svcinf_control.h>
#include		<nws_clnscn_control.h>
#include		<nws_svcinf_control.h>

#include		<nwsutils.h>
#include		<twbif502.h>
#include		<twbifutl.h>
#include		<tempmsif.h>

#include		<uiscncap.h>
#include		<uitftdsp.h>
#include		<uicommon.h>
#include		<uitiming.h>
#include        <tftdspif.h>
#include		<uitftdsp_color.h>

#include		<heating_control.h>
#include		<cooling_control.h>
#include		<twbifmgr.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
typedef enum 	NWS_USERIF_NC7__TAG_VALUE {

	NWS_USERIF_NC7__TV__HELP_BTN  = 1,
	NWS_USERIF_NC7__TV__MENU_BTN  = 3,
	NWS_USERIF_NC7__TV__ALERT_BTN = 25

} NWS_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_USERIF_NC7__IMAGE_NUMBER {

	NWS_USERIF_NC7__IN__MENU_BTN  	  = 0,
	NWS_USERIF_NC7__IN__HELP_BTN      = 1,
	NWS_USERIF_NC7__IN__BRIVIS_LOGO   = 2,
	NWS_USERIF_NC7__IN__ALERT_ICON    = 3

} NWS_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							    Image - Passive,					           Image - Active,   X,   Y, 						      Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USERIF_NC7__TV__MENU_BTN  },
	{  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USERIF_NC7__TV__HELP_BTN  },
#else
	{  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USERIF_NC7__TV__MENU_BTN  },
	{  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USERIF_NC7__TV__HELP_BTN  },
#endif
#if defined(USE_BRIVIS_LOGO)
	{  		UITFTDSP_IMAGE__IN__COMMON__BRIVIS_LOGO,         		     UITFTDSP_IMAGE__NO_IMAGE, 438, 230, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1							  },
#else
	{  		UITFTDSP_IMAGE__IN__COMMON__BRIVIS_LOGO,         			 UITFTDSP_IMAGE__NO_IMAGE, 438, 242, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1							  },
#endif
	{ 		  UITFTDSP_IMAGE_COMMON__IN__ALERT_ICON,            		 UITFTDSP_IMAGE__NO_IMAGE, 440, 165, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USERIF_NC7__TV__ALERT_BTN }
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Number - Use as index to mkaText[]
typedef enum	NWS_USERIF_NC7__TEXT_NUMBER {

	NWS_USERIF_NC7__TN_NO_APPLIANCE = 0,
	NWS_USERIF_NC7__TN__ATTENTION	= 1

} NWS_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails				mkaText[] = {
	//   X,   Y,  X0,  Y0, 			        Color,          	      FontSize,    	        Justification, String Label
	{  112,  53,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, "No Appliance" },
	{  454, 200,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Alert"    	  }
} ;

// Macro definition to simplify function calls : X = NWS_USERIF__NC7__TEXT_NUMBER type which indexes array
#define NWS_USERIF_NC7__Text(X)				( (UITFTDSP_TEXT__TextDetails*)&mkaText[X] )
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// PIN Entry Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
typedef enum 	NWS_ACCESS_USERIF_NC7__PIN_ENTRY {							// Pin Entry
																			//
	NWS_ACCESS_USERIF_NC7__PE__INSTALLER_SETUP = 0,							// - Installer Setup
	NWS_ACCESS_USERIF_NC7__PE__SERVICE_ACCESS  = 1,							// - Service Access

} NWS_ACCESS_USERIF_NC7__PIN_ENTRY;

static const NWS_PINENT_COMMON__PinEntryDetails	mkPinEntry[] = {			// Use index : NWS_ACCESS_USERIF_NC7__PIN_ENTRY

	//           Pin Number , Top Status Text		  , Pin Description
	{ { '5', '2', '4', '0' }, "MENU : INSTALLER SETUP", "Enter installer setup PIN:"  	},
	{ { '3', '3', '6', '6' }, "MENU : DIAGNOSTICS"	  , "Enter the Brivis Service PIN" 	}
};
//
// Number of UI update prior to refreshing image
#define			NWS_USERIF_NC7__NUMBER_OF_UPDATE_PRIOR_TO_IMAGE_REFRESH		10
// Time interval when pin access not required - 120s
#define			NWS_USERIF_NC7__PIN_ACCESS_NOT_REQUIRED_INTERVAL_TBUI1S		120
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	NWS_ACCESS_USERIF_NC7__ControlDetails {							// Networker System Access - NC7 Specific Control
																				//
	TWBIFUTL__OPERATING_STATE_NC7			CurrConfigMenuAccessState;			// - Current config menu access state
	TWBIFUTL__OPERATING_STATE_NC7			PrevConfigMenuAccessState;			// - Previous config menu access state
	NWS_PINENT_CONTROL__PIN_ACCESS			PinAccess;							// - Pin access state
																				//
	bool									bScreenCaptureInProgress;			// - Flag for screen capture
	bool									bImageDownloadInProgress;			// - Flag image download
																				//
	GENTIMDT__TimingCounter					InstallParamAccessExitTimeCounter;	// - Timer since install param access exited
	GENTIMDT__TimingCounter					ServiceParamAccessExitTimeCounter;	// - Timer since service param access exited
																				//
	bool									bWaitingAfterDisplayInit;			// - Display init state
	GENTIMDT__TimingCounter					DisplayWaitAfterInitTimeCounter;	// - Timer for display after init

} NWS_ACCESS_USERIF_NC7__ControlDetails;

static NWS_ACCESS_USERIF_NC7__ControlDetails	mNWSystem_NC7UI;
static NWS_SVCINF_COMMON__ServiceInfoDetails	mServiceInfo;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	NWS_ACCESS_USERIF_NC7__kNetworkerSystemInstallParamAccessExitTiming = { &mNWSystem_NC7UI.InstallParamAccessExitTimeCounter  };
const struct	GENTIMDT__Timing	NWS_ACCESS_USERIF_NC7__kNetworkerSystemServiceParamAccessExitTiming = {	&mNWSystem_NC7UI.ServiceParamAccessExitTimeCounter  };
const struct	GENTIMDT__Timing	NWS_ACCESS_USERIF_NC7__kNetworkerSystemDisplayWaitAfterInitTiming 	= {	&mNWSystem_NC7UI.DisplayWaitAfterInitTimeCounter	};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void							ImageDownloadUpdate(bool vbRedoDownload, NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem);
static bool							bViewingServiceInfoPrompt(void);
static void							RapidUpdateControl_None(void);
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
 * @brief:	Access init - Configuration menu (N-C7 Only).
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__ConfigMenu_AccessInit(void)
{
	// Bot waiting, emilio 07-10-16
	mNWSystem_NC7UI.bWaitingAfterDisplayInit = false;
	mNWSystem_NC7UI.bImageDownloadInProgress = false;
	mNWSystem_NC7UI.bScreenCaptureInProgress = false;
	// Config menu access states (previous/current)
	mNWSystem_NC7UI.CurrConfigMenuAccessState = TWBIFUTL__OSNC7__IDLE;
	mNWSystem_NC7UI.PrevConfigMenuAccessState = TWBIFUTL__OSNC7__IDLE;
	// Pin access not required
	mNWSystem_NC7UI.PinAccess = NWS_PINENT_CONTROL__PA__NOT_REQUIRED;
	// Clear access exit timers, emilio 08-12-15
	GENTIMDT__StartTimer_TB_UIBased_1SEC( 0, &mNWSystem_NC7UI.InstallParamAccessExitTimeCounter );
	GENTIMDT__StartTimer_TB_UIBased_1SEC( 0, &mNWSystem_NC7UI.ServiceParamAccessExitTimeCounter );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Access update - Configuration menu (N-C7 Only).
 * 			Switching between the various NC7 specific config menu states and the corresponding UI update is done here.
 * @param:	vOperationalState - The current networker operational state.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__ConfigMenu_AccessUpdate(TWBIFUTL__NWSYS_OPNSTATE vOperationalState)
{
	bool		bStateChange = false;
	//
	// Operational state - NC7 based
	// - Get current config menu access state
	mNWSystem_NC7UI.CurrConfigMenuAccessState = NWSUTILS__ConfigMenuAccessState_NC7();
	// - Check if param access exited to start access exit time counter, emilio 08-12-15
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU )
	{
		if ( mNWSystem_NC7UI.PinAccess == NWS_PINENT_CONTROL__PA__NOT_REQUIRED )
		{
			if ( mNWSystem_NC7UI.PrevConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_SETUP )
			{
				// Access to installer setup exited after it had been granted - Start timeout timer
				GENTIMDT__StartTimer_TB_UIBased_1SEC( NWS_USERIF_NC7__PIN_ACCESS_NOT_REQUIRED_INTERVAL_TBUI1S, &mNWSystem_NC7UI.InstallParamAccessExitTimeCounter );
			}
			else
			if  ( mNWSystem_NC7UI.PrevConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__SERVICE_INFO )
			{
				// Access to service info setup exited after it had been granted - Start timeout timer
				GENTIMDT__StartTimer_TB_UIBased_1SEC( NWS_USERIF_NC7__PIN_ACCESS_NOT_REQUIRED_INTERVAL_TBUI1S, &mNWSystem_NC7UI.ServiceParamAccessExitTimeCounter );
			}
		}
	}
	// - Flag if changed
	bStateChange = ( mNWSystem_NC7UI.PrevConfigMenuAccessState != mNWSystem_NC7UI.CurrConfigMenuAccessState );
	//
	// Check if schedule being accessed by another networker - If so no config menu related UI update, jason 10-12-15
	// - Check for STSP
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		// Check for operational state being accessed
		switch ( mNWSystem_NC7UI.CurrConfigMenuAccessState )
		{
			case TWBIFUTL__OSNC7__CONFIG_MENU:
			case TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_SETUP:
			case TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_SETUP__CUSTOM_SETUP:
			case TWBIFUTL__OSNC7__CONFIG_MENU__SERVICE_INFO:
				// Check if schedule being accessed by another network
				if ( HEATING_CONTROL__bScheduleIsBeingProgrammed() || COOLING_CONTROL__bScheduleIsBeingProgrammed() )
				{
					// Exit before updating config menu related UI
					return;
				}
				break;

			default:
				break;
		}
	}
	//
	// Now config menu related UI
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU )
	{
		//
		// SCREEN - Main Selection
		//
		// Configuration Menu - UI update here
		NWS_CFGMNU_CONTROL__UserifUpdate( bStateChange );
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__CLEAN_SCREEN )
	{
		//
		// SCREEN - Clean Screen
		//
		// - UI update here
		NWS_CLNSCN_CONTROL__UserifUpdate( bStateChange );
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__SET_CLOCK )
	{
		//
		// SCREEN - Set Day/Time
		//		  - Update via NWS_CLKSET_CONTROL__UserifUpdate() to maintain backward compatibility with N-C6
		//
		// Set Clock - Just check for entry (UI update where OperationalState == TWBIFUTL_kucVALU_OPNSTATE_SETCLOCK)
		if ( vOperationalState != TWBIFUTL__NWSYS_OPNSTATE__SETCLOCK ) { NWSUTILS__SetCurrentOperationalState_ClockSetting(); }
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_PARAM_ACCESS )
	{
		//
		// SCREEN - Parameter Setting (Installer OR Service)
		//		  - Update via NWS_PRMAXS_CONTROL__UserifUpdate() to maintain backward compatibility with N-C6
		//
		// Access params - Just check for entry (UI update where OperationalState == TWBIFUTL_kucVALU_OPNSTATE_SETCLOCK)
		if ( vOperationalState != TWBIFUTL__NWSYS_OPNSTATE__AXSPARAM )
		{
			NWSUTILS__SetCurrentOperationalState_ParameterAccess( NWS_CSETUP_CONTROL__pDeviceAddress() );

			NWS_PRMAXS_CONTROL__SetDeviceSelectedDetails( NWS_CSETUP_CONTROL__DeviceTypeSelected(), NWS_CSETUP_CONTROL__Type02DeviceSelected() );
		}
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__SERVICE_PARAM_ACCESS )
	{
		//
		// SCREEN - Parameter Setting (Installer OR Service)
		//		  - Update via NWS_PRMAXS_CONTROL__UserifUpdate() to maintain backward compatibility with N-C6
		//
		// Access params - Just check for entry (UI update where OperationalState == TWBIFUTL_kucVALU_OPNSTATE_SETCLOCK)
		if ( vOperationalState != TWBIFUTL__NWSYS_OPNSTATE__AXSPARAM )
		{
			NWSUTILS__SetCurrentOperationalState_ParameterAccess( NWS_CSETUP_CONTROL__pDeviceAddress() );

			NWS_PRMAXS_CONTROL__SetDeviceSelectedDetails( NWS_CSETUP_CONTROL__DeviceTypeSelected(), NWS_CSETUP_CONTROL__Type02DeviceSelected() );
		}
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_SETUP )
	{
		//
		// SCREEN - Installer Setup
		//		  - Pin access required before access granted
		//
		// Installer Setup Menu (UI update here - PIN UI update before this done)
		if ( mNWSystem_NC7UI.PrevConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU )
		{
			// Installer setup screen has just been called for
			if ( mNWSystem_NC7UI.InstallParamAccessExitTimeCounter.ulValue == 0 )
			{
				// Timeout - Pin access required!
				mNWSystem_NC7UI.PinAccess = NWS_PINENT_CONTROL__PA__REQUIRED;
			}
			else
			{
				// Access still valid - Pin access not required!
				mNWSystem_NC7UI.PinAccess = NWS_PINENT_CONTROL__PA__NOT_REQUIRED;
			}
		}
		//
		// Check pin access state
		if ( mNWSystem_NC7UI.PinAccess == NWS_PINENT_CONTROL__PA__REQUIRED )
		{
			// PIN required - UI update here
			NWS_PINENT_CONTROL__UserifUpdate( bStateChange, (NWS_PINENT_COMMON__PinEntryDetails*)&mkPinEntry[NWS_ACCESS_USERIF_NC7__PE__INSTALLER_SETUP], &mNWSystem_NC7UI.PinAccess );

			if ( mNWSystem_NC7UI.PinAccess == NWS_PINENT_CONTROL__PA__CANCELLED )
			{
				NWSUTILS__RevertConfigMenu();		// Revert back to config menu
			}
		}
		else
		{
			// PIN valid so installer setup screen - UI update here
			if ( mNWSystem_NC7UI.PinAccess == NWS_PINENT_CONTROL__PA__GRANTED )
			{
				// Granted so allow access to Installer Setup
				bStateChange = true; mNWSystem_NC7UI.PinAccess = NWS_PINENT_CONTROL__PA__NOT_REQUIRED;
			}

			NWS_CSETUP_CONTROL__UserifUpdate_Install( bStateChange );
		}
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_SETUP__CUSTOM_SETUP )
	{
		//
		// SCREEN - Installer Setup : Custom Setup
		//
		NWS_CSETUP_CONTROL__UserifUpdate_Install( bStateChange );
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_SETUP__SETUP_ASSIST )
	{
		//
		// SCREEN - Installer Setup : Setup Assistant
		//
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__INSTALLER_SETUP__SYSTEM_INFO )
	{
		//
		// SCREEN - Installer Setup : System Information
		//
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__SERVICE_INFO )
	{
		//
		// SCREEN - Service Access
		//		  - Pin access required before access granted
		//
		// Service Access Menu (UI update here - PIN UI update before this done)
		if ( mNWSystem_NC7UI.PrevConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU )
		{
			// Service parameter access screen has just been called for
			if ( mNWSystem_NC7UI.ServiceParamAccessExitTimeCounter.ulValue == 0 )
			{
				// Timeout - Pin access required!
				mNWSystem_NC7UI.PinAccess = NWS_PINENT_CONTROL__PA__REQUIRED;
			}
			else
			{
				// Access still valid - Pin access not required!
				mNWSystem_NC7UI.PinAccess = NWS_PINENT_CONTROL__PA__NOT_REQUIRED;
			}
		}
		//
		// Check pin access state
		if ( mNWSystem_NC7UI.PinAccess == NWS_PINENT_CONTROL__PA__REQUIRED )
		{
			// PIN required - UI update here
			NWS_PINENT_CONTROL__UserifUpdate( bStateChange, (NWS_PINENT_COMMON__PinEntryDetails*)&mkPinEntry[NWS_ACCESS_USERIF_NC7__PE__SERVICE_ACCESS], &mNWSystem_NC7UI.PinAccess );

			if ( mNWSystem_NC7UI.PinAccess == NWS_PINENT_CONTROL__PA__CANCELLED )
			{
				NWSUTILS__RevertConfigMenu();		// Revert back to config menu
			}
		}
		else
		{
			// PIN valid so installer setup screen - UI update here
			if ( mNWSystem_NC7UI.PinAccess == NWS_PINENT_CONTROL__PA__GRANTED )
			{
				// Granted so allow access to Installer Setup
				bStateChange = true; mNWSystem_NC7UI.PinAccess = NWS_PINENT_CONTROL__PA__NOT_REQUIRED;
			}

			NWS_CSETUP_CONTROL__UserifUpdate_Service( bStateChange );
		}
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__USER_PREF )
	{
		//
		// SCREEN - User Preferences
		//		  - Update via NWS_USRSET_CONTROL__UserifUpdate() to maintain backward compatibility with N-C6
		//
		// User Preferences - Just check for entry (UI update where OperationalState == TWBIFUTL_kucVALU_OPNSTATE_AXSUSRST)
		NWS_USRPRF_CONTROL__UserifUpdate( bStateChange );
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__USER_PREF_KEYPAD_LOCKOUT )
	{
		//
		// SCREEN - User Preferences : Keypad Lockout
		//
		// User Preferences - Just check for entry (UI update where OperationalState == TWBIFUTL_kucVALU_OPNSTATE_ENTERPIN)
		if ( vOperationalState != TWBIFUTL__NWSYS_OPNSTATE__ENTERPIN ) { NWSUTILS__SetCurrentOperationalState_PinNumberEntry( TWBIF502__ThisUnitID() ); }
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__USER_PREF_GNRL_SETTINGS )
	{
		//
		// SCREEN - User Preferences : General display settings
		//
		NWS_DSPSET_CONTROL__UserifUpdate( bStateChange );
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__USER_PREF_SYST_SETTINGS )
	{
		//
		// SCREEN - User Preferences : System settings
		//
		// System settings same as user settings in N-C6, emilio 04-01-15
		if ( vOperationalState != TWBIFUTL__NWSYS_OPNSTATE__AXSUSRST ) { NWSUTILS__SetCurrentOperationalState_UserSettings( TWBIF502__ThisUnitID() ); }

		NWS_SYSSET_CONTROL__UserifUpdate( bStateChange );
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__USER_PREF_ZONE_NAMES )
	{
		//
		// SCREEN - User Preferences : Set Zone Names
		//
		NWS_ZONENM_CONTROL__UserifUpdate( bStateChange );
	}
	else
	if ( mNWSystem_NC7UI.CurrConfigMenuAccessState == TWBIFUTL__OSNC7__CONFIG_MENU__PROGRAM_SCHEDULE )
	{
		//
		// SCREEN - Program Schedule
		//
		switch ( NWSUTILS__CurrentClimateControlMode() )
		{
			case NWSUTILS__CCMODE__HEATING:
				// Heating mode
				HEATING_CONTROL__InitialiseDataForEntryIntoProgrammingMode( bStateChange );
				break;
	
			case NWSUTILS__CCMODE__COOLING:
				// Refrigerative cooling mode
				COOLING_CONTROL__InitialiseDataForEntryIntoProgrammingMode( bStateChange );
				break;

			default:
				break;
		}
	}
	// Add more config meuu updates here
	// .
	// .
	// .

	mNWSystem_NC7UI.PrevConfigMenuAccessState = mNWSystem_NC7UI.CurrConfigMenuAccessState;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Return the N-C7 specific current config menu access state.
 * @param:	None.
 * @retval:	TWBIFUTL__OPERATING_STATE_NC7.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
TWBIFUTL__OPERATING_STATE_NC7		NWS_ACCESS_USERIF_NC7__CurrentConfigMenuAccessState(void)
{
	return mNWSystem_NC7UI.CurrConfigMenuAccessState;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - Slave not registered display update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__UpdateDisplay_SlaveNotRegistered(void)
{
	UICOMMON__TopStatusDetails		TopStatus;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() )	{ return; }
	//
    // Top status area
	// - Initialize structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define required details
	TopStatus.pstrTextToDisplay = "Searching for master networker";
	TopStatus.bFlashText        = true;
	TopStatus.TextFlashRate     = UITFTDSP__FR__1HZ;
	TopStatus.pImage2			= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USERIF_NC7__IN__MENU_BTN];
#if defined(INCLUDE_HELP_MENU_OPTION)
	TopStatus.pImage3			= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USERIF_NC7__IN__HELP_BTN];
#endif
	// - Update
	UICOMMON__TopStatus_Update( &TopStatus );
	//
	// Other
	UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USERIF_NC7__IN__BRIVIS_LOGO] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - Slave not registered control update.
 * @param:	pNetworkerSystem - Pointer to networker system data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__UpdateControl_SlaveNotRegistered(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }

	switch ( (NWS_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_USERIF_NC7__TV__MENU_BTN:
   		case NWS_USERIF_NC7__TV__HELP_BTN:
   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
    		break;

        default:
        	break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )				// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (NWS_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
	    		case NWS_USERIF_NC7__TV__MENU_BTN:
	    			pNetworkerSystem->UserAccess.Control.ButtonPressed = NWS_ACCESS_COMMON__CB__ACCESS_CONFIG_MENU;
	    			break;

	    		case NWS_USERIF_NC7__TV__HELP_BTN:
		   			// TODO - Help button
					break;

		   		default:
					break;
			}
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates if UI utility access occurring.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool								NWS_ACCESS_USERIF_NC7__bUtilityAccessToUI(void)
{
	if ( mNWSystem_NC7UI.bScreenCaptureInProgress )
	{
		return true;
	}
	if ( mNWSystem_NC7UI.bWaitingAfterDisplayInit )
	{
		return true;
	}
	if ( mNWSystem_NC7UI.bImageDownloadInProgress )
	{
		return true;
	}

	return false;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates if screen capture in progress.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool								NWS_ACCESS_USERIF_NC7__bScreenCaptureInProgress(void)		// Added, NC7090_SCNCAP
{
	return mNWSystem_NC7UI.bScreenCaptureInProgress;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates UI utilities - Screen capture.
 * 								 - Delay after display init.
 * 								 - Image download.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__UtilityUpdate(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem)
{
	bool		bRedoImageDownload = false;
	//
	//-------------------------------------------------------------------------------------------------------------------
	// SCREEN CAPTURE CHECK
	//-------------------------------------------------------------------------------------------------------------------
	if ( UISCNCAP__ScreenCapture_bProcessActive() )
	{
		mNWSystem_NC7UI.bScreenCaptureInProgress = true;
	}
	else
	if ( mNWSystem_NC7UI.bScreenCaptureInProgress )
	{
		// Need to redo image download!
		mNWSystem_NC7UI.bScreenCaptureInProgress = false; bRedoImageDownload = true;
	}

	if ( mNWSystem_NC7UI.bScreenCaptureInProgress )
	{
		UISCNCAP__ScreenCapture_Update();
		return;
	}
	//
	//-------------------------------------------------------------------------------------------------------------------
	// DISPLAY RE-INIT
	//-------------------------------------------------------------------------------------------------------------------
	if ( !TFTDSPIF__bDisplayIsReady() )
	{
		return;
	}
	else
	if ( mNWSystem_NC7UI.bWaitingAfterDisplayInit )
	{
		if ( mNWSystem_NC7UI.DisplayWaitAfterInitTimeCounter.ulValue == 0 )
		{
			mNWSystem_NC7UI.bWaitingAfterDisplayInit = false; bRedoImageDownload = true;
		}
	}
	else
	if ( TFTDSPIF__bInitialised() )			// This is a one-shot...
	{
		// Start timer
		GENTIMDT__StartTimer_TB_10MS( 25, &mNWSystem_NC7UI.DisplayWaitAfterInitTimeCounter );

		mNWSystem_NC7UI.bWaitingAfterDisplayInit = true;
	}

	if ( mNWSystem_NC7UI.bWaitingAfterDisplayInit )
	{
		return;
	}
	//
	// Check for any image download to be done
	ImageDownloadUpdate( bRedoImageDownload, pNetworkerSystem );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - Any action to perform on the UI device before updating the display.
 * @param:	vulBackgroundColor - Background color.
 * 			pNetworkerSystem   - Pointer to networker system data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate(uint32_t vulBackgroundColor, NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem)
{
	if ( mNWSystem_NC7UI.bScreenCaptureInProgress )
	{
		return;
	}
	if ( ( mNWSystem_NC7UI.bWaitingAfterDisplayInit ) || ( !TFTDSPIF__bDisplayIsReady() ) )
	{
		return;
	}
	if ( mNWSystem_NC7UI.bImageDownloadInProgress )
	{
		return;
	}
	if ( UITFTDSP__bDisplayShutdownIsActive() )
	{
		return;
	}
	//
	// Only allow this when - No UI utility update
	//						- Display is not shutdown
	UICOMMON__TFTDisplayList_Init_NormalOperation( vulBackgroundColor );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - Any action to perform on the UI device after updating the display.
 * @param:	pNetworkerSystem - Pointer to networker system data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__ActionAfterUpdate(void)
{
	if ( mNWSystem_NC7UI.bScreenCaptureInProgress )
	{
		return;
	}
	if ( ( mNWSystem_NC7UI.bWaitingAfterDisplayInit ) || ( !TFTDSPIF__bDisplayIsReady() ) )
	{
		return;
	}
	if ( mNWSystem_NC7UI.bImageDownloadInProgress )
	{
		return;
	}
	if ( UITFTDSP__bDisplayShutdownIsActive() )
	{
		return;
	}
	//
	// Only allow this when - No UI utility update
	//						- Display is not shutdown
	if ( UITIMING__bUpdateUI() )
	{
		UICOMMON__TFTDisplayList_Execute( FUNC_LOOP );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - Any common display update and control update action to be set.
 * @param:	pNetworkerSystem - Pointer to networker system data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__UpdateDisplayControl_Common(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem)
{
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	// Under STSP check if the schedule access being enabled by another networker - If so no bottom status update
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		if ( HEATING_CONTROL__bScheduleIsBeingProgrammed() || COOLING_CONTROL__bScheduleIsBeingProgrammed() ) { return; }
	}
	//
	// Init bottom status structure
	UICOMMON__BottomStatus_InitStructure();
	// Define what we need to show
	// - Only show the M indicator if this is the master
	if ( ( TWBIF502__bIsMasterType02Device ) && ( !NWS_SVCINF_CONTROL__ServiceInfoPrompt_bShowIt() ) )
	{
		UICOMMON__BottomStatus_UpdateImage( UI_COMMON__BSN_MASTER, false, 0 );
	}
    // - If locked out flash the padlock icon
	if ( NWSUTILS__bUserControlIsLockedOut() )
	{
		UICOMMON__BottomStatus_UpdateImage( UI_COMMON__BSN_LOCK, true, UITFTDSP__FR__1HZ );
	}
	//
	// Set flag to show bottom status
	UICOMMON__ShowBottomStatus( true );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - Any action to perform when all UI access completed.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__IndicateUserAccessCompleted(void)
{
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	// Under STSP check if the schedule access being enabled by another networker - If so no bottom status update
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		if ( HEATING_CONTROL__bScheduleIsBeingProgrammed() || COOLING_CONTROL__bScheduleIsBeingProgrammed() ) { return; }
	}
	//
	// Commit what is required for bottom status
	UICOMMON__BottomStatus_Update();
	
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - No appliance mode display update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__Mode_NoAppliance_UpdateDisplay(void)
{
	UICOMMON__TopStatusDetails		TopStatus;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) 				{ return; }
	// Service info prompt?
	if ( bViewingServiceInfoPrompt() )	{ return; }
	//
    // Top status area
	// - Initialize structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define required details
	TopStatus.pClock  				= RTMCLKIF__pClock();
	TopStatus.bClockIn12HourFormat  = TWBIFUTL__bClockDisplayFormatIn12HourFormat();
	TopStatus.pucOutdoorTemperature = NWSUTILS__pucAmbientTemperature();
#if defined(INCLUDE_HELP_MENU_OPTION)
	TopStatus.pImage2 				= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USERIF_NC7__IN__HELP_BTN];
#endif
	TopStatus.pImage3 				= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USERIF_NC7__IN__MENU_BTN];
	// - Update
	UICOMMON__TopStatus_Update( &TopStatus );
	//
	// Show room temperature
	UICOMMON__ShowRoomTemperature( TEMPMSIF__ucMeasuredTemperature( TWBIFUTL__bTemperatureDisplayFormatInCelsius() ) );
	//
	// Error  - Thermistor?
	//        - Bus power?
	if ( TEMPMSIF__bTemperatureMeasurementThermistorIsFaulty() )
	{
		mServiceInfo.MessageType 	 = NWS_SVCINF_COMMON__MT__FAULT_TRANSIENT;
		mServiceInfo.GroupID     	 = TWBIF502__pAddressOfThisUnit()->GroupID;
		mServiceInfo.UnitID      	 = TWBIF502__pAddressOfThisUnit()->UnitID;
		mServiceInfo.ucFaultCode 	 = TWBIF502__ERRORCODE_THERMISTOR;
		//
		// Show alert image
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USERIF_NC7__IN__ALERT_ICON] );
		// Flash Attention text
		UITFTDSP__FlashTextAbsolute( UITFTDSP__FR__2HZ, (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_USERIF_NC7__TN__ATTENTION] );
	}
	//
	// Other
	UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USERIF_NC7__IN__BRIVIS_LOGO] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_USERIF_NC7__TN_NO_APPLIANCE] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI Update - No appliance mode control update.
 * @param:	pNetworkerSystem - Pointer to networker system data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								NWS_ACCESS_USERIF_NC7__Mode_NoAppliance_UpdateControl(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }

	switch ( (NWS_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_USERIF_NC7__TV__MENU_BTN:
   		case NWS_USERIF_NC7__TV__HELP_BTN:
		case NWS_USERIF_NC7__TV__ALERT_BTN:
   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
    		break;

        default:
        	break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )				// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (NWS_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
	    		case NWS_USERIF_NC7__TV__MENU_BTN:
	    			pNetworkerSystem->UserAccess.Control.ButtonPressed = NWS_ACCESS_COMMON__CB__ACCESS_CONFIG_MENU;
	    			break;

	    		case NWS_USERIF_NC7__TV__ALERT_BTN:
	    			NWS_SVCINF_CONTROL__ServiceInfoPrompt_FlagToShowIt( &mServiceInfo );
	    			break;

	    		case NWS_USERIF_NC7__TV__HELP_BTN:
		   			// TODO - Help button
					break;

		   		default:
					break;
			}
		}
	}

	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to allow images to be downloaded to the display device.
 * @param:	vbRedoDownload	 - Override internal statics and perform a download if this flag is true.
 * 			pNetworkerSystem - Pointer to networker system data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							ImageDownloadUpdate(bool vbRedoDownload, NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem)
{
	static	bool sbDownloadedCommonImages 		     = false;
	static  bool sbModeSpecificImageDownloadRequired = false;

	bool	bDownloadModeSpecificImages = false;
	//
	// Check if common images to be downloaded
	if ( ( !sbDownloadedCommonImages ) || ( vbRedoDownload ) )
	{
		UITFTDSP_IMAGE__SingleImageUpdate_Stop();

		mNWSystem_NC7UI.bImageDownloadInProgress = true; UITFTDSP_IMAGE__UpdateFT800GRAM_CommonImages( FUNC_LOOP ); mNWSystem_NC7UI.bImageDownloadInProgress = false;

		sbDownloadedCommonImages = true;

		sbModeSpecificImageDownloadRequired = true;
	}
	//
	// In test sequence there is no mode, so check for NULL
	if ( pNetworkerSystem == 0 )
	{
		return;
	}
	//
	// Now check for mode specific images to be downloaded
	if ( pNetworkerSystem->bModeChangeHasOccurred )
	{
		if ( sbDownloadedCommonImages )
		{
			// Common images have been downloaded - Allow mode specific images to be doenloaded
			sbModeSpecificImageDownloadRequired = false;

			bDownloadModeSpecificImages = true;
		}
		else
		{
			// Common images not downloaded yet - Retain fact that mode specific images yet to be downloaded
			sbModeSpecificImageDownloadRequired = true;

			bDownloadModeSpecificImages = false;
		}

		pNetworkerSystem->bModeChangeHasOccurred = false;
	}
	else
	if ( sbModeSpecificImageDownloadRequired )
	{
		bDownloadModeSpecificImages = sbDownloadedCommonImages;
	}
	else
	{
		bDownloadModeSpecificImages = false;
	}
	//
	// Check if mode specific images to be downloaded
	if ( ( !bDownloadModeSpecificImages ) && ( !vbRedoDownload ) )
	{

	}
	else
	{
		switch ( pNetworkerSystem->CurrClimateControlMode )
		{
			case NWSUTILS__CCMODE__HEATING:
				// Mode - Heating
				// - Stop any single image download occurring
				UITFTDSP_IMAGE__SingleImageUpdate_Stop();

				mNWSystem_NC7UI.bImageDownloadInProgress = true; UITFTDSP_IMAGE__UpdateFT800GRAM_HeatingImages( FUNC_LOOP ); mNWSystem_NC7UI.bImageDownloadInProgress = false;

				sbModeSpecificImageDownloadRequired = false;
				break;

			case NWSUTILS__CCMODE__EVPCOOL:
				// Mode - Evap Cooling
				// - Stop any single image download occurring
				UITFTDSP_IMAGE__SingleImageUpdate_Stop();

				mNWSystem_NC7UI.bImageDownloadInProgress = true; UITFTDSP_IMAGE__UpdateFT800GRAM_EvpCoolImages( FUNC_LOOP ); mNWSystem_NC7UI.bImageDownloadInProgress = false;

				sbModeSpecificImageDownloadRequired = false;
				break;

			case NWSUTILS__CCMODE__COOLING:
				// Mode - Cooling
				// - Stop any single image download occurring
				UITFTDSP_IMAGE__SingleImageUpdate_Stop();

				mNWSystem_NC7UI.bImageDownloadInProgress = true; UITFTDSP_IMAGE__UpdateFT800GRAM_CoolingImages( FUNC_LOOP ); mNWSystem_NC7UI.bImageDownloadInProgress = false;

				sbModeSpecificImageDownloadRequired = false;
				break;

			default:
				break;
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks if service prompt screen is being displayed.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static bool						bViewingServiceInfoPrompt(void)
{
	bool		bViewingServiceInfo = false;

	if ( NWS_SVCINF_CONTROL__ServiceInfoPrompt_bShowIt() )
	{
		NWS_SVCINF_CONTROL__UserifUpdate();

		bViewingServiceInfo = true;
	}

	return bViewingServiceInfo;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - None.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							RapidUpdateControl_None(void)
{
	UITFTDSP__TouchControl_RapidUpdateDataInit();
	return;
}
//*********************************************************************************************************************************************************************************
