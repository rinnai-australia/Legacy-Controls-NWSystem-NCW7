//*********************************************************************************************************************************************************************************
// cooling_userif_NC7.c
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
#include		<htgclg_common.h>
#include		<htgclg_userif_NC7.h>
#include		<nws_svcinf_control.h>
#include		<nwsmspcl.h>
#include		<nwsutils.h>
#include        <tftdspif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// String definition details - ** Array variables as text to display will vary so define here for pointer inclusion in text table **
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Set temperature text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define					COOLING_USERIF_NC7__SET_TEMPERATURE_TEXT_LENGTH		3
// - Actual container for string
static char				mstrSetTemperatureText[COOLING_USERIF_NC7__SET_TEMPERATURE_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mSetTemperatureString;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	COOLING_USERIF_NC7__IMAGE_NUMBER {

	COOLING_USERIF_NC7__IN__SNOWMAN_ICON = 0

} COOLING_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	// 						        ImageNumber,  ImageNumber_ActivePress,   X,   Y, Persistence time, TAG
	{ UITFTDSP_IMAGE__IN__COOLING__SNOWMAN_ICON, UITFTDSP_IMAGE__NO_IMAGE, 339,  92, 			 	0, -1 }
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaText[]
typedef enum	COOLING_USERIF_NC7__TEXT_NUMBER {

	COOLING_USERIF_NC7__TN__COOL = 0

} COOLING_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails				mkaText[] = {
	// 	X, 	 Y, XO, YO, 			    Color, 			  		 Font Size, 	      Justification, String Label
	{ 428,  58, -1, -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__LEFT, "COOL" }
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Level Button Definition
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaLevelButton[]
typedef enum	COOLING_USERIF_NC7__LEVEL_BUTTON_NUMBER {

	COOLING_USERIF_NC7__LB__SET_TEMP_BTN = 0

} COOLING_USERIF_NC7__LEVEL_BUTTON_NUMBER;
//
// Array of level buttons
static const UITFTDSP_BUTTON__LevelButtonDetails 	mkaLevelButton[] = {
	{
		// RECTANGLE						   	  				     TEXT1																											                TEXT2
		// 	   					   Color,   X,   Y,   W,   H,  LW 		 X,   Y,  XO,  YO, 						         Color,           	       FontSize,		  	 Justification, Text			X,   Y,  XO,  YO, 							    Color,          	      FontSize,		 	    Justification, Text
		{ { UITFTDSP_COLOR__BUTTON_OUTER, 274,  75,  56, 139,  18 }, {  -1,  -1,  -1,   3, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Set to" }, {  -1,  -1,  -1,  15, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, (char*)&mstrSetTemperatureText[0] } },
		// TOP BUTTON
		// 				  				   ImageNumber, 				    ImageNumber_ActivePress,   X,   Y, 						    Persistence time, TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE, 279, 114, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, HTGCLG_USERIF_NC7__TV__SET_TEMP_UP_BTN },
		// BOTTOM BUTTON
		// 				  				   ImageNumber, 					ImageNumber_ActivePress,   X,   Y, 						    Persistence time, TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE, 279, 164, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, HTGCLG_USERIF_NC7__TV__SET_TEMP_DN_BTN },
		// POINT
		//			      Color,   X,   Y, XO1, YO1, XO2, YO2, PointSize
		{ UITFTDSP_COLOR__WHITE,  -1,  -1,  37,  23,  42,  23, 3 }
	}
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
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
static void							UpdateDisplay_Mode_NormalOperation(HTGCLG_COMMON__ControlDetails* pCooling);
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
 * @brief:	User display update - N-C7 : Programming Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateDisplay_Mode_Programming(HTGCLG_COMMON__ControlDetails* pCooling)
{
	// Check if clean screen being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigMenu( TWBIFUTL__OSNC7__CONFIG_MENU__CLEAN_SCREEN ) ) { return; }

	HTGCLG_USERIF_NC7__UpdateDisplay_Mode_Programming( pCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Programming Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateControl_Mode_Programming(HTGCLG_COMMON__ControlDetails* pCooling)
{
	// Check if clean screen being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigMenu( TWBIFUTL__OSNC7__CONFIG_MENU__CLEAN_SCREEN ) ) { return; }

	HTGCLG_USERIF_NC7__UpdateControl_Mode_Programming( pCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User display update - N-C7 : Off Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateDisplay_Mode_Off(HTGCLG_COMMON__ControlDetails* pCooling)
{
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) 					{ return; }
	// Service info being displayed
	if ( HTGCLG_USERIF_NC7__bViewingServiceInfoPrompt( pCooling ) )	{ return; }
	//
	// UPDATE USER DISPLAY
	// -------------------
	// Normal Operation - Common Display
	UpdateDisplay_Mode_NormalOperation( pCooling );

    if ( pCooling->bSlaveModeSwitch ) { return; }
	//
	// Mode - SwitchOff Related
    HTGCLG_USERIF_NC7__UpdateDisplay_Mode_Off( pCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Off Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateControl_Mode_Off(HTGCLG_COMMON__ControlDetails* pCooling)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	// If mode switch get out
    if ( pCooling->bSlaveModeSwitch ) 			{ return; }
    //
    // Get the tag value
	ucTagValue = TFTDSPIF__ucTouchTagValue();

	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__MENU_BTN )			// Allow menu button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__HELP_BTN )			// Allow help button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( !NWSUTILS__bUserControlIsLockedOut() )													// Allow remaining only if not locked out
	{
		switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
		{
			case HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN:
				// Switch system on : Fan Only operation
				pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_FAN_ON;
				// No rapid update
				HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__ONOFF_BTN:
				// On/Off button - Switch sys off
				pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_OFF;
				// No rapid update
				HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__ALERT_BTN:
			case HTGCLG_USERIF_NC7__TV__ZONES_BTN:
			case HTGCLG_USERIF_NC7__TV__MODE_BTN:
	    		// Remember this tag as it causes screen to switch
	   			sucActiveTag = ucTagValue;
	   			// No rapid update
	    	    HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

	        default:
	        	break;
		}
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )				// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case HTGCLG_USERIF_NC7__TV__ALERT_BTN:
					NWS_SVCINF_CONTROL__ServiceInfoPrompt_FlagToShowIt( HTGCLG_USERIF_NC7__pServiceInfoDetails() );
					break;

				case HTGCLG_USERIF_NC7__TV__ZONES_BTN:
					// Zones button - Can only be displayed when MSP enabled so toggle zones
					NWSMSPCL__AccessNextAvailableZone();
					break;

				case HTGCLG_USERIF_NC7__TV__MODE_BTN:
					// Mode button - Switch climate control mode
					pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_MODE;
					break;

				case HTGCLG_USERIF_NC7__TV__MENU_BTN:
					// Menu button - Access config menu
					pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__ACCESS_CONFIG_MENU;
		   		    break;

				case HTGCLG_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	User display update - N-C7 : Fan Only Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateDisplay_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pCooling)
{
	bool		bZoningDoneButtonPressed = false, bZoningBackButtonPressed = false;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() )					 	{ return; }
	// Service info being displayed
	if ( HTGCLG_USERIF_NC7__bViewingServiceInfoPrompt( pCooling ) )	{ return; }
	//
	// Check if zoning screen is being accessed
	if ( pCooling->UserAccess.bZoningDetails )
	{
		// Do not show bottom status in zoning screen
		UICOMMON__ShowBottomStatus( false );
		//
		// Accessing zoning details to be programmed
		// - Display update
		ZONING_CONTROL__UpdateDisplay_InstalledAndEnabledZones();
		// - Control update
		ZONING_CONTROL__UpdateControl_InstalledAndEnabledZones( &bZoningDoneButtonPressed, &bZoningBackButtonPressed );
		// - Zone list update if user has accepted change
		if ( bZoningDoneButtonPressed )		// Done - Update changes and get out
		{
			uint8_t		ucZoneStateList = 0;

			ZONING_CONTROL__DefineFromUpdate_ZoneStateList( &ucZoneStateList ); ZONING_CONTROL__SetUserEnabledZoneStateList( ucZoneStateList );

			pCooling->UserAccess.bZoningDetails = false;
		}
		else
		if ( bZoningBackButtonPressed )		// Done - No changes
		{
			pCooling->UserAccess.bZoningDetails = false;
		}
		return;
	}
	//
	// UPDATE USER DISPLAY
	// -------------------
	// Normal Operation - Common Display
	UpdateDisplay_Mode_NormalOperation( pCooling );

    if ( ( pCooling->bSlaveModeSwitch ) || ( UICOMMON__bMasterModeSwitch() ) ) { return; }
	//
	// Mode - FanOnly Related
	HTGCLG_USERIF_NC7__UpdateDisplay_Mode_FanOnly( pCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Fan Only Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateControl_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pCooling)
{
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	// Check if zoning screen is being accessed
	if ( pCooling->UserAccess.bZoningDetails ) 	{ return; }
	// If mode switch get out
    if ( ( pCooling->bSlaveModeSwitch ) || ( UICOMMON__bMasterModeSwitch() ) ) { return; }
	//
	// Not locked out and no mode switch so process user actions
	HTGCLG_USERIF_NC7__UpdateControl_Mode_FanOnly( pCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User display update - N-C7 : On Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateDisplay_Mode_On(HTGCLG_COMMON__ControlDetails* pCooling)
{
	HTGCLG_USERIF_NC7__TemperatureControlSpecificDetails	TemperatureControl;
	bool													bZoningDoneButtonPressed = false, bZoningBackButtonPressed = false;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) 					{ return; }
	// Service info being displayed
	if ( HTGCLG_USERIF_NC7__bViewingServiceInfoPrompt( pCooling ) )	{ return; }
	//
	// Check if zoning screen is being accessed
	if ( pCooling->UserAccess.bZoningDetails )
	{
		// Do not show bottom status in zoning screen
		UICOMMON__ShowBottomStatus( false );
		//
		// Accessing zoning details to be programmed
		// - Display update
		ZONING_CONTROL__UpdateDisplay_InstalledAndEnabledZones();
		// - Control update
		ZONING_CONTROL__UpdateControl_InstalledAndEnabledZones( &bZoningDoneButtonPressed, &bZoningBackButtonPressed );
		// - Zone list update if user has accepted change
		if ( bZoningDoneButtonPressed )		// Done - Update changes and get out
		{
			uint8_t		ucZoneStateList = 0;

			ZONING_CONTROL__DefineFromUpdate_ZoneStateList( &ucZoneStateList ); ZONING_CONTROL__SetUserEnabledZoneStateList( ucZoneStateList );

			pCooling->UserAccess.Control.bZoningDoneButtonPressed = true;		// Need to know when user changes zone states for schedule override

			pCooling->UserAccess.bZoningDetails = false;
		}
		else
		if ( bZoningBackButtonPressed )		// Done - No changes
		{
			pCooling->UserAccess.bZoningDetails = false;
		}
		return;
	}
	//
	// UPDATE USER DISPLAY
	// -------------------
	// Normal Operation - Common Display
	UpdateDisplay_Mode_NormalOperation( pCooling );

    if ( ( pCooling->bSlaveModeSwitch ) || ( UICOMMON__bMasterModeSwitch() ) ) { return; }
	//
	// Mode - SwitchOn Related
    // - Define what we need to show in this operating mode
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
	    TemperatureControl.bShowSetTemperature = true;
	}
	else
	if ( NWSMSPCL__bAZoneIsCurrentlyBeingAccessed() )
	{
	    TemperatureControl.bShowSetTemperature = true;
	}
	else
	{
	    TemperatureControl.bShowSetTemperature = false;
	}
    TemperatureControl.ucSetTemperature		= pCooling->Requested.ucSetTemperature;
    TemperatureControl.pLevelButtons		= (UITFTDSP_BUTTON__LevelButtonDetails*)&mkaLevelButton[COOLING_USERIF_NC7__LB__SET_TEMP_BTN];
    /*
    TemperatureControl.bShowRoomTemperature	= true;
    */ // Show room temperature same as set temperature, NC7090_OTSCHECK
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		TemperatureControl.bShowRoomTemperature	= true;
	}
	else
	if ( NWSMSPCL__bAZoneIsCurrentlyBeingAccessed() )
	{
		TemperatureControl.bShowRoomTemperature	= true;
	}
	else
	{
		TemperatureControl.bShowRoomTemperature	= false;
	}
    TemperatureControl.pMSPControl			= pCooling->pMSPControlZone;
	// - Initialise string used for text displays
	GENSTRDT__InitString( COOLING_USERIF_NC7__SET_TEMPERATURE_TEXT_LENGTH, &mstrSetTemperatureText[0], &mSetTemperatureString );
	// - Commit what is required
	HTGCLG_USERIF_NC7__UpdateTemperatureControlDetailsWhenInClimateControlState( &TemperatureControl, &mSetTemperatureString );
	// - Update snow man image
	switch ( pCooling->UserAccess.SnowmanDisplayState )
	{
		case HTGCLG_COMMON__SDS__FLASH:
			UITFTDSP__FlashImage( UITFTDSP__FR__1HZ, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[COOLING_USERIF_NC7__IN__SNOWMAN_ICON] );
			break;

		case HTGCLG_COMMON__SDS__ON:
			UITFTDSP__ShowImage(  (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[COOLING_USERIF_NC7__IN__SNOWMAN_ICON] );
			break;

		default:
			break;
	}
	// - Update display common to both heating and cooling modes
	HTGCLG_USERIF_NC7__UpdateDisplay_Mode_On( pCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : On Mode.
 * @param:	pCooling - Pointer to the cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								COOLING_USERIF_NC7__UpdateControl_Mode_On(HTGCLG_COMMON__ControlDetails* pCooling)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t   		ucTagValue;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	// Check if zoning screen is being accessed
	if ( pCooling->UserAccess.bZoningDetails ) 	{ return; }
	// If mode switch get out
    if ( ( pCooling->bSlaveModeSwitch ) || ( UICOMMON__bMasterModeSwitch() ) ) { return; }
	//
	// Not locked out and no mode switch so process user actions
	// - Get the tag value
	ucTagValue = TFTDSPIF__ucTouchTagValue();
	// - Process button presses
	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__MENU_BTN )			// Allow menu button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__HELP_BTN )			// Allow help button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( !NWSUTILS__bUserControlIsLockedOut() )													// Allow remaining only if not locked out
	{
		switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
		{
			case HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN:
				// Fan menu button - Toggle circ fan on/off
				pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__TOGGLE_CIRC_FAN;
				// No rapid update
				HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__SET_TEMP_UP_BTN:
				if ( pCooling->UserAccess.Control.bAllowSetPointAdjustment )
				{
					// Set temperature button - Increase set temperature
					pCooling->UserAccess.Control.iSetPointAdjust = 1; pCooling->UserAccess.Control.bDetectedSetPointAdjustment = true;
					// Rapid update
					HTGCLG_USERIF_NC7__RapidUpdateControl_SetTemperature( true, pCooling->UserAccess.Control.ucSetPoint );
				}
				break;

			case HTGCLG_USERIF_NC7__TV__SET_TEMP_DN_BTN:
				if ( pCooling->UserAccess.Control.bAllowSetPointAdjustment )
				{
					// Set temperature button - Decrease set temperature
					pCooling->UserAccess.Control.iSetPointAdjust = -1; pCooling->UserAccess.Control.bDetectedSetPointAdjustment = true;
					// Rapid update
					HTGCLG_USERIF_NC7__RapidUpdateControl_SetTemperature( false, pCooling->UserAccess.Control.ucSetPoint );
				}
				break;

			case HTGCLG_USERIF_NC7__TV__MANUAL_BTN:
				// Check if in auto program control
				if ( pCooling->pMSPUserRequest->ScheduleControl.bEnabled )
				{
					// Manual button - Switch to manual control
					pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_AUTO_MANUAL;
				}
				// No rapid update
				HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__ONOFF_BTN:
				// On/Off button - Switch off
				pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_OFF;
				// No rapid update
				HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__ALERT_BTN:
	   		case HTGCLG_USERIF_NC7__TV__ZONES_BTN:
			case HTGCLG_USERIF_NC7__TV__MODE_BTN:
			case HTGCLG_USERIF_NC7__TV__SCHEDULE_BTN:
			case HTGCLG_USERIF_NC7__TV__CANCEL_BTN:
			case HTGCLG_USERIF_NC7__TV__ADVANCE_PERIOD_BTN:
	    		// Remember this tag as it causes screen to switch
	   			sucActiveTag = ucTagValue;
	   			// No rapid update
	    	    HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

	        default:
	        	break;
		}
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )		// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case HTGCLG_USERIF_NC7__TV__ALERT_BTN:
					NWS_SVCINF_CONTROL__ServiceInfoPrompt_FlagToShowIt( HTGCLG_USERIF_NC7__pServiceInfoDetails() );
					break;

				case HTGCLG_USERIF_NC7__TV__ZONES_BTN:
	   				// Check for MSP operation as functionality differs
					if ( NWSUTILS__bMSPControlEnabled() )
				    {
						// Cycle to next available zone
						NWSMSPCL__AccessNextAvailableZone();
				    }
					else
					{
						// Initialise before access
						ZONING_CONTROL__InitialiseUIBeforeFirstAccess( ZONING_CONTROL__ucUserEnabledZoneStateList() );
						// Access zoning details
						pCooling->UserAccess.bZoningDetails = true;
					}
		   			break;

				case HTGCLG_USERIF_NC7__TV__MODE_BTN:
					pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_MODE;
					break;

				case HTGCLG_USERIF_NC7__TV__SCHEDULE_BTN:
					pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_AUTO_MANUAL;
					break;

				case HTGCLG_USERIF_NC7__TV__CANCEL_BTN:
				case HTGCLG_USERIF_NC7__TV__ADVANCE_PERIOD_BTN:
					pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__ADV_PERIOD_OR_CANCEL_PROGRAM_OVERRIDE;
					break;

				case HTGCLG_USERIF_NC7__TV__MENU_BTN:
					pCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__ACCESS_CONFIG_MENU;
		   		    break;

				case HTGCLG_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	Common display update under normal operation - N-C7.
 * @param:	pHeating - Pointer to the heating control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							UpdateDisplay_Mode_NormalOperation(HTGCLG_COMMON__ControlDetails* pCooling)
{
	// Display info common to htg/clg
	HTGCLG_USERIF_NC7__UpdateDisplay_Mode_NormalOperation( NWSUTILS__CCMODE__COOLING, pCooling );
	//
	// Display info specific to htg unless mode switch
	// - Cooling icon
	if ( ( !pCooling->bSlaveModeSwitch ) && ( !UICOMMON__bMasterModeSwitch() ) )
	{
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[COOLING_USERIF_NC7__TN__COOL] );
	}

	return;
}
//*********************************************************************************************************************************************************************************
