//*********************************************************************************************************************************************************************************
// nws_cfgmnu_control.c
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
#include		<nws_cfgmnu_common.h>
#include		<nws_prompt_control.h>
#include		<nwsutils.h>
#include		<twbif502.h>

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
#include		<nws_cfgmnu_userif_NC7.h>
#include		<heating_control.h>
#include		<cooling_control.h>
#include		<htgclg_userif_NC7.h>
#include		<tftdspif.h>
#endif

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			NWS_CFGMNU_CONTROL__NO_INPUT_TIMEOUT_1S		60			// Timeout when no user activity
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static NWS_CFGMNU_COMMON__ControlDetails	mConfigMenu;
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
//
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
 * @brief:	Initialises data ready for processing once this state is entered.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void						NWS_CFGMNU_CONTROL__InitBeforeStateEntry(void)
{
#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_CFGMNU_USERIF_NC7__InitBeforeStateEntry( &mConfigMenu );
#endif
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Config menu state.
 * @param:	vbStateJustEntered - Indicates if this state has just been entered (first time in this fcn).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void						NWS_CFGMNU_CONTROL__UserifUpdate(bool vbStateJustEntered)
{
	if ( NWS_PROMPT_CONTROL__InfoPrompt_bShowIt() )
	{
		NWS_PROMPT_CONTROL__UserifUpdate();
		return;
	}
	//
	// Init any data when state just entered
	if ( vbStateJustEntered ) { NWS_CFGMNU_CONTROL__InitBeforeStateEntry(); }
	//
	// Clear any button press
	mConfigMenu.UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__NONE_PRESSED;
	//
	// User inteface update
#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_CFGMNU_USERIF_NC7__UpdateDisplay( &mConfigMenu );
	NWS_CFGMNU_USERIF_NC7__UpdateControl( &mConfigMenu );
#endif
	//
	// Process any control actions
	switch ( mConfigMenu.UserAccess.Control.ButtonPressed )
	{
		case NWS_CFGMNU_COMMON__CB__CLEAN_SCREEN:
			NWSUTILS__AccessConfigMenu_CleanScreen();
			break;

		case NWS_CFGMNU_COMMON__CB__ACCESS_DIAGNOSTICS:
			NWSUTILS__AccessConfigMenu_ServiceInformation();
			break;

		case NWS_CFGMNU_COMMON__CB__INSTALLER_SETUP:
			NWSUTILS__AccessConfigMenu_InstallerSetup();
			break;

		case NWS_CFGMNU_COMMON__CB__PROGRAM_SCHEDULE:
			if ( NWSUTILS__bClimateControlModeIsEvaporativeCooling() )
			{
				// Schedule unavailable to evap cooling mode
				NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_EVPCOOL );
			}
			else
			if ( NWSUTILS__bNoClimateControlModeIsCurrentlyBeingAccessed() )
			{
				// Schedule unavailable to no control mode
				NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_NOCNTRL );
			}
			else
			{
				// Check if fan only mode being accessed in heating/cooling mode
				if ( HEATING_CONTROL__bFanOnlyModeIsCurrentlyBeingAccessed() || COOLING_CONTROL__bFanOnlyModeIsCurrentlyBeingAccessed() )
				{
					// - Schedule unavailable to fan only mode
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_FANONLY );
				}
				else
				{
					// - Show schedule 
					NWSUTILS__AccessConfigMenu_ProgramSchedule();
				}
			}
			// Set schedule access flag, jason 10-12-15
			HTGCLG_USERIF_NC7__EnableScheduleAccessByThisNetworker( true );
			break;

		case NWS_CFGMNU_COMMON__CB__CLOCK_SET:
			if ( !TWBIF502__bIsMasterType02Device )
			{
				NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__CLOCK_SET_MASTER );
			}
			else
			{
				NWSUTILS__AccessConfigMenu_SetClock();
			}
			break;

		case NWS_CFGMNU_COMMON__CB__USER_PREFERENCE_SET:
			NWSUTILS__AccessConfigMenu_UserPreferences();
			break;

		case NWS_CFGMNU_COMMON__CB__BACK:
			NWSUTILS__CancelConfigMenu();
			break;

		default:
			if ( TFTDSPIF__bNoTouchTimeout() ) { NWSUTILS__CancelConfigMenu(); }
			break;
	}

	return;
}
//*********************************************************************************************************************************************************************************
