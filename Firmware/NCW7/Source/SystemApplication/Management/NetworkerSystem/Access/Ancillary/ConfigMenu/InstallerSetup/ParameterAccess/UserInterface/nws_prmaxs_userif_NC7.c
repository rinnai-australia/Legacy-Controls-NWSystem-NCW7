//*********************************************************************************************************************************************************************************
// nws_prmaxs_userif_NC7.c
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
#include		<nws_prmaxs_common.h>
#include		<uitftdsp.h>
#include		<uicommon.h>
#include		<uitftdsp_button.h>
#include		<uitftdsp_image.h>
#include		<uitftdsp_color.h>
#include        <tftdspif.h>
#include		<nwsutils.h>
#include        <paramdsc.h>
#include        <genstrdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// String definition details - ** Array variables as text to display will vary so define here for pointer inclusion in text table **
//
// Parameter description string
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			NWS_PRMAXS_USERIF_NC7__PARAMETER_DESC_TEXT_LENGTH	32
// - Actual container for string
static char 			mstrParameterDescriptionText[NWS_PRMAXS_USERIF_NC7__PARAMETER_DESC_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mParameterDescriptionString;
//
// Actual value string
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			NWS_PRMAXS_USERIF_NC7__PARAMETER_VALUE_TEXT_LENGTH	5
// - Actual container for string
static char 			mstrParameterValueText[NWS_PRMAXS_USERIF_NC7__PARAMETER_VALUE_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mParameterValueString;
//
// Parameter detailed description strings
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH	80
// - Actual container for string
static char 			mstrParameterInfoHeadingText[NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH+1];
static char 			mstrParameterInfoLine1Text[NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH+1];
static char 			mstrParameterInfoLine2Text[NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH+1];
static char 			mstrParameterInfoLine3Text[NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH+1];
static char 			mstrParameterInfoLine4Text[NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mParameterInfoHeadingString;
static GENSTRDT__String	mParameterInfoLine1String;
static GENSTRDT__String	mParameterInfoLine2String;
static GENSTRDT__String	mParameterInfoLine3String;
static GENSTRDT__String	mParameterInfoLine4String;
//
// Type/Version description
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			NWS_PRMAXS_USERIF_NC7__TYPE_VERSION_TEXT_LENGTH		32
// - Actual container for string
static char				mstrTypeVersionInfo[NWS_PRMAXS_USERIF_NC7__TYPE_VERSION_TEXT_LENGTH+1];
//
// Constant strings
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const char mkstrTopStatusText_ParamAccess_Install[] 	= "MENU : INSTALLER SETUP - PARAMETERS";
static const char mkstrTopStatusText_ParamAccess_Service[] 	= "MENU : SERVICE ACCESS - PARAMETERS";
static const char mkstrTopStatusText_ParamAccess_Remote[] 	= "ACCESS BARRED : Parameter Access Active";
static const char mkstrTopStatusText_ForceNWAddx[] 			= "MENU : SET NETWORKER ADDRESS";
//
// Device Group Abbreviation - Index is device address group
static const uint8_t	mkaucGroupDescription[] = {

	'0', 		// N/A
	'E', 		// Evap cooler
	'n', 		// Networker
	'H', 		// Heater
	'r',		// Addon
	'0', 		// N/A
	'A', 		// Zoning
	'0', 		// N.A
	'R' 		// Reverse-Cycle Air-Conditioning
};
//
// Reference line for start of top most line - All other lines will be relative to this
#define			NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE		62
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag Values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_PRMAXS_USERIF_NC7__TAG_VALUE {

	NWS_PRMAXS_USERIF_NC7__TV__HELP_BTN 	 = 1,
	NWS_PRMAXS_USERIF_NC7__TV__BACK_BTN 	 = 2,

	NWS_PRMAXS_USERIF_NC7__TV__LEVEL_UP_BTN  = 11,
	NWS_PRMAXS_USERIF_NC7__TV__LEVEL_DN_BTN  = 12,
	NWS_PRMAXS_USERIF_NC7__TV__PREV_BTN      = 13,
	NWS_PRMAXS_USERIF_NC7__TV__NEXT_BTN      = 14,
	NWS_PRMAXS_USERIF_NC7__TV__MORE_LESS_BTN = 15,
	NWS_PRMAXS_USERIF_NC7__TV__RESET_BTN     = 16

} NWS_PRMAXS_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_PRMAXS_USERIF_NC7__IMAGE_NUMBER {

	NWS_PRMAXS_USERIF_NC7__IN__HELP_BTN       =  0,
	NWS_PRMAXS_USERIF_NC7__IN__BACK_BTN       =  1,
	NWS_PRMAXS_USERIF_NC7__IN__VALUE_UP_BTN	  =  2,
	NWS_PRMAXS_USERIF_NC7__IN__VALUE_DN_BTN	  =  3

} NWS_PRMAXS_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	// 				  UITFTDSP_IMAGE__IMAGE_NUMBER,   						  			   		   X,  												Y, 					         Persistence time, TAG
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429, 												3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PRMAXS_USERIF_NC7__TV__HELP_BTN 		}, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386, 												3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PRMAXS_USERIF_NC7__TV__BACK_BTN 		}, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429, 												3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PRMAXS_USERIF_NC7__TV__HELP_BTN 		}, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429, 												3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PRMAXS_USERIF_NC7__TV__BACK_BTN 		}, // 1
#endif

	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE, 425, NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PRMAXS_USERIF_NC7__TV__LEVEL_UP_BTN }, // 2
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE, 375, NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PRMAXS_USERIF_NC7__TV__LEVEL_DN_BTN }  // 3
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_PRMAXS_USERIF_NC7__BUTTON_NUMBER {

	NWS_PRMAXS_USERIF_NC7__BN__PREV_BTN  = 0,
	NWS_PRMAXS_USERIF_NC7__BN__NEXT_BTN  = 1,
	NWS_PRMAXS_USERIF_NC7__BN__MORE_BTN  = 2,
	NWS_PRMAXS_USERIF_NC7__BN__LESS_BTN  = 3,
	NWS_PRMAXS_USERIF_NC7__BN__RESET_BTN = 4

} NWS_PRMAXS_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton[] = {
		// BUTTON																																				       RECTANGLE						      TEXT
		//                Normal State,   HL,         	  		           Persist time, 					    Style,	                               Tag,			       Color,   X,   Y,   W,   Y,  LW  	      X,   Y,  XO,  YO, Color,          	  	  FontSize,		   	 	Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_PRMAXS_USERIF_NC7__TV__PREV_BTN,   		{  {       0, 230, 233, 110,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Previous" 		 	} } },	// 0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_PRMAXS_USERIF_NC7__TV__NEXT_BTN, 	 	{  {       0, 355, 233, 110,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Next" 			 	} } },	// 1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_PRMAXS_USERIF_NC7__TV__MORE_LESS_BTN,	{  {       0,  15, 233, 200,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "More Information" 	} } },	// 2
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_PRMAXS_USERIF_NC7__TV__MORE_LESS_BTN,	{  {       0,  15, 233, 200,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Less Information" 	} } },	// 3
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_PRMAXS_USERIF_NC7__TV__RESET_BTN,	    {  {       0, 105, 233, 110,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Reset" 				} } }	// 4
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle with text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaTextEntry[]
typedef enum	NWS_PRMAXS_USERIF_NC7__RECTANGLE_NUMBER {

	NWS_PRMAXS_USERIF_NC7__RN__PARAMETER_VALUE = 0

} NWS_PRMAXS_USERIF_NC7__RECTANGLE_NUMBER;
//
// Array of text entries
static const UI_TFTSHAPE__RectangleWithTextDetails	mkaTextEntry[] = {

	// Rectangle														         							   Text
	// 			         Color,    X,                                                    Y,   W,   H,   LW        X,   Y,  XO,  YO,                  Color,          		   FontSize,             Justification,   Text to display
	{  { UITFTDSP_COLOR__WHITE,  267,  NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE+2, 100,  40,   8 },   { -1,  -1,  10,  -1,  UITFTDSP_COLOR__BLACK, UITFTDSP_FONTS__FS__ROUND_4,  UITFTDSP_TEXT__TJ__RIGHT,   (char*)&mstrParameterValueText[0] } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaText[]
typedef enum	NWS_PRMAXS_USERIF_NC7__TEXT_NUMBER {

	NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DESCRIPTION   = 0,
	NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE01 = 1,
	NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE02 = 2,
	NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE03 = 3,
	NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE04 = 4,
	NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE05 = 5,
	NWS_PRMAXS_USERIF_NC7__TN_VERSION_DETAILS 		  = 6,
	NWS_PRMAXS_USERIF_NC7__TN_PROMPT_LINE1 			  = 7,
	NWS_PRMAXS_USERIF_NC7__TN_PROMPT_LINE2 			  = 8,
	NWS_PRMAXS_USERIF_NC7__TN_TYPE_VERSION_INFO 	  = 9

} NWS_PRMAXS_USERIF_NC7__TEXT_NUMBER;
//
// Array of text texts
static const UITFTDSP_TEXT__TextDetails		mkaText[] = {

	//   X,    													 Y,  X0,  Y0, 			       Color,          	         FontSize,  		     Justification, String Label
	{   15,  NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE+ 10,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrParameterDescriptionText[0] 			},  // 0 : Parameter description text
	{   30,  NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE+ 55,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrParameterInfoHeadingText[0] 			},  // 1 : Parameter value text
	{   30,  NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE+ 82,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrParameterInfoLine1Text[0]   			},  // 2 : Parameter value text
	{   30,  NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE+102,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrParameterInfoLine2Text[0]   			},  // 3 : Parameter value text
	{   30,  NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE+122,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrParameterInfoLine3Text[0]   			},  // 4 : Parameter value text
	{   30,  NWS_PRMAXS_USERIF_NC7__PARAMETER_DESCRIPTION_LINE+142,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrParameterInfoLine4Text[0]   			},  // 5 : Parameter value text
	{   30,  												   240,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,     UITFTDSP_TEXT__TJ__LEFT, "Firmware Revision - B003"   						},  // 6 : Version info
	{  240, 												   100,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__CENTER, "This networker's parameters are being accessed"   	},  // 7 :
	{  240, 												   125,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__CENTER, "by the master networker!"   			 			},  // 8 :
	{   15, 												   237,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrTypeVersionInfo[0]	 		 			},  // 9 :
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool								mbShowDetailedInformation;
static PARAMDSC__ParameterDescription*	mpParameterDescription;
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
static void						RapidUpdateControl_ValueUpdate(bool vbIncreasingValue, NWS_PRMAXS_COMMON__ControlDetails* pParamAccess);
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
 * @param:	pParamAccess - Pointer to parameter access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PRMAXS_USERIF_NC7__InitBeforeStateEntry(NWS_PRMAXS_COMMON__ControlDetails* pParamAccess)
{
	mbShowDetailedInformation = false;
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Parameter access display update - NC7 specific.
 * @param:	vbRestoringDefaults	- Flag indicating when defaults are being restored.
 * 			vbForceSetOfNWAddx	- Flag indicating when networker address is being forced.
 * 			pParamAccess		- Pointer to parameter access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PRMAXS_USERIF_NC7__UpdateDisplay(bool vbRestoringDefaults, bool vbForceSetOfNWAddx, NWS_PRMAXS_COMMON__ControlDetails* pParamAccess)
{
	UICOMMON__TopStatusDetails	TopStatus;
	GENSTRDT__String				String;

	if ( vbRestoringDefaults )
	{
		// Display nothing - Update in nws_csetup
		return;
	}

    if ( vbForceSetOfNWAddx )
    {
        // Top status line update
        // - Init structure
    	UICOMMON__TopStatus_InitStructure( &TopStatus );
    	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
        TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__HELP_BTN];
#endif
        TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__BACK_BTN];
        TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_ForceNWAddx;
        // - Perform update
        UICOMMON__TopStatus_Update( &TopStatus );
    }
    else
   	if ( !TWBIF502__bIsMasterType02Device )
   	{
   	    // Top status line update
   	    // - Init structure
   		UICOMMON__TopStatus_InitStructure( &TopStatus );
   		// - Define requirements
   		TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_ParamAccess_Remote;
   		TopStatus.TextFlashRate      = UITFTDSP__FR__1HZ;
   		TopStatus.bFlashText         = true;
   	    // - Perform update
   	    UICOMMON__TopStatus_Update(&TopStatus);

 	    if ( TWBIF502__bDeviceWithAddressIsThisUnit( &pParamAccess->DeviceAddress ) )
   	    {
 	    	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PROMPT_LINE1] );
 	    	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PROMPT_LINE2] );
   	    }

 	    return;
   	}
    else
   	if ( pParamAccess->UserAccess.Parameter.Type == GENPRMDT__PT__INSTALL )
   	{
   	    // Top status line update
   	    // - Init structure
   		UICOMMON__TopStatus_InitStructure( &TopStatus );
   		// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
   	    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__HELP_BTN];
#endif
   	    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__BACK_BTN];
   		TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_ParamAccess_Install;
   	    // - Perform update
   	    UICOMMON__TopStatus_Update( &TopStatus );
   	}
   	else
   	{
   	    // Top status line update
   	    // - Init structure
   		UICOMMON__TopStatus_InitStructure( &TopStatus );
   		// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
   	    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__HELP_BTN];
#endif
   	    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__BACK_BTN];
   		TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_ParamAccess_Service;
   	    // - Perform update
   	    UICOMMON__TopStatus_Update( &TopStatus );
   	    //
   	    // Type/version info
   	    // - Init string
   	    GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__TYPE_VERSION_TEXT_LENGTH, &mstrTypeVersionInfo[0], &String );
   	    // - Add type/version if available
   	    if ( pParamAccess->UserAccess.ucTypeNumber == 0 )
   	    {
   	    	GENSTRDT__AddTextToString("T??V???", &String);
   	    }
   	    else
   	    {
   	    	GENSTRDT__AddCharToString( 'T', &String );
   	    	GENSTRDT__Add2DigitNumberToString( pParamAccess->UserAccess.ucTypeNumber, true, &String );
   	    	GENSTRDT__AddCharToString( 'V', &String );
   	    	GENSTRDT__Add3DigitNumberToString( pParamAccess->UserAccess.ucRevisionNumber, true, &String );
   	    }
   	    // - Display it
   	    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_TYPE_VERSION_INFO] );
   	    //
   	    // Reset button
   	    UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PRMAXS_USERIF_NC7__BN__RESET_BTN] );
   	}
    //
    // Parameter details
	// - Init strings used for text displays
	GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__PARAMETER_DESC_TEXT_LENGTH , &mstrParameterDescriptionText[0], &mParameterDescriptionString );
	GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__PARAMETER_VALUE_TEXT_LENGTH, &mstrParameterValueText[0]      , &mParameterValueString );
	//
    // Parameter info update
    if ( pParamAccess->UserAccess.Parameter.bHasBeenRetrieved )
    {
    	// Show parameter details
    	//
    	// Description string
    	// - Header
    	GENSTRDT__AssignTextToString( "Parameter: ", &mParameterDescriptionString );
    	// - Group description
    	GENSTRDT__AddCharToString( mkaucGroupDescription[pParamAccess->DeviceAddress.GroupID], &mParameterDescriptionString );
    	// - Unit Id
    	GENSTRDT__Add2DigitNumberToString( (uint16_t)pParamAccess->DeviceAddress.UnitID, true, &mParameterDescriptionString );
    	// - " ID"
    	GENSTRDT__AddTextToString( " ID", &mParameterDescriptionString );
    	// - Parameter ID
    	GENSTRDT__Add3DigitNumberToString( pParamAccess->UserAccess.Parameter.ucIdentification, true, &mParameterDescriptionString );
    	//
    	// Actual value
    	GENSTRDT__Add5DigitNumberToString( (uint32_t)( (uint32_t)pParamAccess->UserAccess.Parameter.ucActualValue * (uint32_t)pParamAccess->UserAccess.Parameter.ucScalingValue), false, &mParameterValueString );
    }
    else
    {
    	// Waiting for parameter to be retrieved
    	GENSTRDT__AssignTextToString( "Retrieving...", &mParameterDescriptionString );
    	// No value retrieved yet
    	GENSTRDT__AssignTextToString( "???", &mParameterValueString );
    }
    //
    // Display parameter info that has been setup
    //
    // Parameter description
    // - Just show text
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DESCRIPTION] );
    // - Rectangle with text
    UITFTDSP__ShowRectangleWithText( (UI_TFTSHAPE__RectangleWithTextDetails*)&mkaTextEntry[NWS_PRMAXS_USERIF_NC7__RN__PARAMETER_VALUE] );
    //
    // Detailed Info
    // - Clear pointer
	mpParameterDescription = 0;
	// - Define pointer
	if ( pParamAccess->UserAccess.DeviceTypeSelected == NWS_PRMAXS_COMMON__DT__COOLER )
	{
		// Cooler - TBD
	}
	else
   	if ( pParamAccess->UserAccess.DeviceTypeSelected == NWS_PRMAXS_COMMON__DT__HEATER )
   	{
		// Heater - TBD
   	}
   	else
   	{
		// Networker - Only controllers
   		switch ( pParamAccess->UserAccess.DeviceType02Selected )
   		{
   			case NWS_PRMAXS_COMMON__02DT__CONTROL:
   				if ( pParamAccess->UserAccess.Parameter.Type == GENPRMDT__PT__INSTALL )
   				{
   					mpParameterDescription = (PARAMDSC__ParameterDescription*)PARAMDSC__NetworkerDevice_pParameterDescription( pParamAccess->UserAccess.Parameter.ucIdentification );
   				}
   				break;

   			case NWS_PRMAXS_COMMON__02DT__SENSOR:
   				break;

   			case NWS_PRMAXS_COMMON__02DT__SENSOR_OUTDOOR:
   				break;

   			case NWS_PRMAXS_COMMON__02DT__REMOTE_ACCESS:
   				break;

   			default:
   				break;
   		}
   	}

    if ( ( mbShowDetailedInformation ) && ( pParamAccess->UserAccess.Parameter.bHasBeenRetrieved) && (mpParameterDescription != 0 ) )
    {
		GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH , &mstrParameterInfoHeadingText[0], &mParameterInfoHeadingString );
		GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH , &mstrParameterInfoLine1Text[0], &mParameterInfoLine1String );
		GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH , &mstrParameterInfoLine2Text[0], &mParameterInfoLine2String );
		GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH , &mstrParameterInfoLine3Text[0], &mParameterInfoLine3String );
		GENSTRDT__InitString( NWS_PRMAXS_USERIF_NC7__PARAMETER_INFO_TEXT_LENGTH , &mstrParameterInfoLine4Text[0], &mParameterInfoLine4String );

		GENSTRDT__AssignTextToString( mpParameterDescription->pstrHeading, &mParameterInfoHeadingString );
		GENSTRDT__AssignTextToString( mpParameterDescription->pstrText1, &mParameterInfoLine1String );
		GENSTRDT__AssignTextToString( mpParameterDescription->pstrText2, &mParameterInfoLine2String );
		GENSTRDT__AssignTextToString( mpParameterDescription->pstrText3, &mParameterInfoLine3String );
		GENSTRDT__AssignTextToString( mpParameterDescription->pstrText4, &mParameterInfoLine4String );

		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE01] );
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE02] );
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE03] );
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE04] );
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PRMAXS_USERIF_NC7__TN_PARAMETER_DETAIL_LINE05] );
   	}
    //
    // Control buttons
    // - Value update buttons, Added, emilio 09-04-15
    UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__VALUE_DN_BTN] );
    UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PRMAXS_USERIF_NC7__IN__VALUE_UP_BTN] );
	// - Previous/Next parameter buttons (Only show when not forcing networker addx)
    if ( !vbForceSetOfNWAddx )
    {
    	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PRMAXS_USERIF_NC7__BN__PREV_BTN] );
    	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PRMAXS_USERIF_NC7__BN__NEXT_BTN] );
    }
    // - Detailed info?
	if ( mpParameterDescription != 0 )
	{
		if ( mbShowDetailedInformation )
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PRMAXS_USERIF_NC7__BN__LESS_BTN] );
		}
		else
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_PRMAXS_USERIF_NC7__BN__MORE_BTN] );
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Parameter access control update - NC7 specific.
 * @param:	vbRestoringDefaults	- Flag indicating when defaults are being restored.
 * 			vbForceSetOfNWAddx	- Flag indicating when networker address is being forced.
 * 			pParamAccess		- Pointer to parameter access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PRMAXS_USERIF_NC7__UpdateControl(bool vbRestoringDefaults, bool vbForceSetOfNWAddx, NWS_PRMAXS_COMMON__ControlDetails* pParamAccess)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	if ( vbRestoringDefaults )
	{
		// Control nothing - Update in nws_csetup
		return;
	}

	if ( ( TWBIF502__bIsMasterType02Device ) || ( vbForceSetOfNWAddx ) )
	{
		switch ( (NWS_PRMAXS_USERIF_NC7__TAG_VALUE)ucTagValue )
		{
	   		case NWS_PRMAXS_USERIF_NC7__TV__LEVEL_UP_BTN:
	   			if ( pParamAccess->UserAccess.Parameter.bHasBeenRetrieved )
	   			{
	   				pParamAccess->UserAccess.Control.iValueAdjust = +1;
	   	   			RapidUpdateControl_ValueUpdate( true, pParamAccess );
	   			}
	   			break;

	   		case NWS_PRMAXS_USERIF_NC7__TV__LEVEL_DN_BTN:
	   			if ( pParamAccess->UserAccess.Parameter.bHasBeenRetrieved )
	   			{
	   				pParamAccess->UserAccess.Control.iValueAdjust = -1;
	   	   			RapidUpdateControl_ValueUpdate( false, pParamAccess );
	   			}
	    		break;

	   		case NWS_PRMAXS_USERIF_NC7__TV__MORE_LESS_BTN:
	   			if ( !mbShowDetailedInformation )
	   			{
	   				mbShowDetailedInformation = true;
	   			}
	   			else
	   			{
	   				mbShowDetailedInformation = false;
	   			}
	   			RapidUpdateControl_None();
	    		break;

	   		case NWS_PRMAXS_USERIF_NC7__TV__NEXT_BTN:
	   			pParamAccess->UserAccess.Control.ButtonPressed = NWS_PRMAXS_COMMON__CB__GET_NEXT_PARAM;
	   			RapidUpdateControl_None();
	    		break;

	   		case NWS_PRMAXS_USERIF_NC7__TV__PREV_BTN:
	   			pParamAccess->UserAccess.Control.ButtonPressed = NWS_PRMAXS_COMMON__CB__GET_PREV_PARAM;
	   			RapidUpdateControl_None();
	    		break;

	   		case NWS_PRMAXS_USERIF_NC7__TV__RESET_BTN:
	   			pParamAccess->UserAccess.Control.ButtonPressed = NWS_PRMAXS_COMMON__CB__PERFORM_RESET;
	   			RapidUpdateControl_None();
	   			break;

	   		case NWS_PRMAXS_USERIF_NC7__TV__BACK_BTN:
	   		case NWS_PRMAXS_USERIF_NC7__TV__HELP_BTN:
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
			switch ( (NWS_PRMAXS_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_PRMAXS_USERIF_NC7__TV__BACK_BTN:
		   		    if ( vbForceSetOfNWAddx )
		   		    {
		   		    	// Only allow exit if networker addx is set
		   		    	if ( pParamAccess->UserAccess.Parameter.ucActualValue != 0 ) { pParamAccess->UserAccess.Control.ButtonPressed = NWS_PRMAXS_COMMON__CB__EXIT_PARAM_ACCESS; }
		   		    }
		   		    else
		   		    {
		   	   			pParamAccess->UserAccess.Control.ButtonPressed = NWS_PRMAXS_COMMON__CB__EXIT_PARAM_ACCESS;
		   		    }
					break;

		   		case NWS_PRMAXS_USERIF_NC7__TV__HELP_BTN:
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
 * @param:	vbIncreasingValue - Whether value is increasing or not.
 * 			pParamAccess	  - Pointer to parameter access details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_ValueUpdate(bool vbIncreasingValue, NWS_PRMAXS_COMMON__ControlDetails* pParamAccess)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = ( (pParamAccess->UserAccess.Parameter.ucMaximumValue - pParamAccess->UserAccess.Parameter.ucMinimumValue) > 16);
	TouchTagValue.bIncreasing        = vbIncreasingValue;
	TouchTagValue.ucMinimum			 = pParamAccess->UserAccess.Parameter.ucMinimumValue;
	TouchTagValue.ucMaximum			 = pParamAccess->UserAccess.Parameter.ucMaximumValue;
	TouchTagValue.ucActual			 = pParamAccess->UserAccess.Parameter.ucActualValue;

	FT800_TOUCHIFC__RapidUpdateDataDefine(&TouchTagValue);

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
