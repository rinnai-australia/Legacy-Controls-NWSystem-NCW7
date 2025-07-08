//*********************************************************************************************************************************************************************************
// nws_prompt_userif_NC7.c
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
#include		<nws_csetup_userif_NC7.h>
#include		<nws_prompt_common.h>

#include		<uitftdsp.h>
#include		<uitftdsp_button.h>
#include		<uitftdsp_image.h>
#include		<uicommon.h>
#include        <tftdspif.h>
#include        <genstrdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const char 	mkstrInfoPromptText_TopStatus[] 	   = "INFORMATION";
static const char 	mkstrInfoPromptText_RecalTouchScreen[] = "Recalibrating Touch Screen - Please wait for restart";
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_PROMPT_USERIF_NC7__TAG_VALUE {

	NWS_PROMPT_USERIF_NC7__TV__HELP_BTN		= 1,
	NWS_PROMPT_USERIF_NC7__TV__BACK_BTN	  	= 2,

	NWS_PROMPT_USERIF_NC7__TV__PROMPT_BTN   = 11,
	NWS_PROMPT_USERIF_NC7__TV__YES_BTN  	= 12,
	NWS_PROMPT_USERIF_NC7__TV__NO_BTN	    = 13

} NWS_PROMPT_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_PROMPT_USERIF_NC7__IMAGE_NUMBER {

	NWS_PROMPT_USERIF_NC7__IN__HELP_BTN =  0,
	NWS_PROMPT_USERIF_NC7__IN__BACK_BTN =  1

} NWS_PROMPT_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PROMPT_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PROMPT_USERIF_NC7__TV__BACK_BTN  }, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PROMPT_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PROMPT_USERIF_NC7__TV__BACK_BTN  }, // 1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Containers
// - Max length allowed
#define			NWS_PROMPT_USERIF_NC7__INFO_PROMPT_LENGTH		100
// - Actual containers for strings
static char 	mstrInfoPromptTextLine1[NWS_PROMPT_USERIF_NC7__INFO_PROMPT_LENGTH+1];
static char 	mstrInfoPromptTextLine2[NWS_PROMPT_USERIF_NC7__INFO_PROMPT_LENGTH+1];
static char 	mstrInfoPromptTextLine3[NWS_PROMPT_USERIF_NC7__INFO_PROMPT_LENGTH+1];
//
// Number - Use as index to mkaText[]
typedef enum	NWS_PROMPT_USERIF_NC7__TEXT_NUMBER {

	NWS_PROMPT_USERIF_NC7__TN__PROMPT_LINE1 = 0,
	NWS_PROMPT_USERIF_NC7__TN__PROMPT_LINE2 = 1,
	NWS_PROMPT_USERIF_NC7__TN__PROMPT_LINE3 = 2

} NWS_PROMPT_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails		mkaText[] = {
	//   X,   Y,  X0,  Y0, 			    	Color,         		      FontSize,  		   	  Justification, String Label
	{  240, 100,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__CENTER, &mstrInfoPromptTextLine1[0]   },   // 0
	{  240, 125,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__CENTER, &mstrInfoPromptTextLine2[0]   },   // 1
	{  240, 150,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__CENTER, &mstrInfoPromptTextLine3[0]   },   // 2
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_PROMPT_USERIF_NC7__BUTTON_NUMBER {

	NWS_PROMPT_USERIF_NC7__BN__PROMPT_BTN	=  0,
	NWS_PROMPT_USERIF_NC7__BN__YES_BTN      =  1,
	NWS_PROMPT_USERIF_NC7__BN__NO_BTN       =  2

} NWS_PROMPT_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton[] = {
		// BUTTON																										  				      			           RECTANGLE						     TEXT
		//                Normal State,   HL,                              Persist time, 					    Style,	                          		   Tag,	      Color,   X,   Y,   W,   H,  LW  	     X,   Y,  XO,  YO, Color,          		     FontSize,		       Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,   NWS_PROMPT_USERIF_NC7__TV__PROMPT_BTN, {  {      0, 150, 200, 180,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "Ok"	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,      NWS_PROMPT_USERIF_NC7__TV__YES_BTN, {  {      0, 120, 200, 100,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "Yes"	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    	 NWS_PROMPT_USERIF_NC7__TV__NO_BTN, {  {      0, 260, 200, 100,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, "No"	 } } },
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
static void						DefineMessage(NWS_PROMPT_COMMON__MESSAGE vMessage);
static void						SetInfoPromptTextLine1(const char* pstrPrompt);
static void						SetInfoPromptTextLine2(const char* pstrPrompt);
static void						SetInfoPromptTextLine3(const char* pstrPrompt);

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
 * @brief:	Info prompt display update - NC7 specific.
 * @param:	pInfoPrompt - Pointer to info prompt access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PROMPT_USERIF_NC7__UpdateDisplay(NWS_PROMPT_COMMON__ControlDetails* pInfoPrompt)
{
	if ( pInfoPrompt->Message == NWS_PROMPT_COMMON__MSG__RECAL_TOUCH_SCREEN )
	{
		// Recalibrate touch screen
		NWS_CSETUP_USERIF_NC7__UpdateDisplay_ShowInfoPromptTextWithMovingDots( (char*)&mkstrInfoPromptText_RecalTouchScreen );
	}
	else
	{
		UICOMMON__TopStatusDetails	TopStatus;
		//
		// Top status line update
		// - Init structure
		UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
		TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PROMPT_USERIF_NC7__IN__HELP_BTN];
#endif
		TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PROMPT_USERIF_NC7__IN__BACK_BTN];
		TopStatus.pstrTextToDisplay = (char*)mkstrInfoPromptText_TopStatus;
		// - Perform update
		UICOMMON__TopStatus_Update( &TopStatus );
		//
		// Prompt
		// - Define message
		DefineMessage( pInfoPrompt->Message );
		// - Show it
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PROMPT_USERIF_NC7__TN__PROMPT_LINE1] );
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PROMPT_USERIF_NC7__TN__PROMPT_LINE2] );
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PROMPT_USERIF_NC7__TN__PROMPT_LINE3] );
		//
		// Button
		if ( ( pInfoPrompt->Message == NWS_PROMPT_COMMON__MSG__RESET_NETWORKER_ID ) || ( pInfoPrompt->Message == NWS_PROMPT_COMMON__MSG__RESTORE_DEFAULTS ) )
		{
			// - Yes button
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PROMPT_USERIF_NC7__BN__YES_BTN] );
			// - No button
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PROMPT_USERIF_NC7__BN__NO_BTN] );
		}
		else
		{
			// - Ok button
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PROMPT_USERIF_NC7__BN__PROMPT_BTN] );
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Info prompt control update - NC7 specific.
 * @param:	pInfoPrompt - Pointer to info prompt access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PROMPT_USERIF_NC7__UpdateControl(NWS_PROMPT_COMMON__ControlDetails* pInfoPrompt)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_PROMPT_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
   		case NWS_PROMPT_USERIF_NC7__TV__BACK_BTN:
   		case NWS_PROMPT_USERIF_NC7__TV__PROMPT_BTN:
   		case NWS_PROMPT_USERIF_NC7__TV__YES_BTN:
   		case NWS_PROMPT_USERIF_NC7__TV__NO_BTN:
   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
    		break;

   		case NWS_PROMPT_USERIF_NC7__TV__HELP_BTN:
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
			switch ( (NWS_PROMPT_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_PROMPT_USERIF_NC7__TV__BACK_BTN:
					pInfoPrompt->UserAccess.Control.ButtonPressed = NWS_PROMPT_COMMON__CB__BACK;
					break;

				case NWS_PROMPT_USERIF_NC7__TV__PROMPT_BTN:
					pInfoPrompt->UserAccess.Control.ButtonPressed = NWS_PROMPT_COMMON__CB__PROMPT_OK;
					break;

				case NWS_PROMPT_USERIF_NC7__TV__YES_BTN:
					pInfoPrompt->UserAccess.Control.ButtonPressed = NWS_PROMPT_COMMON__CB__PROMPT_YES;
					break;

				case NWS_PROMPT_USERIF_NC7__TV__NO_BTN:
					pInfoPrompt->UserAccess.Control.ButtonPressed = NWS_PROMPT_COMMON__CB__PROMPT_NO;
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
 * @brief:  Message to define.
 * @param:	vMessage - Message type.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DefineMessage(NWS_PROMPT_COMMON__MESSAGE vMessage)
{
	switch (vMessage)
	{
		case NWS_PROMPT_COMMON__MSG__CLOCK_SET_MASTER:
			SetInfoPromptTextLine1( "Day/Time Setting can only be from the" );
			SetInfoPromptTextLine2( "master networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__INSTALL_AXS_MASTER:
			SetInfoPromptTextLine1( "Installer Setup parameters can only be accessed" );
			SetInfoPromptTextLine2( "using the master networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__SERVICE_AXS_MASTER:
			SetInfoPromptTextLine1( "Service Access parameters can only be accessed" );
			SetInfoPromptTextLine2( "using the master networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__INSTALL_AXS_SWITCH_OFF:
			SetInfoPromptTextLine1( "Installer Setup parameters can only be accessed" );
			SetInfoPromptTextLine2( "when the system is switched off!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__SERVICE_AXS_SWITCH_OFF:
			SetInfoPromptTextLine1( "Service Access parameters can only be accessed" );
			SetInfoPromptTextLine2( "when the system is switched off!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__SYST_SETTINGS_MASTER:
			SetInfoPromptTextLine1( "System settings can only be set using the" );
			SetInfoPromptTextLine2( "master networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__ZONE_NAMES_MASTER:
			SetInfoPromptTextLine1( "Zone names can only be set using the" );
			SetInfoPromptTextLine2( "master networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__ZONE_NAMES_NO_ZONES:
			SetInfoPromptTextLine1( "There are no zones installed in the" );
			SetInfoPromptTextLine2( "system to name!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_EVPCOOL:
			SetInfoPromptTextLine1( "Schedule is unavailable to evaporative" );
			SetInfoPromptTextLine2( "cooling operation mode!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_NOCNTRL:
			SetInfoPromptTextLine1( "Schedule is unavailable - no appliance" );
			SetInfoPromptTextLine2( "attached to this networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_FANONLY:
			SetInfoPromptTextLine1( "Schedule can only be accessed when the" );
			SetInfoPromptTextLine2( "fan is switched off!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__RECAL_TOUCH_SCREEN:
			// No message here - Info prompt text to be shown in top status area
			break;

		case NWS_PROMPT_COMMON__MSG__RESET_NETWORKER_ID:
			SetInfoPromptTextLine1( "Are you sure you want to reset" );
			SetInfoPromptTextLine2( "networker ID?" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__RESTORE_DEFAULTS:
			SetInfoPromptTextLine1( "Are you sure you want to restore" );
			SetInfoPromptTextLine2( "networker defaults?" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__SYST_LOKOUT_REGNWK:
			SetInfoPromptTextLine1( "System can only be locked out using a" );
			SetInfoPromptTextLine2( "registered networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		case NWS_PROMPT_COMMON__MSG__SYST_UNLOCK_REGNWK:
			SetInfoPromptTextLine1( "System can only be unlocked using a" );
			SetInfoPromptTextLine2( "registered networker!" );
			SetInfoPromptTextLine3( "" );
			break;

		default:
			// Cannot happen - Just keep compiler happy
			SetInfoPromptTextLine1( "" );
			SetInfoPromptTextLine2( "" );
			SetInfoPromptTextLine3( "" );
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:  Initialises the information prompt ready for display.
 * @param:	pstrPrompt - Pointer to the text to display.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						SetInfoPromptTextLine1(const char* pstrPrompt)
{
	GENSTRDT__String		Text;

	GENSTRDT__InitString( NWS_PROMPT_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrInfoPromptTextLine1[0], &Text ); GENSTRDT__AssignTextToString( (char*)pstrPrompt, &Text );

	return;
}
static void						SetInfoPromptTextLine2(const char* pstrPrompt)
{
	GENSTRDT__String		Text;

	GENSTRDT__InitString( NWS_PROMPT_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrInfoPromptTextLine2[0], &Text ); GENSTRDT__AssignTextToString( (char*)pstrPrompt, &Text );

	return;
}
static void						SetInfoPromptTextLine3(const char* pstrPrompt)
{
	GENSTRDT__String		Text;

	GENSTRDT__InitString( NWS_PROMPT_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrInfoPromptTextLine3[0], &Text ); GENSTRDT__AssignTextToString( (char*)pstrPrompt, &Text );

	return;
}
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
