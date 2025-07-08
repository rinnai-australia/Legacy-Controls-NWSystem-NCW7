//*********************************************************************************************************************************************************************************
// nws_usrprf_userif_NC7.c
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
#include		<nws_usrprf_common.h>
#include        <nwsutils.h>
#include		<uitftdsp.h>
#include		<uicommon.h>
#include        <tftdspif.h>
#include		<twbif502.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const char 	mkstrTopStatusText_UserPreferences[] = "MENU : USER PREFERENCES";
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_USRPRF_USERIF_NC7__TAG_VALUE {

	NWS_USRPRF_USERIF_NC7__TV__HELP_BTN	  		  = 1,
	NWS_USRPRF_USERIF_NC7__TV__BACK_BTN	  		  = 2,

	NWS_USRPRF_USERIF_NC7__TV__KEYPAD_LOCKOUT_BTN = 11,
	NWS_USRPRF_USERIF_NC7__TV__GNRL_SETTINGS_BTN  = 12,
	NWS_USRPRF_USERIF_NC7__TV__SYST_SETTINGS_BTN  = 13,
	NWS_USRPRF_USERIF_NC7__TV__ZONE_NAMES_BTN     = 14

} NWS_USRPRF_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_USRPRF_USERIF_NC7__IMAGE_NUMBER {

	NWS_USRPRF_USERIF_NC7__IN__HELP_BTN = 0,
	NWS_USRPRF_USERIF_NC7__IN__BACK_BTN = 1

} NWS_USRPRF_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USRPRF_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USRPRF_USERIF_NC7__TV__BACK_BTN  }, //   1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USRPRF_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_USRPRF_USERIF_NC7__TV__BACK_BTN  }, //   1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_USRPRF_USERIF_NC7__BUTTON_NUMBER {

	NWS_USRPRF_USERIF_NC7__BN__LOCK_SYSTEM_BTN 		  	  = 0,
	NWS_USRPRF_USERIF_NC7__BN__UNLOCK_SYSTEM_BTN 		  = 1,
	NWS_USRPRF_USERIF_NC7__BN__SYSTEM_LOCKOUT_CONTROL_BTN = 2,
	NWS_USRPRF_USERIF_NC7__BN__GNRL_SETTINGS    		  = 3,
	NWS_USRPRF_USERIF_NC7__BN__SYST_SETTINGS    		  = 4,
	NWS_USRPRF_USERIF_NC7__BN__ZONE_NAMES 		  		  = 5

} NWS_USRPRF_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton[] = {
		// BUTTON																										  				      			                 RECTANGLE						       TEXT
		//                Normal State,   HL,                              Persist time, 					    Style,	                          			     Tag,       Color,   X,   Y,   W,   H,  LW  	   X,   Y,  XO,  YO, Color,                    FontSize,	       Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_USRPRF_USERIF_NC7__TV__KEYPAD_LOCKOUT_BTN, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Lockout System"	    	} } },	 //  0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_USRPRF_USERIF_NC7__TV__KEYPAD_LOCKOUT_BTN, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Unlock System"		    } } },	 //  1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_USRPRF_USERIF_NC7__TV__KEYPAD_LOCKOUT_BTN, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "System Lockout Control"	} } },	 //  2
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  NWS_USRPRF_USERIF_NC7__TV__GNRL_SETTINGS_BTN, {  {      0,  12, 110, 458,  44,  0  },  {  -1 , -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "General Settings"	  	} } },	 //  3
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  NWS_USRPRF_USERIF_NC7__TV__SYST_SETTINGS_BTN, {  {      0,  12, 164, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "System Settings" 		} } },	 //  4
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_USRPRF_USERIF_NC7__TV__ZONE_NAMES_BTN, {  {      0,  12, 218, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Set Zone Names"   	   	} } },	 //  5
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
static void						RapidUpdateControl_None(void);
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void							NWS_USRPRF_USERIF_NC7__InitBeforeStateEntry(NWS_USRPRF_COMMON__ControlDetails* pUserPreference);

void							NWS_USRPRF_USERIF_NC7__UpdateDisplay(NWS_USRPRF_COMMON__ControlDetails* pUserPreference);
void							NWS_USRPRF_USERIF_NC7__UpdateControl(NWS_USRPRF_COMMON__ControlDetails* pUserPreference);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// APPLICATION LEVEL FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises data ready for processing once this state is entered.
 * @param:	pUserPreference	- Pointer to user preferences info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_USRPRF_USERIF_NC7__InitBeforeStateEntry(NWS_USRPRF_COMMON__ControlDetails* pUserPreference)
{
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User set display update - NC7 specific.
 * @param:	pUserPreference	- Pointer to user preferences info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_USRPRF_USERIF_NC7__UpdateDisplay(NWS_USRPRF_COMMON__ControlDetails* pUserPreference)
{
	UICOMMON__TopStatusDetails	TopStatus;
	//
    // Top status line update
    // - Init structure
    UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USRPRF_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_USRPRF_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_UserPreferences;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
    //
    // Show the menu buttos
	if ( NWSUTILS__bUserControlIsLockedOut() )
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_USRPRF_USERIF_NC7__BN__UNLOCK_SYSTEM_BTN] );
	}
	else
	if (!TWBIF502__bIsMasterType02Device)
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_USRPRF_USERIF_NC7__BN__LOCK_SYSTEM_BTN] );
	}
	else
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_USRPRF_USERIF_NC7__BN__SYSTEM_LOCKOUT_CONTROL_BTN] );
	}
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_USRPRF_USERIF_NC7__BN__GNRL_SETTINGS] );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_USRPRF_USERIF_NC7__BN__SYST_SETTINGS] );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_USRPRF_USERIF_NC7__BN__ZONE_NAMES] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User set control update - NC7 specific.
 * @param:	pUserPreference	- Pointer to user preferences info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_USRPRF_USERIF_NC7__UpdateControl(NWS_USRPRF_COMMON__ControlDetails* pUserPreference)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_USRPRF_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_USRPRF_USERIF_NC7__TV__KEYPAD_LOCKOUT_BTN:
		case NWS_USRPRF_USERIF_NC7__TV__BACK_BTN:
		case NWS_USRPRF_USERIF_NC7__TV__HELP_BTN:
			sucActiveTag = ucTagValue; RapidUpdateControl_None();
			break;

		case NWS_USRPRF_USERIF_NC7__TV__GNRL_SETTINGS_BTN:
		case NWS_USRPRF_USERIF_NC7__TV__SYST_SETTINGS_BTN:
		case NWS_USRPRF_USERIF_NC7__TV__ZONE_NAMES_BTN:
			if ( !NWSUTILS__bUserControlIsLockedOut() )
			{
				// No lockout so process all user input actions
				sucActiveTag = ucTagValue; RapidUpdateControl_None();
			}
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
			switch ( (NWS_USRPRF_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_USRPRF_USERIF_NC7__TV__KEYPAD_LOCKOUT_BTN:
					pUserPreference->UserAccess.Control.ButtonPressed = NWS_USRPRF_COMMON__CB__KEYPAD_LOCKOUT;
					break;

				case NWS_USRPRF_USERIF_NC7__TV__GNRL_SETTINGS_BTN:
					pUserPreference->UserAccess.Control.ButtonPressed = NWS_USRPRF_COMMON__CB__GENERAL_SETTINGS;
					break;

				case NWS_USRPRF_USERIF_NC7__TV__SYST_SETTINGS_BTN:
					pUserPreference->UserAccess.Control.ButtonPressed = NWS_USRPRF_COMMON__CB__SYSTEM_SETTINGS;
					break;

				case NWS_USRPRF_USERIF_NC7__TV__ZONE_NAMES_BTN:
					pUserPreference->UserAccess.Control.ButtonPressed = NWS_USRPRF_COMMON__CB__SET_ZONE_NAMES;
					break;

				case NWS_USRPRF_USERIF_NC7__TV__BACK_BTN:
					pUserPreference->UserAccess.Control.ButtonPressed = NWS_USRPRF_COMMON__CB__BACK;
					break;

				case NWS_USRPRF_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	Rapid update control - None.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_None(void)
{
	UITFTDSP__TouchControl_RapidUpdateDataInit();
	return;
}
//*********************************************************************************************************************************************************************************
