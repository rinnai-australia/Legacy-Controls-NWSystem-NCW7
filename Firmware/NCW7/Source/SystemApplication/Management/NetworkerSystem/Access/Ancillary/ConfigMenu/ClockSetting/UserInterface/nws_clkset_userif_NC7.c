//*********************************************************************************************************************************************************************************
// nws_clkset_userif_NC7.c
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
#include		<nws_clkset_common.h>
#include		<uicommon.h>
#include		<twbif502.h>
#include        <tftdspif.h>
#include		<uitftdsp_color.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			NWS_CLKSET_USERIF_NC7__DAYSET_LINE					85
#define			NWS_CLKSET_USERIF_NC7__TIMSET_LINE					135
//
// Top status header text
static const char		mkstrTopStatusText_Master[] = "MENU : SET DAY/TIME";
static const char		mkstrTopStatusText_Remote[] = "ACCESS BARRED : User Clock Setting Access Active";
//
// String definition details - ** Array variables as text to display will vary so define here for pointer inclusion in text table **
//
// Day of week text
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			NWS_CLKSET_USERIF_NC7__DAY_OF_WEEK_TEXT_LENGTH		10
// - Actual container for string
static char 			mstrDayOfWeekText[NWS_CLKSET_USERIF_NC7__DAY_OF_WEEK_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mDayOfWeekString;
//
// Time display text
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			NWS_CLKSET_USERIF_NC7__TIME_TEXT_LENGTH				10
// - Actual container for string
static char 			mstrTimeText[NWS_CLKSET_USERIF_NC7__TIME_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mTimeString;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_CLKSET_USERIF_NC7__TAG_VALUE {

	NWS_CLKSET_USERIF_NC7__TV__HELP_BTN	  		= 1,
	NWS_CLKSET_USERIF_NC7__TV__BACK_BTN	  		= 2,

	NWS_CLKSET_USERIF_NC7__TV__DAY_UP_BTN 		= 11,
	NWS_CLKSET_USERIF_NC7__TV__DAY_DN_BTN  		= 12,
	NWS_CLKSET_USERIF_NC7__TV__TIME_UP_BTN 		= 13,
	NWS_CLKSET_USERIF_NC7__TV__TIME_DN_BTN  	= 14,
	NWS_CLKSET_USERIF_NC7__TV__DONE_BTN  		= 15,
	NWS_CLKSET_USERIF_NC7__TV__12HR_BTN  		= 16,
	NWS_CLKSET_USERIF_NC7__TV__24HR_BTN  		= 17

} NWS_CLKSET_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_CLKSET_USERIF_NC7__IMAGE_NUMBER {

	NWS_CLKSET_USERIF_NC7__IN__BACK_BTN	   = 0,
	NWS_CLKSET_USERIF_NC7__IN__HELP_BTN	   = 1,
	NWS_CLKSET_USERIF_NC7__IN__DAY_UP_BTN  = 2,
	NWS_CLKSET_USERIF_NC7__IN__DAY_DN_BTN  = 3,
	NWS_CLKSET_USERIF_NC7__IN__TIME_UP_BTN = 4,
	NWS_CLKSET_USERIF_NC7__IN__TIME_DN_BTN = 5

} NWS_CLKSET_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						   		 Image - Active,   X,   								 Y, 						     Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   								 3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__BACK_BTN  	}, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   								 3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__HELP_BTN  	}, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   								 3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__BACK_BTN  	}, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   								 3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__HELP_BTN  	}, // 1
#endif
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE, 425,   NWS_CLKSET_USERIF_NC7__DAYSET_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__DAY_UP_BTN 	}, // 2
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE, 375,   NWS_CLKSET_USERIF_NC7__DAYSET_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__DAY_DN_BTN 	}, // 3
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE, 425,   NWS_CLKSET_USERIF_NC7__TIMSET_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__TIME_UP_BTN 	}, // 4
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE, 375,   NWS_CLKSET_USERIF_NC7__TIMSET_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CLKSET_USERIF_NC7__TV__TIME_DN_BTN 	}  // 5
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_CLKSET_USERIF_NC7__BUTTON_NUMBER {

	NWS_CLKSET_USERIF_NC7__BN__12HOUR_SELECTED_BTN 	   = 0,
	NWS_CLKSET_USERIF_NC7__BN__12HOUR_NOT_SELECTED_BTN = 1,
	NWS_CLKSET_USERIF_NC7__BN__24HOUR_SELECTED_BTN 	   = 2,
	NWS_CLKSET_USERIF_NC7__BN__24HOUR_NOT_SELECTED_BTN = 3,
	NWS_CLKSET_USERIF_NC7__BN__DONE_BTN				   = 4

} NWS_CLKSET_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton[] = {
		// BUTTON																										  				      		              RECTANGLE						    									   TEXT
		//                Normal State,    HL,                              Persist time, 					     Style,	                   			       Tag,	     Color,   X,                                      Y,   W,   H,  LW  	   X,   Y,  XO,  YO, Color,          		   FontSize,		     Justification, Text
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    NWS_CLKSET_USERIF_NC7__TV__12HR_BTN, { {      0, 215,  NWS_CLKSET_USERIF_NC7__TIMSET_LINE+50,  70,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "12 Hr"  } } },	 //  0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    NWS_CLKSET_USERIF_NC7__TV__12HR_BTN, { {      0, 215,  NWS_CLKSET_USERIF_NC7__TIMSET_LINE+50,  70,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "12 Hr"  } } },	 //  1
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    NWS_CLKSET_USERIF_NC7__TV__24HR_BTN, { {      0, 295,  NWS_CLKSET_USERIF_NC7__TIMSET_LINE+50,  70,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "24 Hr"  } } },	 //  2
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    NWS_CLKSET_USERIF_NC7__TV__24HR_BTN, { {      0, 295,  NWS_CLKSET_USERIF_NC7__TIMSET_LINE+50,  70,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "24 Hr"  } } },	 //  3
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    NWS_CLKSET_USERIF_NC7__TV__DONE_BTN, { {      0, 375,  NWS_CLKSET_USERIF_NC7__TIMSET_LINE+50,  95,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Done" 	 } } },	 //  4
} ;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaText[]
typedef enum	NWS_CLKSET_USERIF_NC7__TEXT_NUMBER {

	NWS_CLKSET_USERIF_NC7__TN__DAY_OF_WEEK = 0,
	NWS_CLKSET_USERIF_NC7__TN__TIME 	   = 1

} NWS_CLKSET_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails			mkaText[] = {
	//   X,    								    Y,  X0,  Y0, 			   	  Color,          		 	FontSize,  		 	   Justification, String Label
	{  205, NWS_CLKSET_USERIF_NC7__DAYSET_LINE+10,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__RIGHT, "Set current day:"   },   // 0
	{  205, NWS_CLKSET_USERIF_NC7__TIMSET_LINE+10,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__RIGHT, "Set current time:"  },   // 1
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle with text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaRectangle[]
typedef enum	NWS_CLKSET_USERIF_NC7__RECTANGLE_NUMBER {

	NWS_CLKSET_USERIF_NC7__RN__DAY_OF_WEEK = 0,
	NWS_CLKSET_USERIF_NC7__RN__TIME 	   = 1

} NWS_CLKSET_USERIF_NC7__RECTANGLE_NUMBER;
//
// Array of rectangles with two texts
static const UI_TFTSHAPE__RectangleWithTextDetails	mkaRectangle[] = {
	// Rectangle														         				Text
	// 			         Color,    X,                                     Y,   W,   H,   LW        X,   Y,  XO,  YO,                  Color,          	        FontSize,              Justification,  Text to display
	{  { UITFTDSP_COLOR__WHITE,  215,  NWS_CLKSET_USERIF_NC7__DAYSET_LINE+2, 150,  40,   8 },   { -1,  -1,  -1,  -1,  UITFTDSP_COLOR__BLACK, UITFTDSP_FONTS__FS__ROUND_3,  UITFTDSP_TEXT__TJ__CENTER,  (char*)&mstrDayOfWeekText[0] } },
	{  { UITFTDSP_COLOR__WHITE,  215,  NWS_CLKSET_USERIF_NC7__TIMSET_LINE+2, 150,  40,   8 },   { -1,  -1,  -1,  -1,  UITFTDSP_COLOR__BLACK, UITFTDSP_FONTS__FS__ROUND_3,  UITFTDSP_TEXT__TJ__CENTER,  (char*)&mstrTimeText[0] 		} },
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
static void						RapidUpdateControl_Time(void);
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
 * @param:	pClockSet - Pointer to clock setting info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CLKSET_USERIF_NC7__InitBeforeStateEntry(NWS_CLKSET_COMMON__ControlDetails* pClockSet)
{
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Clock set display update - NC7 specific.
 * @param:	pClockSet - Pointer to clock setting info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CLKSET_USERIF_NC7__UpdateDisplay(NWS_CLKSET_COMMON__ControlDetails* pClockSet)
{
	UICOMMON__TopStatusDetails	TopStatus;

	if ( TWBIF502__bIsMasterType02Device )
	{
		// Master can only show clock setting details
	    //
	    // Top status line update
	    // - Init structure
		UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
	    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CLKSET_USERIF_NC7__IN__HELP_BTN];
#endif
	    if ( !pClockSet->bStateForced )		// Only allow back button when this state is not forced, emilio 11-01-16
	    {
		    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CLKSET_USERIF_NC7__IN__BACK_BTN];
	    }
        TopStatus.pstrTextToDisplay = (char*)&mkstrTopStatusText_Master[0];
	    // - Perform update
        UICOMMON__TopStatus_Update( &TopStatus );

		//
		// Show current hour format
		if ( TWBIFUTL__bClockDisplayFormatIn12HourFormat() )
		{
			// 12 hour format
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CLKSET_USERIF_NC7__BN__12HOUR_SELECTED_BTN] 	 );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CLKSET_USERIF_NC7__BN__24HOUR_NOT_SELECTED_BTN] );
		}
		else
		{
			// 24 hour format
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CLKSET_USERIF_NC7__BN__12HOUR_NOT_SELECTED_BTN] );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CLKSET_USERIF_NC7__BN__24HOUR_SELECTED_BTN] 	 );
		}
		//
		// Day of week
	    // - Init string
	    GENSTRDT__InitString( NWS_CLKSET_USERIF_NC7__DAY_OF_WEEK_TEXT_LENGTH, &mstrDayOfWeekText[0], &mDayOfWeekString );
		// - Setup day to display
	    GENSTRDT__AssignTextToString( UICOMMON__pstrDayOfWeek(RTMCLKIF__pClockSet()->Day), &mDayOfWeekString );
		// - Prompt
	    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_CLKSET_USERIF_NC7__TN__DAY_OF_WEEK] );
	    // - Entry
		UITFTDSP__ShowRectangleWithText( (UI_TFTSHAPE__RectangleWithTextDetails*)&mkaRectangle[NWS_CLKSET_USERIF_NC7__RN__DAY_OF_WEEK] );
		// - Up/Down buttons
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CLKSET_USERIF_NC7__IN__DAY_UP_BTN] );
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CLKSET_USERIF_NC7__IN__DAY_DN_BTN] );
	    //
	    // Time
	    // - Init string
	    GENSTRDT__InitString( NWS_CLKSET_USERIF_NC7__TIME_TEXT_LENGTH, &mstrTimeText[0], &mTimeString );
	    // - Setup time to display
	    UICOMMON__AssignTimeToString( TWBIFUTL__bClockDisplayFormatIn12HourFormat(), RTMCLKIF__pClockSet()->ShortTime.ucHour, RTMCLKIF__pClockSet()->ShortTime.ucMinute, &mTimeString );
		// - Prompt
	    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_CLKSET_USERIF_NC7__TN__TIME] );
	    // - Entry
		UITFTDSP__ShowRectangleWithText( (UI_TFTSHAPE__RectangleWithTextDetails*)&mkaRectangle[NWS_CLKSET_USERIF_NC7__RN__TIME] );
	    // - Up/Down buttons
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CLKSET_USERIF_NC7__IN__TIME_UP_BTN] );
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CLKSET_USERIF_NC7__IN__TIME_DN_BTN] );
		//
		// Show done button
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CLKSET_USERIF_NC7__BN__DONE_BTN] );
	}
	else
	{
		// Slave just indicates master in clock setting state
		//
		// Top status line update
	    // - Init structure
		UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
		TopStatus.pstrTextToDisplay = (char*)&mkstrTopStatusText_Remote[0];
		TopStatus.TextFlashRate     = UITFTDSP__FR__1HZ;
		TopStatus.bFlashText	    = true;
	    // - Perform update
		UICOMMON__TopStatus_Update( &TopStatus );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Clock set control update - NC7 specific.
 * @param:	pClockSet - Pointer to clock setting info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CLKSET_USERIF_NC7__UpdateControl(NWS_CLKSET_COMMON__ControlDetails* pClockSet)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	if ( TWBIF502__bIsMasterType02Device )
	{
		switch ( (NWS_CLKSET_USERIF_NC7__TAG_VALUE)ucTagValue )
		{
			case NWS_CLKSET_USERIF_NC7__TV__12HR_BTN:
				// 12 hour format button press
				pClockSet->UserAccess.Control.ButtonPressed = NWS_CLKSET_COMMON__CB__SET_12HR_FORMAT;
				// No rapid update
				RapidUpdateControl_None();
				break;

			case NWS_CLKSET_USERIF_NC7__TV__24HR_BTN:
				// 24 hour format button press
				pClockSet->UserAccess.Control.ButtonPressed = NWS_CLKSET_COMMON__CB__SET_24HR_FORMAT;
				// No rapid update
				RapidUpdateControl_None();
				break;

			case NWS_CLKSET_USERIF_NC7__TV__DAY_UP_BTN:
				// Day up button - change to the next day
				pClockSet->UserAccess.Control.ButtonPressed = NWS_CLKSET_COMMON__CB__SET_NEXT_DAY;
				// No rapid update
				RapidUpdateControl_None();
				break;

			case NWS_CLKSET_USERIF_NC7__TV__DAY_DN_BTN:
				// Day down button - change to the previous day
				pClockSet->UserAccess.Control.ButtonPressed = NWS_CLKSET_COMMON__CB__SET_PREVIOUS_DAY;
				// No rapid update
				RapidUpdateControl_None();
				break;

			case NWS_CLKSET_USERIF_NC7__TV__TIME_UP_BTN:
				// Time up button - increase time by 1 minute
				pClockSet->UserAccess.Control.iTimeAdjust_Minutes = 1;
				// Allow rapid update
				RapidUpdateControl_Time();
				break;

			case NWS_CLKSET_USERIF_NC7__TV__TIME_DN_BTN:
				// Time down button - decrease time by 1 minute
				pClockSet->UserAccess.Control.iTimeAdjust_Minutes = -1;
				// Allow rapid update
				RapidUpdateControl_Time();
				break;

			case NWS_CLKSET_USERIF_NC7__TV__DONE_BTN:
			case NWS_CLKSET_USERIF_NC7__TV__BACK_BTN:
			case NWS_CLKSET_USERIF_NC7__TV__HELP_BTN:
	   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
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
			switch ( (NWS_CLKSET_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_CLKSET_USERIF_NC7__TV__DONE_BTN:
					pClockSet->UserAccess.Control.ButtonPressed = NWS_CLKSET_COMMON__CB__SAVE_AND_EXIT;
					break;

				case NWS_CLKSET_USERIF_NC7__TV__BACK_BTN:
					pClockSet->UserAccess.Control.ButtonPressed = NWS_CLKSET_COMMON__CB__EXIT_NO_SAVE;
					break;

				case NWS_CLKSET_USERIF_NC7__TV__HELP_BTN:
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
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - Time update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_Time(void)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = true;
	TouchTagValue.ucMinimum 		 = 0;
	TouchTagValue.ucMaximum 		 = 59;

	FT800_TOUCHIFC__RapidUpdateDataDefine(&TouchTagValue);

	return;
}
//*********************************************************************************************************************************************************************************
