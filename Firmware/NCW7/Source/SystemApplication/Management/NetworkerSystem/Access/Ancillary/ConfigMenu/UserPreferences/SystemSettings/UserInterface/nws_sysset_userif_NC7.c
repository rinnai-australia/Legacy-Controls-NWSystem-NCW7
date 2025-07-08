//*********************************************************************************************************************************************************************************
// nws_sysset_userif_NC7.c
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
#include		<nws_sysset_common.h>
#include        <nwsutils.h>
#include		<uitftdsp.h>
#include		<uicommon.h>
#include        <tftdspif.h>
#include		<twbif502.h>
#include		<genstrdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const char 	mkstrTopStatusText_Master[] = "MENU : SYSTEM SETTINGS";
static const char 	mkstrTopStatusText_Remote[] = "ACCESS BARRED : System Settings Access Active";
//
// Day grouping option
#define				NWS_SYSSET_USERIF_NC7__DAY_GROUPING_TEXT_LENGTH			20
// Heating
static char			mstrHeatingDayGroupOptionText[NWS_SYSSET_USERIF_NC7__DAY_GROUPING_TEXT_LENGTH+1];
// Cooling
static char			mstrCoolingDayGroupOptionText[NWS_SYSSET_USERIF_NC7__DAY_GROUPING_TEXT_LENGTH+1];
//
// Presleep Option
// - Max length allowed
#define				NWS_SYSSET_USERIF_NC7__PRE_SLEEP_OPTION_TEXT_LENGTH		5
// - Actual container for string
static char			mstrPreSleepOptionText[NWS_SYSSET_USERIF_NC7__PRE_SLEEP_OPTION_TEXT_LENGTH+1];
//
// Set temperature scale string details - ** Array variables as text to display will vary so define here for pointer inclusion in text table **
// - Max length allowed
#define				NWS_SYSSET_USERIF_NC7__TEMP_SCALE_TEXT_LENGTH			16
// - Actual container for string
static char			mstrTemperatureScaleText[NWS_SYSSET_USERIF_NC7__TEMP_SCALE_TEXT_LENGTH+1];
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_SYSSET_USERIF_NC7__TAG_VALUE {

	NWS_SYSSET_USERIF_NC7__TV__HELP_BTN	  		  = 1,
	NWS_SYSSET_USERIF_NC7__TV__BACK_BTN	  		  = 2,

	NWS_SYSSET_USERIF_NC7__TV__HTG_DAY_GROUPING   = 11,
	NWS_SYSSET_USERIF_NC7__TV__CLG_DAY_GROUPING   = 12,
	NWS_SYSSET_USERIF_NC7__TV__PRESLEEP_OPTION    = 13,
	NWS_SYSSET_USERIF_NC7__TV__TEMP_SCALE_BTN     = 14

} NWS_SYSSET_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_SYSSET_USERIF_NC7__IMAGE_NUMBER {

	NWS_SYSSET_USERIF_NC7__IN__HELP_BTN = 0,
	NWS_SYSSET_USERIF_NC7__IN__BACK_BTN = 1

} NWS_SYSSET_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SYSSET_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SYSSET_USERIF_NC7__TV__BACK_BTN  }, //   1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SYSSET_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SYSSET_USERIF_NC7__TV__BACK_BTN  }, //   1
#endif
};
//
//--------------------------------------------------------------------------------------------------------------------------------
// Button With Two Texts Details
//--------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaTwoTextsButton[]
typedef enum	NWS_SYSSET_USERIF_NC7__TWO_TEXTS_BUTTON_NUMBER {

	NWS_SYSSET_USERIF_NC7__TTBN__HTG_DAY_GROUP   =  0,
	NWS_SYSSET_USERIF_NC7__TTBN__CLG_DAY_GROUP   =  1,
	NWS_SYSSET_USERIF_NC7__TTBN__PRESLEEP_OPTION =  2,
	NWS_SYSSET_USERIF_NC7__TTBN__TEMP_SCALE_BTN	 =  3,

} NWS_SYSSET_USERIF_NC7__TWO_TEXTS_BUTTON_NUMBER;
//
// Array of buttons with two texts
static const UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition	mkaTwoTextsButton[] = {
		// BUTTON																																	  	  	 			    RECTANGLE						   	  TEXT1																								           			 TEXT2
	    //               Normal State,     HL,         	  		  		    Persist time, 						   Style,	                             		 	Tag, 	   Color,   X,   Y,   W,   H,  LW  	   	  X,   Y,  XO,  YO, Color,          	      FontSize,		   Justification,  	 Text								     X,   Y,  XO,  YO, Color,         		     FontSize,		 	   Justification,  Text
	{  UITFTDSP_BUTTON__STATE_PASSIVE,	 true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	UITFTDSP_BUTTON__STYLE_ROUND, 	NWS_SYSSET_USERIF_NC7__TV__HTG_DAY_GROUPING, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__LEFT, "Schedule Day Grouping (Heat):"  }, { 452,  70,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, 	UITFTDSP_TEXT__TJ__RIGHT,  (char*)&mstrHeatingDayGroupOptionText[0] } } },
	{  UITFTDSP_BUTTON__STATE_PASSIVE,	 true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	UITFTDSP_BUTTON__STYLE_ROUND, 	NWS_SYSSET_USERIF_NC7__TV__CLG_DAY_GROUPING, {  {      0,  12, 110, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__LEFT, "Schedule Day Grouping (Cool):"  }, { 452, 124,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, 	UITFTDSP_TEXT__TJ__RIGHT,  (char*)&mstrCoolingDayGroupOptionText[0] } } },
	{  UITFTDSP_BUTTON__STATE_PASSIVE,	 true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	UITFTDSP_BUTTON__STYLE_ROUND, 	 NWS_SYSSET_USERIF_NC7__TV__PRESLEEP_OPTION, {  {      0,  12, 164, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__LEFT, "Schedule Pre-Sleep Option:" 	  }, { 452, 178,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, 	UITFTDSP_TEXT__TJ__RIGHT,  (char*)&mstrPreSleepOptionText[0]   		} } },
	{  UITFTDSP_BUTTON__STATE_PASSIVE,	 true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	UITFTDSP_BUTTON__STYLE_ROUND, 	  NWS_SYSSET_USERIF_NC7__TV__TEMP_SCALE_BTN, {  {      0,  12, 218, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__LEFT, "Temperature Display Scale:" 	  }, { 452, 232,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, 	UITFTDSP_TEXT__TJ__RIGHT,  (char*)&mstrTemperatureScaleText[0]   	} } },
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
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises data ready for processing once this state is entered.
 * @param:	pSystSet - Pointer to system setting info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SYSSET_USERIF_NC7__InitBeforeStateEntry(NWS_SYSSET_COMMON__ControlDetails* pSystSet)
{
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	System set display update - NC7 specific.
 * @param:	pSystSet - Pointer to system setting info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SYSSET_USERIF_NC7__UpdateDisplay(NWS_SYSSET_COMMON__ControlDetails* pSystSet)
{
	UICOMMON__TopStatusDetails	TopStatus;
	NWSUTILS__DAY_GROUP			DayGrouping;
	uint8_t						ucIndex;

	if ( TWBIF502__bIsMasterType02Device )
	{
	    // Top status line update
	    // - Init structure
	    UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
	    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_SYSSET_USERIF_NC7__IN__HELP_BTN];
#endif
	    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_SYSSET_USERIF_NC7__IN__BACK_BTN];
		TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_Master;
	    // - Perform update
		UICOMMON__TopStatus_Update( &TopStatus );
	    //
		// Schedule day grouping options
		// - String definition
		GENSTRDT__String		DayGroupOption;
		//
		for ( ucIndex = 0; ucIndex < 2; ucIndex++ )
		{
			// - Initialise string used for text displays
			if ( ucIndex == 0 )
			{
				GENSTRDT__InitString( NWS_SYSSET_USERIF_NC7__DAY_GROUPING_TEXT_LENGTH , &mstrHeatingDayGroupOptionText[0], &DayGroupOption );

				DayGrouping = pSystSet->UserAccess.HtgDayGrouping;
			}
			else
			{
				GENSTRDT__InitString( NWS_SYSSET_USERIF_NC7__DAY_GROUPING_TEXT_LENGTH , &mstrCoolingDayGroupOptionText[0], &DayGroupOption );

				DayGrouping = pSystSet->UserAccess.ClgDayGrouping;
			}
			// - Define the string
			switch ( DayGrouping )
			{
				case NWSUTILS__DG__INDIVIDUAL_DAYS:
					GENSTRDT__AssignTextToString( "Set Individually", &DayGroupOption );
					break;

				case NWSUTILS__DG__WEEK_AND_WEEKEND_DAYS:
					GENSTRDT__AssignTextToString( "Weekdays/Weekends", &DayGroupOption );
					break;

				default:
					GENSTRDT__AssignTextToString( "Same for all days", &DayGroupOption );
					break;
			}
		}
		// - Show buttons
		UITFTDSP__ShowButtonWithTwoTexts( (UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[NWS_SYSSET_USERIF_NC7__TTBN__HTG_DAY_GROUP] );
		UITFTDSP__ShowButtonWithTwoTexts( (UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[NWS_SYSSET_USERIF_NC7__TTBN__CLG_DAY_GROUP] );
		//
		// Schedule presleep option
		// - String definition
		GENSTRDT__String		PreSleepOption;
		// - Initialise string used for text displays
		GENSTRDT__InitString( NWS_SYSSET_USERIF_NC7__PRE_SLEEP_OPTION_TEXT_LENGTH , &mstrPreSleepOptionText[0], &PreSleepOption );
		// - Define the string
		if ( pSystSet->UserAccess.bPreSleepEnabled )
		{
			GENSTRDT__AssignTextToString( "On", &PreSleepOption );
		}
		else
		{
			GENSTRDT__AssignTextToString( "Off", &PreSleepOption );
		}
		// - Show button
		UITFTDSP__ShowButtonWithTwoTexts( (UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[NWS_SYSSET_USERIF_NC7__TTBN__PRESLEEP_OPTION] );
		//
		// Set temperature scale details
		// - String definition
		GENSTRDT__String		TempScale;
		// - Initialise string used for text displays
		GENSTRDT__InitString( NWS_SYSSET_USERIF_NC7__TEMP_SCALE_TEXT_LENGTH , &mstrTemperatureScaleText[0], &TempScale );
		// - Check temperature unit
		if ( pSystSet->UserAccess.bTempScaleCelsius ) { GENSTRDT__AssignTextToString( "Celsius", &TempScale ); } else { GENSTRDT__AssignTextToString( "Fahrenheit", &TempScale ); }
		// - Show set temperature screen button
		UITFTDSP__ShowButtonWithTwoTexts( (UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[NWS_SYSSET_USERIF_NC7__TTBN__TEMP_SCALE_BTN] );
	}
	else
	{
		// Top status line update
		// - Init structure
		UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
		TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_Remote;
		TopStatus.TextFlashRate      = UITFTDSP__FR__1HZ;
		TopStatus.bFlashText         = true;
		// - Perform update
		UICOMMON__TopStatus_Update( &TopStatus );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	System set control update - NC7 specific.
 * @param:	pSystSet - Pointer to system setting info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SYSSET_USERIF_NC7__UpdateControl(NWS_SYSSET_COMMON__ControlDetails* pSystSet)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_SYSSET_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_SYSSET_USERIF_NC7__TV__HTG_DAY_GROUPING:
		case NWS_SYSSET_USERIF_NC7__TV__CLG_DAY_GROUPING:
		case NWS_SYSSET_USERIF_NC7__TV__PRESLEEP_OPTION:
		case NWS_SYSSET_USERIF_NC7__TV__TEMP_SCALE_BTN:
		case NWS_SYSSET_USERIF_NC7__TV__BACK_BTN:
		case NWS_SYSSET_USERIF_NC7__TV__HELP_BTN:
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
			switch ( (NWS_SYSSET_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_SYSSET_USERIF_NC7__TV__HTG_DAY_GROUPING:
					pSystSet->UserAccess.Control.ButtonPressed = NWS_SYSSET_COMMON__CB__NEXT_HTG_DAY_GROUP_OPTION;
					break;

				case NWS_SYSSET_USERIF_NC7__TV__CLG_DAY_GROUPING:
					pSystSet->UserAccess.Control.ButtonPressed = NWS_SYSSET_COMMON__CB__NEXT_CLG_DAY_GROUP_OPTION;
					break;

				case NWS_SYSSET_USERIF_NC7__TV__PRESLEEP_OPTION:
					pSystSet->UserAccess.Control.ButtonPressed = NWS_SYSSET_COMMON__CB__TOGGLE_PRESLEEP_OPTION;
					break;

				case NWS_SYSSET_USERIF_NC7__TV__TEMP_SCALE_BTN:
					pSystSet->UserAccess.Control.ButtonPressed = NWS_SYSSET_COMMON__CB__TOGGLE_TEMPERATURE_UNITS;
					break;

				case NWS_SYSSET_USERIF_NC7__TV__BACK_BTN:
					pSystSet->UserAccess.Control.ButtonPressed = NWS_SYSSET_COMMON__CB__BACK;
					break;

				case NWS_SYSSET_USERIF_NC7__TV__HELP_BTN:
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
