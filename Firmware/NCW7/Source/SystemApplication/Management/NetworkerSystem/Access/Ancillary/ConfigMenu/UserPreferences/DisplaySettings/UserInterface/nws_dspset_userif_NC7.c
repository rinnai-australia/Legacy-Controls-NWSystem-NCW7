//*********************************************************************************************************************************************************************************
// nws_dspset_userif_NC7.c
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
#include		<nws_dspset_common.h>
#include		<uicommon.h>
#include		<uitftdsp.h>
#include		<uitftdsp_button.h>
#include		<uitftdsp_image.h>
#include		<uitftdsp_color.h>

#include        <tftdspif.h>
#include		<nwsutils.h>
#include        <genstrdt.h>
#include		<gensavdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const char mkstrTopStatusText[] = "MENU : USER PREFERENCES - General Settings";

typedef struct	DisplaySettingDetails {					// Display Setting Details
														//
	const GENPRMDT__ParameterValue*	pValue;				// - Pointer to the value (parameter)
														//
	char*							pstrShortDesc;		// - Pointer to string (Short description)
	char*							pstrDescLine1;		// - Pointer to string (Line 1 description)
	char*							pstrDescLine2;		// - Pointer to string (Line 2 description)
	char*							pstrDescLine3;		// - Pointer to string (Line 3 description)

} DisplaySettingDetails;

