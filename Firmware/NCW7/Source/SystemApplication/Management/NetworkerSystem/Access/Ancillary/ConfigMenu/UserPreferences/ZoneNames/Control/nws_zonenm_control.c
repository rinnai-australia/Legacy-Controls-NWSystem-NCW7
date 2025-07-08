//*********************************************************************************************************************************************************************************
// nws_zonenm_control.c
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
#include		<nws_zonenm_common.h>
#include		<heating_control.h>
#include		<evpcool_control.h>
#include		<cooling_control.h>
#include		<nvmifmgr.h>
#include		<nwsutils.h>

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
#include		<nws_zonenm_userif_NC7.h>
#include		<zoning_userif_NC7.h>
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
static char									mstrNewZoneNameDescription[ZONING_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH+1];
static NWS_ZONENM_COMMON__ControlDetails	mZoneNamesSetup;
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
void							NWS_ZONENM_CONTROL__InitBeforeStateEntry(void)
{
	// Define zone information
	// - ZoneA
	mZoneNamesSetup.UserAccess.ZoneA.pDescription = ZONING_USERIF_NC7__pZoneDescription(TWBIFUTL__ZONE__A);
	mZoneNamesSetup.UserAccess.ZoneA.bInstalled   = EVPCOOL_CONTROL__bZoneAInstalled() || HEATING_CONTROL__bZoneAInstalled() || COOLING_CONTROL__bZoneAInstalled();
	// - ZoneB
	mZoneNamesSetup.UserAccess.ZoneB.pDescription = ZONING_USERIF_NC7__pZoneDescription(TWBIFUTL__ZONE__B);
	mZoneNamesSetup.UserAccess.ZoneB.bInstalled   = EVPCOOL_CONTROL__bZoneBInstalled() || HEATING_CONTROL__bZoneBInstalled() || COOLING_CONTROL__bZoneBInstalled();
	// - ZoneC
	mZoneNamesSetup.UserAccess.ZoneC.pDescription = ZONING_USERIF_NC7__pZoneDescription(TWBIFUTL__ZONE__C);
	mZoneNamesSetup.UserAccess.ZoneC.bInstalled   = EVPCOOL_CONTROL__bZoneCInstalled() || HEATING_CONTROL__bZoneCInstalled() || COOLING_CONTROL__bZoneCInstalled();
	// - ZoneD
	mZoneNamesSetup.UserAccess.ZoneD.pDescription = ZONING_USERIF_NC7__pZoneDescription(TWBIFUTL__ZONE__D);
	mZoneNamesSetup.UserAccess.ZoneD.bInstalled   = EVPCOOL_CONTROL__bZoneDInstalled() || HEATING_CONTROL__bZoneDInstalled() || COOLING_CONTROL__bZoneDInstalled();
	//
	// Init access details
	mZoneNamesSetup.UserAccess.bZoneNameBeingEntered = false;
	// Init where the string will be entered
	mZoneNamesSetup.UserAccess.NewZoneName.pstrText      = (char*)&mstrNewZoneNameDescription[0];
	mZoneNamesSetup.UserAccess.NewZoneName.uiMaxLength   = ZONING_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH;
	mZoneNamesSetup.UserAccess.NewZoneName.uiLength	     = 0;
	mZoneNamesSetup.UserAccess.NewZoneName.bUsingDefault = false;
	mZoneNamesSetup.UserAccess.NewZoneName.pstrText[0]   = '\0';

	NWS_ZONENM_USERIF_NC7__InitBeforeStateEntry( &mZoneNamesSetup );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Zone names state.
 * @param:	vbStateJustEntered - Indicates if this state has just been entered (first time in this fcn).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_ZONENM_CONTROL__UserifUpdate(bool vbStateJustEntered)
{
	// Init any data when state just entered
	if ( vbStateJustEntered ) { NWS_ZONENM_CONTROL__InitBeforeStateEntry(); }
	//
	// Clear any button press
	mZoneNamesSetup.UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__NONE_PRESSED;
	//
	// User inteface update
	NWS_ZONENM_USERIF_NC7__UpdateDisplay( &mZoneNamesSetup );
	NWS_ZONENM_USERIF_NC7__UpdateControl( &mZoneNamesSetup );
	//
	// Process any control actions
	switch ( mZoneNamesSetup.UserAccess.Control.ButtonPressed )
	{
		case NWS_ZONENM_COMMON__CB__ZONE_A_SELECTED:
			// Zone A selected for entry
			// - Make a copy
			GENSTRDT__CopyGENString( mZoneNamesSetup.UserAccess.ZoneA.pDescription, &mZoneNamesSetup.UserAccess.NewZoneName );
			// - Flag zone name to be entered
			mZoneNamesSetup.UserAccess.ZoneBeingUpdated = TWBIFUTL__ZONE__A; mZoneNamesSetup.UserAccess.bZoneNameBeingEntered = true;
			break;

		case NWS_ZONENM_COMMON__CB__ZONE_B_SELECTED:
			// Zone B selected for entry
			// - Make a copy
			GENSTRDT__CopyGENString( mZoneNamesSetup.UserAccess.ZoneB.pDescription, &mZoneNamesSetup.UserAccess.NewZoneName );
			// - Flag zone name to be entered
			mZoneNamesSetup.UserAccess.ZoneBeingUpdated = TWBIFUTL__ZONE__B; mZoneNamesSetup.UserAccess.bZoneNameBeingEntered = true;
			break;

		case NWS_ZONENM_COMMON__CB__ZONE_C_SELECTED:
			// Zone C selected for entry
			// - Make a copy
			GENSTRDT__CopyGENString( mZoneNamesSetup.UserAccess.ZoneC.pDescription, &mZoneNamesSetup.UserAccess.NewZoneName );
			// - Flag zone name to be entered
			mZoneNamesSetup.UserAccess.ZoneBeingUpdated = TWBIFUTL__ZONE__C; mZoneNamesSetup.UserAccess.bZoneNameBeingEntered = true;
			break;

		case NWS_ZONENM_COMMON__CB__ZONE_D_SELECTED:
			// Zone D selected for entry
			// - Make a copy
			GENSTRDT__CopyGENString( mZoneNamesSetup.UserAccess.ZoneD.pDescription, &mZoneNamesSetup.UserAccess.NewZoneName );
			// - Flag zone name to be entered
			mZoneNamesSetup.UserAccess.ZoneBeingUpdated = TWBIFUTL__ZONE__D; mZoneNamesSetup.UserAccess.bZoneNameBeingEntered = true;
			break;

		case NWS_ZONENM_COMMON__CB__ZONE_NAME_DONE:
			// Zone name entry done
			// - Copy back
			if ( mZoneNamesSetup.UserAccess.ZoneBeingUpdated == TWBIFUTL__ZONE__A )
			{
				GENSTRDT__CopyGENString( &mZoneNamesSetup.UserAccess.NewZoneName, mZoneNamesSetup.UserAccess.ZoneA.pDescription );

				ZONING_USERIF_NC7__FlagZoneNameAltered( TWBIFUTL__ZONE__A );
			}
			else
			if ( mZoneNamesSetup.UserAccess.ZoneBeingUpdated == TWBIFUTL__ZONE__B )
			{
				GENSTRDT__CopyGENString( &mZoneNamesSetup.UserAccess.NewZoneName, mZoneNamesSetup.UserAccess.ZoneB.pDescription );

				ZONING_USERIF_NC7__FlagZoneNameAltered( TWBIFUTL__ZONE__B );
			}
			else
			if ( mZoneNamesSetup.UserAccess.ZoneBeingUpdated == TWBIFUTL__ZONE__C )
			{
				GENSTRDT__CopyGENString( &mZoneNamesSetup.UserAccess.NewZoneName, mZoneNamesSetup.UserAccess.ZoneC.pDescription );

				ZONING_USERIF_NC7__FlagZoneNameAltered( TWBIFUTL__ZONE__C );
			}
			else
			if ( mZoneNamesSetup.UserAccess.ZoneBeingUpdated == TWBIFUTL__ZONE__D )
			{
				GENSTRDT__CopyGENString( &mZoneNamesSetup.UserAccess.NewZoneName, mZoneNamesSetup.UserAccess.ZoneD.pDescription );

				ZONING_USERIF_NC7__FlagZoneNameAltered( TWBIFUTL__ZONE__D );
			}
			// - Cancel entry screen
			mZoneNamesSetup.UserAccess.bZoneNameBeingEntered = false;
			break;

		case NWS_ZONENM_COMMON__CB__ZONE_NAME_CANCEL:
			// Zone name entry cancelled
			mZoneNamesSetup.UserAccess.bZoneNameBeingEntered = false;
			break;

		case NWS_ZONENM_COMMON__CB__BACK:
			// Back button
			// - Save updated zone names
			NVMIFMGR__BeginSaveProcess_SavedStrings();
			// - Revert back to user preferences screen
			NWSUTILS__RevertConfigMenu_UserPreferences();
			break;

		default:
			if ( TFTDSPIF__bNoTouchTimeout() ) { NWSUTILS__CancelConfigMenu(); }
			break;
	}

	return;
}
//*********************************************************************************************************************************************************************************
