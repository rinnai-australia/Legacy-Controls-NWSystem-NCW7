//*********************************************************************************************************************************************************************************
// nws_cfgmnu_userif_NC7.c
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

#include		<uitftdsp.h>
#include		<uitftdsp_button.h>
#include		<uitftdsp_image.h>
#include		<uicommon.h>
#include        <tftdspif.h>
#include		<nws_access_common.h>
#include        <genstrdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const char 	mkstrTopStatusText[]  = "MENU";
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_CFGMNU_USERIF_NC7__TAG_VALUE {

	NWS_CFGMNU_USERIF_NC7__TV__HELP_BTN	  		= 1,
	NWS_CFGMNU_USERIF_NC7__TV__BACK_BTN	  		= 2,

	NWS_CFGMNU_USERIF_NC7__TV__CLEAN_SCREEN_BTN = 11,
	NWS_CFGMNU_USERIF_NC7__TV__DIAGNOSTICS_BTN  = 12,
	NWS_CFGMNU_USERIF_NC7__TV__INSTALLER_BTN    = 13,
	NWS_CFGMNU_USERIF_NC7__TV__SCHEDULE_BTN     = 14,
	NWS_CFGMNU_USERIF_NC7__TV__SET_TIME_BTN     = 15,
	NWS_CFGMNU_USERIF_NC7__TV__USER_PREF_BTN    = 16

} NWS_CFGMNU_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_CFGMNU_USERIF_NC7__IMAGE_NUMBER {

	NWS_CFGMNU_USERIF_NC7__IN__HELP_BTN =  0,
	NWS_CFGMNU_USERIF_NC7__IN__BACK_BTN =  1

} NWS_CFGMNU_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						   		 Image - Active,   X,   Y, 						   	    Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CFGMNU_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CFGMNU_USERIF_NC7__TV__BACK_BTN  }, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CFGMNU_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CFGMNU_USERIF_NC7__TV__BACK_BTN  }, // 1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_CFGMNU_USERIF_NC7__BUTTON_NUMBER {

	NWS_CFGMNU_USERIF_NC7__BN__USER_PREF_BTN      =  0,
	NWS_CFGMNU_USERIF_NC7__BN__SCHEDULE_BTN       =  1,
	NWS_CFGMNU_USERIF_NC7__BN__SET_TIME_BTN       =  2,
	NWS_CFGMNU_USERIF_NC7__BN__CLEAN_SCREEN_BTN   =  3,
	NWS_CFGMNU_USERIF_NC7__BN__INSTALLER_BTN      =  4,
	NWS_CFGMNU_USERIF_NC7__BN__DIAGNOSTICS_BTN    =  5

} NWS_CFGMNU_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton[] = {
		// BUTTON																										  				      			       		  RECTANGLE						         TEXT
		//                Normal State,   HL,                              Persist time, 					    Style,	                          			   Tag,	      Color,   X,   Y,   W,   H,  LW  	     X,   Y,  XO,  YO, Color,          		    FontSize,		       Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    NWS_CFGMNU_USERIF_NC7__TV__USER_PREF_BTN, {  {      0,  12,  56, 224,  62,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "User Preferences"	} } },	 //  0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CFGMNU_USERIF_NC7__TV__SCHEDULE_BTN, {  {      0, 246,  56, 224,  62,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "Schedule" 			} } },	 //  1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_CFGMNU_USERIF_NC7__TV__CLEAN_SCREEN_BTN, {  {      0,  12, 128, 224,  62,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "Clean Screen" 	 	} } },	 //  2
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CFGMNU_USERIF_NC7__TV__SET_TIME_BTN, {  {      0, 246, 128, 224,  62,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "Set Day/Time" 	 	} } },	 //  3
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    NWS_CFGMNU_USERIF_NC7__TV__INSTALLER_BTN, {  {      0,  12, 200, 224,  62,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "Installer Setup"  	} } },	 //  4
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  NWS_CFGMNU_USERIF_NC7__TV__DIAGNOSTICS_BTN, {  {      0, 246, 200, 224,  62,  0  },  {  -1 , -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "Service Access" 		} } },	 //  5
} ;
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
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises data ready for processing once this state is entered.
 * @param:	pConfigMenu - Pointer to config menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CFGMNU_USERIF_NC7__InitBeforeStateEntry(NWS_CFGMNU_COMMON__ControlDetails* pConfigMenu)
{
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Config menu display update - NC7 specific.
 * @param:	pConfigMenu	- Pointer to config menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CFGMNU_USERIF_NC7__UpdateDisplay(NWS_CFGMNU_COMMON__ControlDetails* pConfigMenu)
{
	UICOMMON__TopStatusDetails	TopStatus;

    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CFGMNU_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CFGMNU_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
    //
    // Show the menu buttos
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CFGMNU_USERIF_NC7__BN__SCHEDULE_BTN] 	  );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CFGMNU_USERIF_NC7__BN__CLEAN_SCREEN_BTN] );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CFGMNU_USERIF_NC7__BN__DIAGNOSTICS_BTN]  );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CFGMNU_USERIF_NC7__BN__INSTALLER_BTN] 	  );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CFGMNU_USERIF_NC7__BN__SET_TIME_BTN] 	  );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CFGMNU_USERIF_NC7__BN__USER_PREF_BTN] 	  );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Config menu control update - NC7 specific.
 * @param:	pConfigMenu	- Pointer to config menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CFGMNU_USERIF_NC7__UpdateControl(NWS_CFGMNU_COMMON__ControlDetails* pConfigMenu)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_CFGMNU_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_CFGMNU_USERIF_NC7__TV__USER_PREF_BTN:
   		case NWS_CFGMNU_USERIF_NC7__TV__BACK_BTN:
   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
    		break;

		case NWS_CFGMNU_USERIF_NC7__TV__CLEAN_SCREEN_BTN:
		case NWS_CFGMNU_USERIF_NC7__TV__DIAGNOSTICS_BTN:
		case NWS_CFGMNU_USERIF_NC7__TV__INSTALLER_BTN:
		case NWS_CFGMNU_USERIF_NC7__TV__SCHEDULE_BTN:
		case NWS_CFGMNU_USERIF_NC7__TV__SET_TIME_BTN:
			if ( !NWSUTILS__bUserControlIsLockedOut() )
			{
				// No lockout so process all user input actions
				sucActiveTag = ucTagValue; RapidUpdateControl_None();
			}
    		break;

   		case NWS_CFGMNU_USERIF_NC7__TV__HELP_BTN:
   			// TODO - Help button
   			RapidUpdateControl_None();
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
			switch ( (NWS_CFGMNU_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_CFGMNU_USERIF_NC7__TV__CLEAN_SCREEN_BTN:
					// Clean screen button
					pConfigMenu->UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__CLEAN_SCREEN;
					break;

				case NWS_CFGMNU_USERIF_NC7__TV__DIAGNOSTICS_BTN:
					// Diagnostics button
					pConfigMenu->UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__ACCESS_DIAGNOSTICS;
					break;

				case NWS_CFGMNU_USERIF_NC7__TV__INSTALLER_BTN:
					// Installer Setup button
					pConfigMenu->UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__INSTALLER_SETUP;
					break;

				case NWS_CFGMNU_USERIF_NC7__TV__SCHEDULE_BTN:
					// Program Schedule button
					pConfigMenu->UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__PROGRAM_SCHEDULE;
					break;

				case NWS_CFGMNU_USERIF_NC7__TV__SET_TIME_BTN:
					// Set day/time button
					pConfigMenu->UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__CLOCK_SET;
					break;

				case NWS_CFGMNU_USERIF_NC7__TV__USER_PREF_BTN:
					// User Preference button
					pConfigMenu->UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__USER_PREFERENCE_SET;
					break;

				case NWS_CFGMNU_USERIF_NC7__TV__BACK_BTN:
					pConfigMenu->UserAccess.Control.ButtonPressed = NWS_CFGMNU_COMMON__CB__BACK;
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