static DisplaySettingDetails	mkaDisplaySetting[] = {

	{ &UITFTDSP__kTFTControlBacklightOnFullLevel,			 "Active backlight intensity:", "The intensity level the backlight is operated at when the",   "networker is being accessed.",		"Range : 1 - 10" 			},
	{ &UITFTDSP__kTFTControlBacklightOnFullTimeoutInterval,  "Active backlight delay:",	    "This is the time the backlight is kept at the active level",  "when there is no user activity.",	"Range : 10 - 60 Seconds"	},
	{ &UITFTDSP__kTFTControlBacklightDimmedTimeoutInterval,  "Dimmed backlight delay:",     "This is the time the backlight is kept at the dimmed level",  "prior to being switched off.",		"Range : 10 - 60 Seconds"	},
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Setting actual value string
// - Max length allowed
#define			NWS_DSPSET_USERIF_NC7__SETTING_VALUE_TEXT_LENGTH	5
// - Actual container for string
static char 			mstrDisplaySettingValueText[NWS_DSPSET_USERIF_NC7__SETTING_VALUE_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mDisplaySettingValueString;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Settings detailed description strings
// - Max length allowed
#define			NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH		64
// - Actual container for string
static char 			mstrDisplaySettingInfoShortDescText[NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH+1];
static char 			mstrDisplaySettingInfoDescLine1Text[NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH+1];
static char 			mstrDisplaySettingInfoDescLine2Text[NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH+1];
static char 			mstrDisplaySettingInfoDescLine3Text[NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mDisplaySettingInfoShortDescString;
static GENSTRDT__String	mDisplaySettingInfoDescLine1String;
static GENSTRDT__String	mDisplaySettingInfoDescLine2String;
static GENSTRDT__String	mDisplaySettingInfoDescLine3String;
//
// Reference line for start of top most line - All other lines will be relative to this
#define			NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE		62
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Display Settings
typedef enum	NWS_DSPSET_USERIF_NC7__SETTING_NUMBER {

	NWS_DSPSET_USERIF_NC7__SN__BACKLIGHT_INTENSITY_FULLON = 0,
	NWS_DSPSET_USERIF_NC7__SN__BACKLIGHT_DELAY_FULLON     = 1,
	NWS_DSPSET_USERIF_NC7__SN__BACKLIGHT_DELAY_DIMMED     = 2

} NWS_DSPSET_USERIF_NC7__SETTING_NUMBER;

#define			NWS_DSPSET_USERIF_NC7__MAX_SETTINGS		3
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Module Tag Values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_DSPSET_USERIF_NC7__TAG_VALUE {

	NWS_DSPSET_USERIF_NC7__TV__HELP_BTN 	 = 1,
	NWS_DSPSET_USERIF_NC7__TV__BACK_BTN 	 = 2,

	NWS_DSPSET_USERIF_NC7__TV__LEVEL_UP_BTN  = 11,
	NWS_DSPSET_USERIF_NC7__TV__LEVEL_DN_BTN  = 12,
	NWS_DSPSET_USERIF_NC7__TV__PREV_BTN      = 13,
	NWS_DSPSET_USERIF_NC7__TV__NEXT_BTN      = 14,

} NWS_DSPSET_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_DSPSET_USERIF_NC7__IMAGE_NUMBER {

	NWS_DSPSET_USERIF_NC7__IN__HELP_BTN      = 0,
	NWS_DSPSET_USERIF_NC7__IN__BACK_BTN      = 1,
	NWS_DSPSET_USERIF_NC7__IN__VALUE_UP_BTN	 = 2,
	NWS_DSPSET_USERIF_NC7__IN__VALUE_DN_BTN	 = 3

} NWS_DSPSET_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	// 				  UITFTDSP_IMAGE__IMAGE_NUMBER,   						  			   		   X,  											  	Y, 				          	Persistence time, TAG
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,  											  	3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_DSPSET_USERIF_NC7__TV__HELP_BTN 		}, // 0:
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,  											  	3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_DSPSET_USERIF_NC7__TV__BACK_BTN 		}, // 1:
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,  											  	3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_DSPSET_USERIF_NC7__TV__HELP_BTN 		}, // 0:
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,  											  	3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_DSPSET_USERIF_NC7__TV__BACK_BTN 		}, // 1:
#endif
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE, 425, NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_DSPSET_USERIF_NC7__TV__LEVEL_UP_BTN 	}, // 2:
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE, 375, NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_DSPSET_USERIF_NC7__TV__LEVEL_DN_BTN 	}  // 3:
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Number - Use as index to : mkaButton[]
typedef enum	NWS_DSPSET_USERIF_NC7__BUTTON_NUMBER {

	NWS_DSPSET_USERIF_NC7__BN__PREV_BTN = 0,
	NWS_DSPSET_USERIF_NC7__BN__NEXT_BTN = 1

} NWS_DSPSET_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton[] = {
		// BUTTON																																				   RECTANGLE						      TEXT
		//                Normal State,   HL,         	  		           Persist time, 					    Style,	                               Tag,			   Color,   X,   Y,   W,   Y,  LW  	      X,   Y,  XO,  YO, Color,          	      FontSize,		   	    Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_DSPSET_USERIF_NC7__TV__PREV_BTN,   	{  {       0, 230, 233, 110,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Previous" 	} } },	// 0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_DSPSET_USERIF_NC7__TV__NEXT_BTN, 	{  {       0, 355, 233, 110,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Next" 		} } },	// 1
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle with text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle Number - Use as index to : mkaRectangle[]
typedef enum	NWS_DSPSET_USERIF_NC7__RECTANGLE_NUMBER {

	NWS_DSPSET_USERIF_NC7__RN__SETTING_VALUE = 0

} NWS_DSPSET_USERIF_NC7__RECTANGLE_NUMBER;
//
// Array of rectangles
static const UI_TFTSHAPE__RectangleWithTextDetails	mkaRectangle[] = {

	// Rectangle														         							 Text
	// 			         Color,    X,   											   Y,   W,   H,   LW        X,   Y,  XO,  YO,                  Color,          	         FontSize,             Justification,   Text to display
	{  { UITFTDSP_COLOR__WHITE,  267,  NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE+2, 100,  40,   8 },   { -1,  -1,  10,  -1,  UITFTDSP_COLOR__BLACK, UITFTDSP_FONTS__FS__ROUND_4,  UITFTDSP_TEXT__TJ__RIGHT,   (char*)&mstrDisplaySettingValueText[0] } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Number - Use as index to : mkaText[]
typedef enum	NWS_DSPSET_USERIF_NC7__TEXT_NUMBER {

	NWS_DSPSET_USERIF_NC7__TN_SETTING_SHORTDESC   = 0,
	NWS_DSPSET_USERIF_NC7__TN_SETTING_DESCLINE01  = 1,
	NWS_DSPSET_USERIF_NC7__TN_SETTING_DESCLINE02  = 2,
	NWS_DSPSET_USERIF_NC7__TN_SETTING_DESCLINE03  = 3

} NWS_DSPSET_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails		mkaText[] = {

	//   X,    												   Y,  X0,  Y0, 			     Color,          	       FontSize,  		     Justification, String Label
	{   15,  NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE+ 10,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrDisplaySettingInfoShortDescText[0] },  // 0 : Parameter description text
	{   30,  NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE+ 82,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrDisplaySettingInfoDescLine1Text[0] },  // 1 : Parameter value text
	{   30,  NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE+102,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrDisplaySettingInfoDescLine2Text[0] },  // 2 : Parameter value text
	{   30,  NWS_DSPSET_USERIF_NC7__SETTING_DESCRIPTION_LINE+126,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrDisplaySettingInfoDescLine3Text[0] },  // 3 : Parameter value text
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static NWS_DSPSET_USERIF_NC7__SETTING_NUMBER	mCurrentDisplaySettingNumber;
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
static void						RapidUpdateControl_ValueUpdate(bool vbIncreasingValue);
static void						RapidUpdateControl_None(void);

static void						IncrementSetting(void);
static void						DecrementSetting(void);

static void						NextSetting(void);
static void						PrevSetting(void);

static DisplaySettingDetails*	pSelectedSetting(void);
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
 * @param:	pDisplaySettingAccess - Pointer to display setting access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_DSPSET_USERIF_NC7__InitBeforeStateEntry(NWS_DSPSET_COMMON__ControlDetails* pDisplaySettingAccess)
{
	mCurrentDisplaySettingNumber = NWS_DSPSET_USERIF_NC7__SN__BACKLIGHT_INTENSITY_FULLON;
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	General setting access display update - NC7 specific.
 * @param:	pDisplaySettingAccess - Pointer to display setting access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_DSPSET_USERIF_NC7__UpdateDisplay(NWS_DSPSET_COMMON__ControlDetails* pDisplaySettingAccess)
{
	UICOMMON__TopStatusDetails	TopStatus;
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_DSPSET_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_DSPSET_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText;
    // - Perform update
    UICOMMON__TopStatus_Update( &TopStatus );
    //
    // Display Setting Details
	// - Init strings used for text displays
	GENSTRDT__InitString( NWS_DSPSET_USERIF_NC7__SETTING_VALUE_TEXT_LENGTH, &mstrDisplaySettingValueText[0], 		 &mDisplaySettingValueString		 );
	GENSTRDT__InitString( NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH , &mstrDisplaySettingInfoShortDescText[0], &mDisplaySettingInfoShortDescString );
	GENSTRDT__InitString( NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH , &mstrDisplaySettingInfoDescLine1Text[0], &mDisplaySettingInfoDescLine1String );
	GENSTRDT__InitString( NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH , &mstrDisplaySettingInfoDescLine2Text[0], &mDisplaySettingInfoDescLine2String );
	GENSTRDT__InitString( NWS_DSPSET_USERIF_NC7__SETTING_INFO_TEXT_LENGTH , &mstrDisplaySettingInfoDescLine3Text[0], &mDisplaySettingInfoDescLine3String );
	//
	// Define text data
	// - Setting value
	GENSTRDT__Add5DigitNumberToString( (uint32_t)((uint32_t)*pSelectedSetting()->pValue->pucActual * (uint32_t)pSelectedSetting()->pValue->ucScaling), false, &mDisplaySettingValueString );
	// - Short description
	GENSTRDT__AssignTextToString( pSelectedSetting()->pstrShortDesc, &mDisplaySettingInfoShortDescString );
	// - Line 1 description
	GENSTRDT__AssignTextToString( pSelectedSetting()->pstrDescLine1, &mDisplaySettingInfoDescLine1String );
	// - Line 2 description
	GENSTRDT__AssignTextToString( pSelectedSetting()->pstrDescLine2, &mDisplaySettingInfoDescLine2String );
	// - Line 3 description
	GENSTRDT__AssignTextToString( pSelectedSetting()->pstrDescLine3, &mDisplaySettingInfoDescLine3String );
	//
	// Display text info
    // - Short description
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_DSPSET_USERIF_NC7__TN_SETTING_SHORTDESC] );
    // - Setting value
    UITFTDSP__ShowRectangleWithText( (UI_TFTSHAPE__RectangleWithTextDetails*)&mkaRectangle[NWS_DSPSET_USERIF_NC7__RN__SETTING_VALUE] );
    // - Description line 1
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_DSPSET_USERIF_NC7__TN_SETTING_DESCLINE01] );
    // - Description line 2
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_DSPSET_USERIF_NC7__TN_SETTING_DESCLINE02] );
    // - Description line 3
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_DSPSET_USERIF_NC7__TN_SETTING_DESCLINE03] );
	//
    // Control buttons
    // - Value update buttons, Added, emilio 09-04-15
    UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_DSPSET_USERIF_NC7__IN__VALUE_DN_BTN] );
    UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_DSPSET_USERIF_NC7__IN__VALUE_UP_BTN] );

	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_DSPSET_USERIF_NC7__BN__PREV_BTN] );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_DSPSET_USERIF_NC7__BN__NEXT_BTN] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	General setting access control update - NC7 specific.
 * @param:	pDisplaySettingAccess - Pointer to display setting access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_DSPSET_USERIF_NC7__UpdateControl(NWS_DSPSET_COMMON__ControlDetails* pDisplaySettingAccess)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_DSPSET_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
   		case NWS_DSPSET_USERIF_NC7__TV__LEVEL_UP_BTN:
   			IncrementSetting(); RapidUpdateControl_ValueUpdate( true );
   			break;

   		case NWS_DSPSET_USERIF_NC7__TV__LEVEL_DN_BTN:
   			DecrementSetting(); RapidUpdateControl_ValueUpdate( false );
   			break;

   		case NWS_DSPSET_USERIF_NC7__TV__NEXT_BTN:
   			NextSetting(); RapidUpdateControl_None();
    		break;

   		case NWS_DSPSET_USERIF_NC7__TV__PREV_BTN:
   			PrevSetting(); RapidUpdateControl_None();
    		break;

   		case NWS_DSPSET_USERIF_NC7__TV__BACK_BTN:
   		case NWS_DSPSET_USERIF_NC7__TV__HELP_BTN:
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
			switch ( (NWS_DSPSET_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_DSPSET_USERIF_NC7__TV__BACK_BTN:
					pDisplaySettingAccess->UserAccess.Control.ButtonPressed = NWS_DSPSET_COMMON__CB__BACK;
					break;

		   		case NWS_DSPSET_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	Rapid update control - Value update.
 * @param:	vbIncreasingValue	  - Whether value is increasing or not.
 * 			pDisplaySettingAccess - Pointer to general setting access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_ValueUpdate(bool vbIncreasingValue)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;
	uint8_t								ucMin, ucMax, ucVal;

	ucMin =  pSelectedSetting()->pValue->ucMinimum;
	ucMax =  pSelectedSetting()->pValue->ucMaximum;
	ucVal = *pSelectedSetting()->pValue->pucActual;

	TouchTagValue.bWrapAroundEnabled = ( (ucMax - ucMin) > 16);
	TouchTagValue.bIncreasing        = vbIncreasingValue;
	TouchTagValue.ucMinimum			 = ucMin;
	TouchTagValue.ucMaximum			 = ucMax;
	TouchTagValue.ucActual			 = ucVal;

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

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
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Increment the current setting.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						IncrementSetting(void)
{
	uint8_t		ucVal = *pSelectedSetting()->pValue->pucActual;

	if ( ucVal == pSelectedSetting()->pValue->ucMaximum )
	{
		ucVal = pSelectedSetting()->pValue->ucMinimum;
	}
	else
	{
		ucVal++;
	}

	*pSelectedSetting()->pValue->pucActual = ucVal;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Decrement the current setting.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DecrementSetting(void)
{
	uint8_t		ucVal = *pSelectedSetting()->pValue->pucActual;

	if ( ucVal == pSelectedSetting()->pValue->ucMinimum )
	{
		ucVal = pSelectedSetting()->pValue->ucMaximum;
	}
	else
	{
		ucVal--;
	}

	*pSelectedSetting()->pValue->pucActual = ucVal;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Go to the next setting.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						NextSetting(void)
{
	if ( ++mCurrentDisplaySettingNumber >= NWS_DSPSET_USERIF_NC7__MAX_SETTINGS )
	{
		mCurrentDisplaySettingNumber = NWS_DSPSET_USERIF_NC7__SN__BACKLIGHT_INTENSITY_FULLON;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Go to the previous setting.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						PrevSetting(void)
{
	if ( mCurrentDisplaySettingNumber == 0 )
	{
		mCurrentDisplaySettingNumber = NWS_DSPSET_USERIF_NC7__MAX_SETTINGS-1;
	}
	else
	{
		mCurrentDisplaySettingNumber--;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns a pointer to the currently selected setting.
 * @param:	None.
 * @retval:	DisplaySettingDetails*.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static DisplaySettingDetails*	pSelectedSetting(void)
{
	if ( mCurrentDisplaySettingNumber > NWS_DSPSET_USERIF_NC7__SN__BACKLIGHT_DELAY_DIMMED )
	{
		mCurrentDisplaySettingNumber = NWS_DSPSET_USERIF_NC7__SN__BACKLIGHT_INTENSITY_FULLON;
	}

	return &mkaDisplaySetting[mCurrentDisplaySettingNumber];
}
//*********************************************************************************************************************************************************************************
