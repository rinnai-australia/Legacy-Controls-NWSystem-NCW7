//*********************************************************************************************************************************************************************************
// nws_csetup_control.c
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
#include		<nws_csetup_common.h>
#include		<nws_prmaxs_control.h>
#include		<nws_prompt_control.h>
#include		<heating_control.h>
#include		<evpcool_control.h>
#include		<cooling_control.h>
#include		<twbif502.h>
#include		<nwsutils.h>

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
#include		<nws_csetup_userif_NC7.h>
#include		<twbif502.h>
#include		<nvmifmgr.h>
#include		<tftdspif.h>
#endif

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static NWS_CSETUP_COMMON__ControlDetails	mCustomSetup;
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
static void						UserifUpdate(bool vbStateJustEntered);
static void						AccessParameters(void);
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
 * @brief:	Returns a pointer to the address of the device of which the parameters will be accessed.
 * @param:	None.
 * @retval:	TWBCOMIF__TWBDeviceAddress*.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
TWBCOMIF__TWBDeviceAddress*				NWS_CSETUP_CONTROL__pDeviceAddress(void)
{
	return &mCustomSetup.DeviceAddress;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the device type selected.
 * @param:	None.
 * @retval:	NWS_PRMAXS_COMMON__DEVICE_TYPE.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
NWS_PRMAXS_COMMON__DEVICE_TYPE			NWS_CSETUP_CONTROL__DeviceTypeSelected(void)
{
	return mCustomSetup.DeviceTypeSelected;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the Type02 device type selected.
 * @param:	None.
 * @retval:	NWS_PRMAXS_COMMON__02_DEVICE_TYPE.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
NWS_PRMAXS_COMMON__02_DEVICE_TYPE		NWS_CSETUP_CONTROL__Type02DeviceSelected(void)
{
	return mCustomSetup.DeviceType02Selected;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the parameter type being accessed.
 * @param:	None.
 * @retval:	NWS_PRMAXS_COMMON__ACCESS_PARAM_TYPE.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
NWS_PRMAXS_COMMON__ACCESS_PARAM_TYPE	NWS_CSETUP_CONTROL__ParameterTypeBeingAccessed(void)
{
	return mCustomSetup.UserAccess.ParameterType;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Module data initialisation.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									NWS_CSETUP_CONTROL__Init(void)
{
	mCustomSetup.UserAccess.bRestoringDefaults = false;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when defaults are in the process of being restored.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool									NWS_CSETUP_CONTROL__bRestoringDefaults(void)
{
	return mCustomSetup.UserAccess.bRestoringDefaults;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises data ready for processing once this state is entered.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									NWS_CSETUP_CONTROL__InitBeforeStateEntry(void)
{
	// Allow user to select type
	mCustomSetup.UserAccess.bSelectUnit        = false;
	mCustomSetup.UserAccess.bRestoringDefaults = false;

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_CSETUP_USERIF_NC7__InitBeforeStateEntry( &mCustomSetup );
#endif
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Custom setup menu state (Installer).
 * @param:	vbStateJustEntered - Indicates if this state has just been entered (first time in this fcn).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									NWS_CSETUP_CONTROL__UserifUpdate_Install(bool vbStateJustEntered)
{
	mCustomSetup.UserAccess.ParameterType = NWS_PRMAXS_COMMON__APT__INSTALL;

	UserifUpdate( vbStateJustEntered );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Custom setup menu state (Service).
 * @param:	vbStateJustEntered - Indicates if this state has just been entered (first time in this fcn).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									NWS_CSETUP_CONTROL__UserifUpdate_Service(bool vbStateJustEntered)
{
	mCustomSetup.UserAccess.ParameterType = NWS_PRMAXS_COMMON__APT__SERVICE;

	UserifUpdate(vbStateJustEntered);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags networker defaults/address is being resotre.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									NWS_CSETUP_CONTROL__FlagRestoringDefaults(void)
{
	// Flag that defaults/nwk addx is being restored
	mCustomSetup.UserAccess.bRestoringDefaults = true;
	return;
}
//*********************************************************************************************************************************************************************************
//
//
//*********************************************************************************************************************************************************************************
//	MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Custom setup menu state.
 * @param:	vbStateJustEntered - Indicates if this state has just been entered (first time in this fcn).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								UserifUpdate(bool vbStateJustEntered)
{
	TWBCOMIF__UNITID		UnitID;
	bool					bAllowParamAccess;

	if ( NWS_PROMPT_CONTROL__InfoPrompt_bShowIt() )
	{
		NWS_PROMPT_CONTROL__UserifUpdate();
		return;
	}
	else
	if ( NWS_PROMPT_CONTROL__InfoPrompt_bWasBeingDisplayed() )			// Check if was being displayed to check if Yes/No pressed, emilio 21-12-15
	{
		if ( NWS_PROMPT_CONTROL__InfoPrompt_bButtonPressedYes() )
		{
			if ( mCustomSetup.UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__SERVICE )
			{
				// Restore all defaults
				NVMIFMGR__BeginClearProcess_AllSettings(true);
			}
			else
			{
				// Reset networker ID and start parameter save
				*TWBIF502__kThisType02DeviceUnitID.pucActual = 0; NVMIFMGR__BeginSaveProcess_Parameters( true );
			}
			// Flag that defaults/nwk addx is being restored
			mCustomSetup.UserAccess.bRestoringDefaults = true;
		}
	}
	//
	// Init any data when state just entered
	if ( vbStateJustEntered ) { NWS_CSETUP_CONTROL__InitBeforeStateEntry(); }
	//
	// Initialise data for display
	NWS_PRMAXS_CONTROL__UpdateDevicesInfo();
	//
	// Clear any button press
	mCustomSetup.UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__NONE_PRESSED;
	//
	// User inteface update
#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_CSETUP_USERIF_NC7__UpdateDisplay( &mCustomSetup );
	NWS_CSETUP_USERIF_NC7__UpdateControl( &mCustomSetup );
#endif
	//
	// Process any control actions
	switch ( mCustomSetup.UserAccess.Control.ButtonPressed )
	{
		case NWS_CSETUP_COMMON__CB__NETWORKER_PARAMS:
		case NWS_CSETUP_COMMON__CB__COOLER_PARAMS:
		case NWS_CSETUP_COMMON__CB__HEATER_PARAMS:
			if ( !TWBIF502__bIsMasterType02Device )
			{
				// Not the master - No access allowed
				if ( mCustomSetup.UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__INSTALL )
				{
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__INSTALL_AXS_MASTER );
				}
				else
				{
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__SERVICE_AXS_MASTER );
				}

				bAllowParamAccess = false;
			}
			else
			if ( ( !HEATING_CONTROL__bCurrentlySwitchedOff() ) || ( !EVPCOOL_CONTROL__bCurrentlySwitchedOff() ) || ( !COOLING_CONTROL__bCurrentlySwitchedOff() ) )
			{
				// Not switched off - No access allowed
				if ( mCustomSetup.UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__INSTALL )
				{
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__INSTALL_AXS_SWITCH_OFF );
				}
				else
				{
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__SERVICE_AXS_SWITCH_OFF );
				}

				bAllowParamAccess = false;
			}
			else
			{
				bAllowParamAccess = true;
			}

		default:
			break;
	}

	switch ( mCustomSetup.UserAccess.Control.ButtonPressed )
	{
		case NWS_CSETUP_COMMON__CB__NETWORKER_PARAMS:
			if ( bAllowParamAccess )
			{
				// Networker parameters
				mCustomSetup.DeviceTypeSelected = NWS_PRMAXS_COMMON__DT__NETWORKER;
				// Check if more than one device
				if ( NWS_PRMAXS_CONTROL__bMoreThanOneDeviceIsAvailable( NWS_PRMAXS_COMMON__DT__NETWORKER ) )
				{
					// Type02 : User to select the unit
					mCustomSetup.DeviceAddress.GroupID = TWBCOMIF__GID__GRP02;
					mCustomSetup.UserAccess.bSelectUnit   = true;
					// Sort of re-entering state so re-init
					NWS_CSETUP_USERIF_NC7__InitBeforeStateEntry( &mCustomSetup );
				}
				else
				{
					// Type02 : Unit 1 always
					mCustomSetup.DeviceAddress.GroupID = TWBCOMIF__GID__GRP02;
					mCustomSetup.DeviceAddress.UnitID  = 1;
					// Now indicate to access params
					AccessParameters();
				}
			}
			break;

		case NWS_CSETUP_COMMON__CB__COOLER_PARAMS:
			if ( bAllowParamAccess )
			{
				// Cooler installer parameters
				mCustomSetup.DeviceTypeSelected = NWS_PRMAXS_COMMON__DT__COOLER;
				// Check if more than one device
				if ( NWS_PRMAXS_CONTROL__bMoreThanOneDeviceIsAvailable( NWS_PRMAXS_COMMON__DT__COOLER ) )
				{
					// Type01 : User to select the unit
					mCustomSetup.DeviceAddress.GroupID  = TWBCOMIF__GID__GRP01;
					mCustomSetup.UserAccess.bSelectUnit = true;
					// Sort of re-entering state so re-init
					NWS_CSETUP_USERIF_NC7__InitBeforeStateEntry( &mCustomSetup );
				}
				else
				if ( ( ( UnitID = NWS_PRMAXS_CONTROL__DeviceFirstAvailableUnit( NWS_PRMAXS_COMMON__DT__COOLER ) ) ) != 0 )
				{
					// Type01 : First unit available
					mCustomSetup.DeviceAddress.GroupID = TWBCOMIF__GID__GRP01;
					mCustomSetup.DeviceAddress.UnitID  = UnitID;
					// Now indicate to access params
					AccessParameters();
				}
				else
				{
					// No Access!
				}
			}
			break;

		case NWS_CSETUP_COMMON__CB__HEATER_PARAMS:
			if ( bAllowParamAccess )
			{
				// Heater installer parameters
				mCustomSetup.DeviceTypeSelected = NWS_PRMAXS_COMMON__DT__HEATER;
				// Check if more than one device
				if ( NWS_PRMAXS_CONTROL__bMoreThanOneDeviceIsAvailable( NWS_PRMAXS_COMMON__DT__HEATER ) )
				{
					// Type03 : User to select the unit
					mCustomSetup.DeviceAddress.GroupID  = TWBCOMIF__GID__GRP03;
					mCustomSetup.UserAccess.bSelectUnit = true;
					// Sort of re-entering state so re-init
					NWS_CSETUP_USERIF_NC7__InitBeforeStateEntry( &mCustomSetup );
				}
				else
				if ( ( ( UnitID = NWS_PRMAXS_CONTROL__DeviceFirstAvailableUnit( NWS_PRMAXS_COMMON__DT__HEATER ) ) ) != 0 )
				{
					// Type03 : First unit available
					mCustomSetup.DeviceAddress.GroupID = TWBCOMIF__GID__GRP03;
					mCustomSetup.DeviceAddress.UnitID  = UnitID;
					// Now indicate to access params
					AccessParameters();
				}
				else
				{
					// No Access!
				}
			}
			break;

		case NWS_CSETUP_COMMON__CB__UNIT_SELECTED:
			// Define device type
			if ( mCustomSetup.DeviceTypeSelected == NWS_PRMAXS_COMMON__DT__COOLER )
			{
			}
			else
			if ( mCustomSetup.DeviceTypeSelected == NWS_PRMAXS_COMMON__DT__HEATER )
			{
			}
			else
			if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsMasterControl( mCustomSetup.UserAccess.ucUnitIDSelected ) )
			{
				mCustomSetup.DeviceType02Selected = NWS_PRMAXS_COMMON__02DT__CONTROL;
			}
			else
			if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsSlaveControl( mCustomSetup.UserAccess.ucUnitIDSelected ) )
			{
				mCustomSetup.DeviceType02Selected = NWS_PRMAXS_COMMON__02DT__CONTROL;
			}
			else
			if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsSensor( mCustomSetup.UserAccess.ucUnitIDSelected ) )
			{
				mCustomSetup.DeviceType02Selected = NWS_PRMAXS_COMMON__02DT__SENSOR;
			}
			else
			if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsOutdoorSensor( mCustomSetup.UserAccess.ucUnitIDSelected ) )
			{
				mCustomSetup.DeviceType02Selected = NWS_PRMAXS_COMMON__02DT__SENSOR_OUTDOOR;
			}
			else
			if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsRemoteAccessModule( mCustomSetup.UserAccess.ucUnitIDSelected ) )
			{
				mCustomSetup.DeviceType02Selected = NWS_PRMAXS_COMMON__02DT__REMOTE_ACCESS;
			}
			// Set the device unit ID being accessed
			mCustomSetup.DeviceAddress.UnitID = (TWBCOMIF__UNITID)mCustomSetup.UserAccess.ucUnitIDSelected;
			// Now indicate to access params
			AccessParameters();
			break;

		case NWS_CSETUP_COMMON__CB__RESTORE_DEFAULTS:
			if (mCustomSetup.UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__SERVICE)
			{

				// Info prompt - check if user is sure to restore defaults
				NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__RESTORE_DEFAULTS );
			}
			else
			{
				// Info prompt - check if user is sure to reset networker ID
				NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__RESET_NETWORKER_ID );
			}
			break;

		case NWS_CSETUP_COMMON__CB__BACK:
			// Back button
			if ( mCustomSetup.UserAccess.bSelectUnit )
			{
				mCustomSetup.UserAccess.bSelectUnit = false;		// Selecting unit so cancel
			}
			else
			if ( mCustomSetup.UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__INSTALL )
			{
				NWSUTILS__RevertConfigMenu();
			}
			else
			{
				NWSUTILS__RevertConfigMenu();						// Back to config menu
			}
			break;

		default:
#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
			if ( TFTDSPIF__bNoTouchTimeout() ) { NWSUTILS__CancelConfigMenu(); }
#endif
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flag which parameters to access.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						AccessParameters(void)
{
	if ( mCustomSetup.UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__INSTALL )
	{
		NWSUTILS__AccessConfigMenu_InstallerParamAccess();
	}
	else
	{
		NWSUTILS__AccessConfigMenu_ServiceParamAccess();
	}
	return;
}
//*********************************************************************************************************************************************************************************
