//*********************************************************************************************************************************************************************************
// nws_usrprf_control.c
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
#include		<genutils.h>
#include		<nws_usrprf_common.h>
#include		<heating_control.h>
#include		<cooling_control.h>
#include		<evpcool_control.h>
#include		<nwsutils.h>
#include		<twbifutl.h>
#include		<twbif502.h>

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
#include		<nws_prompt_control.h>
#include		<nws_usrprf_userif_NC7.h>
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
static NWS_USRPRF_COMMON__ControlDetails	mUserPreference;
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
void							NWS_USRPRF_CONTROL__InitBeforeStateEntry(void)
{
	NWS_USRPRF_USERIF_NC7__InitBeforeStateEntry( &mUserPreference );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - User setting state.
 * @param:	vbStateJustEntered - State just entered flag.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_USRPRF_CONTROL__UserifUpdate(bool vbStateJustEntered)
{
	TWBIFUTL__ZONE	Zone;
	bool			bZoneInstalled = false;
	//
	// Initialise data
	// - Clear any button press
	mUserPreference.UserAccess.Control.ButtonPressed = NWS_USRPRF_COMMON__CB__NONE_PRESSED;
	//
	// User inteface update
	if ( NWS_PROMPT_CONTROL__InfoPrompt_bShowIt() )
	{
		NWS_PROMPT_CONTROL__UserifUpdate();
		return;
	}
	NWS_USRPRF_USERIF_NC7__UpdateDisplay( &mUserPreference );
	NWS_USRPRF_USERIF_NC7__UpdateControl( &mUserPreference );
	//
	// Process any control actions
	switch ( mUserPreference.UserAccess.Control.ButtonPressed )
	{
		case NWS_USRPRF_COMMON__CB__KEYPAD_LOCKOUT:
			// Keypad lockout button
			if ( ( !TWBIF502__bSettingOfAddressBeingForced() ) && ( !TWBIF502__bThisUnitIsRegisteredWithTheMaster() ) )
			{
				// N-C7 not registered
				if ( NWSUTILS__bUserControlIsLockedOut() )
				{
					// System can only be unlocked by a registered networker
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__SYST_UNLOCK_REGNWK );
				}
				else
				{
					// System can only be locked out by a registered networker
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__SYST_LOKOUT_REGNWK );
				}
			}
			else
			{
				// N-C7 registered
				NWSUTILS__AccessConfigMenu_UserPreferences_KeypadLockout();
			}
			break;

		case NWS_USRPRF_COMMON__CB__GENERAL_SETTINGS:
			// Set schedule periods
			NWSUTILS__AccessConfigMenu_UserPreferences_GeneralSettings();
			break;

		case NWS_USRPRF_COMMON__CB__SYSTEM_SETTINGS:
			if ( !TWBIF502__bIsMasterType02Device )
			{
				// Not the master - No access allowed
				NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__SYST_SETTINGS_MASTER );
			}
			else
			{
				NWSUTILS__AccessConfigMenu_UserPreferences_SystemSettings();
			}
			break;

		case NWS_USRPRF_COMMON__CB__SET_ZONE_NAMES:
			// Set zone names
			if (!TWBIF502__bIsMasterType02Device)
			{
				// Not the master - No access allowed
				NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__ZONE_NAMES_MASTER );
			}
			else
			{
				for ( Zone = TWBIFUTL__ZONE__A; Zone <= TWBIFUTL__ZONE__D; Zone++ )
				{
					if ( ( HEATING_CONTROL__bZoneInstalled(Zone) ) || ( EVPCOOL_CONTROL__bZoneInstalled(Zone) ) || ( COOLING_CONTROL__bZoneInstalled(Zone) ) )
					{
						bZoneInstalled = true;
						break;
					}
				}

				if ( !bZoneInstalled )
				{
					NWS_PROMPT_CONTROL__InfoPrompt_FlagToShowIt( NWS_PROMPT_COMMON__MSG__ZONE_NAMES_NO_ZONES );
				}
				else
				{
					NWSUTILS__AccessConfigMenu_UserPreferences_ZoneNames();
				}
			}
			break;

		case NWS_USRPRF_COMMON__CB__BACK:
			NWSUTILS__RevertConfigMenu();
			break;

		default:
			if ( TFTDSPIF__bNoTouchTimeout() ) { NWSUTILS__CancelConfigMenu(); }
			break;
	}

	return;
}
//*********************************************************************************************************************************************************************************
