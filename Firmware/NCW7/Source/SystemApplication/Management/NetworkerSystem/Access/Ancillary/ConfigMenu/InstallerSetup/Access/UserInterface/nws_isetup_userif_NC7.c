//*********************************************************************************************************************************************************************************
// nws_isetup_userif_NC7.c
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
#include		<nws_isetup_common.h>

#include		<uitftdsp.h>
#include		<uicommon.h>
#include        <tftdspif.h>
#include		<nws_access_common.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const char mkstrTopStatusText[] = "MENU : INSTALLER SETUP";
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_ISETUP_USERIF_NC7__TAG_VALUE {

	NWS_ISETUP_USERIF_NC7__TV__HELP_BTN	  		= 1,
	NWS_ISETUP_USERIF_NC7__TV__BACK_BTN	  		= 2,

	NWS_ISETUP_USERIF_NC7__TV__CUST_SETUP_BTN   = 11,
	NWS_ISETUP_USERIF_NC7__TV__SETUP_ASST_BTN   = 12,
	NWS_ISETUP_USERIF_NC7__TV__SYS_INFO_BTN     = 13

} NWS_ISETUP_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_ISETUP_USERIF_NC7__IMAGE_NUMBER {

	NWS_ISETUP_USERIF_NC7__IN__HELP_BTN =  0,
	NWS_ISETUP_USERIF_NC7__IN__BACK_BTN =  1

} NWS_ISETUP_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ISETUP_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ISETUP_USERIF_NC7__TV__BACK_BTN  }, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ISETUP_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ISETUP_USERIF_NC7__TV__BACK_BTN  }, // 1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_ISETUP_USERIF_NC7__BUTTON_NUMBER {

	NWS_ISETUP_USERIF_NC7__BN__CUST_SETUP_BTN     =  0,
	NWS_ISETUP_USERIF_NC7__BN__SETUP_ASST_BTN     =  1,
	NWS_ISETUP_USERIF_NC7__BN__SYS_INFO_BTN       =  2,

} NWS_ISETUP_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton[] = {
		// BUTTON																										  				      			   		       RECTANGLE						     TEXT
		//                Normal State,   HL,                              Persist time, 					    Style,	                          			 Tag,         Color,   X,   Y,   W,   Y,  LW  	     X,   Y,  XO,  YO, Color,          		     FontSize,		 	 Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_ISETUP_USERIF_NC7__TV__CUST_SETUP_BTN,   {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Custom Setup" 		 } } },	 //  0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_ISETUP_USERIF_NC7__TV__SETUP_ASST_BTN,   {  {      0,  12, 110, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Setup Assistant" 	 } } },	 //  1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,   NWS_ISETUP_USERIF_NC7__TV__SYS_INFO_BTN,   {  {      0,  12, 164, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "System Information" } } },	 //  2
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
 * @param:	pInstallerSetupMenu - Pointer to installer setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_ISETUP_USERIF_NC7__InitBeforeStateEntry(NWS_ISETUP_COMMON__ControlDetails* pInstallerSetupMenu)
{
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Installer setup menu display update.
 * @param:	pInstallerSetupMenu	- Pointer to installer setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_ISETUP_USERIF_NC7__UpdateDisplay(NWS_ISETUP_COMMON__ControlDetails* pInstallerSetupMenu)
{
	UICOMMON__TopStatusDetails	TopStatus;
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_ISETUP_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_ISETUP_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
    //
    // Show the menu buttons
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_ISETUP_USERIF_NC7__BN__CUST_SETUP_BTN] );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_ISETUP_USERIF_NC7__BN__SETUP_ASST_BTN] );
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_ISETUP_USERIF_NC7__BN__SYS_INFO_BTN]   );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Installer setup menu control update.
 * @param:	pInstallerSetupMenu	- Pointer to installer setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_ISETUP_USERIF_NC7__UpdateControl(NWS_ISETUP_COMMON__ControlDetails* pInstallerSetupMenu)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_ISETUP_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_ISETUP_USERIF_NC7__TV__CUST_SETUP_BTN:
		case NWS_ISETUP_USERIF_NC7__TV__SETUP_ASST_BTN:
		case NWS_ISETUP_USERIF_NC7__TV__SYS_INFO_BTN:
   		case NWS_ISETUP_USERIF_NC7__TV__BACK_BTN:
   		case NWS_ISETUP_USERIF_NC7__TV__HELP_BTN:
   			sucActiveTag = ucTagValue;
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
			switch ( (NWS_ISETUP_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_ISETUP_USERIF_NC7__TV__CUST_SETUP_BTN:
					pInstallerSetupMenu->UserAccess.Control.ButtonPressed = NWS_ISETUP_COMMON__CB__CUSTOM_SETUP;
					break;

				case NWS_ISETUP_USERIF_NC7__TV__SETUP_ASST_BTN:
					// TODO - Setup Assistant??
					break;

				case NWS_ISETUP_USERIF_NC7__TV__SYS_INFO_BTN:
					// TODO - System Information??
					break;

	   			case NWS_ISETUP_USERIF_NC7__TV__BACK_BTN:
	   				pInstallerSetupMenu->UserAccess.Control.ButtonPressed = NWS_ISETUP_COMMON__CB__BACK;
					break;

	   			case NWS_ISETUP_USERIF_NC7__TV__HELP_BTN:
					// TODO - Help Screen??
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
