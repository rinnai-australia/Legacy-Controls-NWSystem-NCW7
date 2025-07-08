//*********************************************************************************************************************************************************************************
// nws_csetup_userif_NC7.c
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
#include		<genstrdt.h>
#include		<gentimdt.h>

#include		<nws_csetup_common.h>
#include		<nws_csetup_userif_NC7.h>
#include		<nws_prmaxs_common.h>
#include		<nws_prmaxs_control.h>

#include		<uitftdsp.h>
#include		<uicommon.h>
#include        <tftdspif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// String definition details - ** Array variable as text to display will vary so define here for pointer inclusion in table **
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Select Unit Button Text
// - Max length allowed
#define			NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_TEXT_LENGTH	32
// - Actual container for string
static char 			mstrSelectUnitButtonText[NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mSelectUnitButtonString;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Top Status Text
// - Max length allowed
#define			NWS_CSETUP_USERIF_NC7__TOP_STATUS_TEXT_LENGTH			64
// - Actual container for string
static char 			mstrTopStatusText[NWS_CSETUP_USERIF_NC7__TOP_STATUS_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mTopStatusString;
//
// Constant strings to be added
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - General
static const char mkstrTopStatusText_CustomSetupRestoreDefaults[] 	= "Restoring Defaults - Please wait for restart";
static const char mkstrTopStatusText_CustomSetupResettingNwkID[]  	= "Resetting Networker ID - Please wait for restart";
// - Top status (Custom Setup)
static const char mkstrTopStatusText_CustomSetup[] 	 				= "MENU : CUSTOM SETUP";
static const char mkstrTopStatusText_CustomSetupSelectUnit[]    	= "MENU : CUSTOM SETUP (SELECT UNIT)";
// - Top status (Service Access)
static const char mkstrTopStatusText_ServiceAccess[] 	 			= "MENU : SERVICE INFO";
static const char mkstrTopStatusText_ServiceAccessSelectUnit[]  	= "MENU : SERVICE INFO (SELECT UNIT)";
// - Device descriptions
static const char mkstrSelectUnit_MasterNetworker[] 				= "Master Networker";
static const char mkstrSelectUnit_SlaveNetworker[]  				= "Slave Networker";
static const char mkstrSelectUnit_ZoneSensor[]      				= "Zone Temperature Sensor";
static const char mkstrSelectUnit_OutdoorSensor[]   				= "Outdoor Temperature Sensor";
static const char mkstrSelectUnit_RemoteModule[]    				= "Remote Access Module";
static const char mkstrSelectUnit_CoolerControl[]   				= "Cooler Control";
static const char mkstrSelectUnit_HeaterControl[]  	 				= "Heater Control";
//
// Select Unit Button Number (4 buttons max)
typedef enum	NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_NUMBER {

	NWS_CSETUP_USERIF_NC7__SUBN__BUTTON01 = 0,
	NWS_CSETUP_USERIF_NC7__SUBN__BUTTON02 = 1,
	NWS_CSETUP_USERIF_NC7__SUBN__BUTTON03 = 2,
	NWS_CSETUP_USERIF_NC7__SUBN__BUTTON04 = 3

} NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_NUMBER;

#define			NWS_CSETUP_USERIF_NC7__SELECT_UNIT_MAX_BUTTONS	(NWS_CSETUP_USERIF_NC7__SUBN__BUTTON04+1)
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_CSETUP_USERIF_NC7__TAG_VALUE {

	NWS_CSETUP_USERIF_NC7__TV__HELP_BTN	  			= 1,
	NWS_CSETUP_USERIF_NC7__TV__BACK_BTN	  			= 2,

	NWS_CSETUP_USERIF_NC7__TV__LEVEL_UP_BTN 		= 11,
	NWS_CSETUP_USERIF_NC7__TV__LEVEL_DN_BTN  		= 12,
	NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_1 = 13,
	NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_2 = 14,
	NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_3 = 15,
	NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_4 = 16,
	NWS_CSETUP_USERIF_NC7__TV__RESTORE_DEFAULTS_BTN = 17,
	NWS_CSETUP_USERIF_NC7__TV__NWK_PARAM_BTN    	= 18,
	NWS_CSETUP_USERIF_NC7__TV__EVP_PARAM_BTN     	= 19,
	NWS_CSETUP_USERIF_NC7__TV__HTR_PARAM_BTN   		= 20

} NWS_CSETUP_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_CSETUP_USERIF_NC7__IMAGE_NUMBER {

	NWS_CSETUP_USERIF_NC7__IN__HELP_BTN =  0,
	NWS_CSETUP_USERIF_NC7__IN__BACK_BTN =  1,
	NWS_CSETUP_USERIF_NC7__IN__PAGE_UP  =  2,
	NWS_CSETUP_USERIF_NC7__IN__PAGE_DN  =  3

} NWS_CSETUP_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						   		 Image - Active,   X,   Y, 						   	    Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CSETUP_USERIF_NC7__TV__HELP_BTN  		}, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CSETUP_USERIF_NC7__TV__BACK_BTN  		}, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CSETUP_USERIF_NC7__TV__HELP_BTN  		}, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CSETUP_USERIF_NC7__TV__BACK_BTN  		}, // 1
#endif
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE, 425,  56, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CSETUP_USERIF_NC7__TV__LEVEL_UP_BTN  	}, // 2
	{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE, 425, 110, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_CSETUP_USERIF_NC7__TV__LEVEL_DN_BTN  	}, // 3
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_CSETUP_USERIF_NC7__BUTTON_NUMBER {

	NWS_CSETUP_USERIF_NC7__BN__NWK_RESTORE_DEFAULTS_BTN   =  0,
	NWS_CSETUP_USERIF_NC7__BN__NWK_RESET_NWK_ADDX_BTN	  =  1,
	NWS_CSETUP_USERIF_NC7__BN__NWK_LINE1_PARAM_BTN        =  2,
	NWS_CSETUP_USERIF_NC7__BN__EVP_LINE2_PARAM_BTN        =  3,
	NWS_CSETUP_USERIF_NC7__BN__HTR_LINE2_PARAM_BTN 	  	  =  4,
	NWS_CSETUP_USERIF_NC7__BN__HTR_LINE3_PARAM_BTN 	      =  5,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_1_SMALL =  6,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_2_SMALL =  7,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_3_SMALL =  8,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_4_SMALL =  9,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_1_LARGE = 10,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_2_LARGE = 11,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_3_LARGE = 12,
	NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_4_LARGE = 13

} NWS_CSETUP_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition  mkaButton[] =
{
		// BUTTON																										  				      			      	   			   // RECTANGLE						    // TEXT
		//                Normal State,   HL,                              Persist time, 					    Style,	                          			  	       Tag,	   // Color,   X,   Y,   W,   H,  LW  	     X,   Y,  XO,  YO, Color,          		     FontSize,		     Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__RESTORE_DEFAULTS_BTN, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Restore Networker Defaults" 			} } },	 //  0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__RESTORE_DEFAULTS_BTN, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Reset Networker ID" 		 			} } },	 //  1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  		  NWS_CSETUP_USERIF_NC7__TV__NWK_PARAM_BTN, {  {      0,  12, 110, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Networker Parameters" 	 				} } },	 //  2
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  		  NWS_CSETUP_USERIF_NC7__TV__EVP_PARAM_BTN, {  {      0,  12, 164, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Cooler Parameters" 	 				} } },	 //  3
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  		  NWS_CSETUP_USERIF_NC7__TV__HTR_PARAM_BTN, {  {      0,  12, 164, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Heater Parameters" 	 				} } },	 //  4
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  	      NWS_CSETUP_USERIF_NC7__TV__HTR_PARAM_BTN, {  {      0,  12, 218, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Heater Parameters" 	 				} } },	 //  5
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_1, {  {      0,  12,  56, 404,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  6
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_2, {  {      0,  12, 110, 404,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  7
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_3, {  {      0,  12, 164, 404,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  8
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_4, {  {      0,  12, 218, 404,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  9
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_1, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  10
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_2, {  {      0,  12, 110, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  11
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_3, {  {      0,  12, 164, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  12
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,     NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_4, {  {      0,  12, 218, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrSelectUnitButtonText[0] 	} } },	 //  13
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	SelectUnitDetails {												// Select Unit Control Details
																				//
	uint8_t         ucOffset;  						         					// - Index for entry, display and tag use
	uint8_t         ucMaxUnits;            										// - Last entry in the table
	uint8_t			ucAvailableUnits;											// - Number of available units
	uint8_t			aucUnitIDList[NWS_PRMAXS_COMMON__MAX_COOLER_DEVICES];		// - List of unit IDs
																				//
} SelectUnitDetails;															//
																				//
static SelectUnitDetails		mSelectUnit;									// Container

typedef struct	MessageDotDetails {												// Message Dot Control Details
																				//
	uint8_t					ucCount;											// - Dot count
	GENTIMDT__TimingCounter	TimeCounter;										// - Time counter
																				//
} MessageDotDetails;															//
																				//
static MessageDotDetails		mMessageDot;									// Container
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	NWS_CSETUP_USERIF_NC7__kMessageDotTiming = { &mMessageDot.TimeCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void						UpdateDisplay_SelectUnit(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu);
static void						UpdateControl_SelectUnit(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu);

static void						UpdateDisplay_CustomSetupMenu(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu);
static void						UpdateControl_CustomSetupMenu(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu);

static void						DrawSelectUnitButton(NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_NUMBER vSelectUnitButtonNumber, NWS_CSETUP_USERIF_NC7__BUTTON_NUMBER vButtonNumberStart, NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu);

static void						RapidUpdateControl_None(void);
static void						RapidUpdateControl_UpDnControl(bool vbIncreasingValue);
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
 * @param:	pCustomSetupMenu - Pointer to custom setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CSETUP_USERIF_NC7__InitBeforeStateEntry(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	uint8_t		ucIndex, ucUnitID;

	if ( pCustomSetupMenu->UserAccess.bSelectUnit )
	{
		mSelectUnit.ucOffset = 0;
		// Define max units available
		switch ( pCustomSetupMenu->DeviceTypeSelected )
		{
			case NWS_PRMAXS_COMMON__DT__COOLER:
				mSelectUnit.ucMaxUnits = NWS_PRMAXS_COMMON__MAX_COOLER_DEVICES;
				break;

			case NWS_PRMAXS_COMMON__DT__HEATER:
				mSelectUnit.ucMaxUnits = NWS_PRMAXS_COMMON__MAX_HEATER_DEVICES;
				break;

			default:
				mSelectUnit.ucMaxUnits = NWS_PRMAXS_COMMON__MAX_NETWORK_DEVICES;
				break;
		}
	    // Define available units
	    mSelectUnit.ucAvailableUnits = NWS_PRMAXS_CONTROL__ucNumberOfDevicesAvailable( pCustomSetupMenu->DeviceTypeSelected );
	    // Create list of unit IDs
	    ucUnitID = 1; ucIndex = 0;

		do
		{
			if ( NWS_PRMAXS_CONTROL__bDeviceIsAvailable( pCustomSetupMenu->DeviceTypeSelected, ucUnitID ) )
			{
				mSelectUnit.aucUnitIDList[ucIndex++] = ucUnitID;
			}

			ucUnitID++;
		}
		while ( ucUnitID <= mSelectUnit.ucMaxUnits );
	}
	else
	{
		// Nothing to init in this case
	}

	mMessageDot.ucCount = 0;

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Custom setup menu display update.
 * @param:	pCustomSetupMenu - Pointer to custom setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CSETUP_USERIF_NC7__UpdateDisplay(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	if ( pCustomSetupMenu->UserAccess.bRestoringDefaults )
	{
		if ( pCustomSetupMenu->UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__SERVICE )
		{
			NWS_CSETUP_USERIF_NC7__UpdateDisplay_ShowInfoPromptTextWithMovingDots( (char*)mkstrTopStatusText_CustomSetupRestoreDefaults );
		}
		else
		{
			NWS_CSETUP_USERIF_NC7__UpdateDisplay_ShowInfoPromptTextWithMovingDots( (char*)mkstrTopStatusText_CustomSetupResettingNwkID );
		}
	}
	else
	if ( pCustomSetupMenu->UserAccess.bSelectUnit )
	{
		UpdateDisplay_SelectUnit( pCustomSetupMenu );
	}
	else
	{
		UpdateDisplay_CustomSetupMenu( pCustomSetupMenu );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Custom setup menu control update.
 * @param:	pInstallerSetupMenu	- Pointer to installer setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CSETUP_USERIF_NC7__UpdateControl(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	if ( pCustomSetupMenu->UserAccess.bRestoringDefaults )
	{

	}
	else
	if ( pCustomSetupMenu->UserAccess.bSelectUnit )
	{
		UpdateControl_SelectUnit( pCustomSetupMenu );
	}
	else
	{
		UpdateControl_CustomSetupMenu( pCustomSetupMenu );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows info prompt text with moving dots in top status area.
 * @param:	pstrText - Pointer to null terminated string to assign.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_CSETUP_USERIF_NC7__UpdateDisplay_ShowInfoPromptTextWithMovingDots(char* pstrText)
{
	UICOMMON__TopStatusDetails	TopStatus;
	uint8_t						ucIndex;
	//
	// Init string
	GENSTRDT__InitString( NWS_CSETUP_USERIF_NC7__TOP_STATUS_TEXT_LENGTH, &mstrTopStatusText[0], &mTopStatusString );
	// Initial string assignment
	GENSTRDT__AssignTextToString( pstrText, &mTopStatusString );
	// Add dots
	for ( ucIndex = 1; ucIndex <= mMessageDot.ucCount; ucIndex++ )
	{
		GENSTRDT__AddTextToString(" .", &mTopStatusString);
	}
	// Time up
	if ( mMessageDot.TimeCounter.ulValue == 0 )
	{
		if ( mMessageDot.ucCount >= 3 ) { mMessageDot.ucCount = 0; } else { mMessageDot.ucCount++; }

		GENTIMDT__StartTimer_TB_UIBased( ( 50 / GENTIMDT__kucUI_TIMEBASE_10MS ), &mMessageDot.TimeCounter );
	}
	//
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
	TopStatus.pstrTextToDisplay = &mstrTopStatusText[0];
	// - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );

	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Custom setup menu display update (Unit Selection).
 * @param:	pCustomSetupMenu - Pointer to custom setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateDisplay_SelectUnit(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_NUMBER	SelectButtonNumber;
	UICOMMON__TopStatusDetails							TopStatus;
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CSETUP_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CSETUP_USERIF_NC7__IN__BACK_BTN];

    if ( pCustomSetupMenu->UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__INSTALL )
	{
		TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_CustomSetupSelectUnit;
	}
	else
	{
		TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_ServiceAccessSelectUnit;
	}
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
    //
    // Buttons to show
	if ( mSelectUnit.ucAvailableUnits <= NWS_CSETUP_USERIF_NC7__SELECT_UNIT_MAX_BUTTONS )
	{
	    if ( mSelectUnit.ucAvailableUnits == 0 )
	    {
			// No devices available anymore - Go back!
	    }
	    else
	    {
			// Large buttons
			for ( SelectButtonNumber = NWS_CSETUP_USERIF_NC7__SUBN__BUTTON01; SelectButtonNumber < mSelectUnit.ucAvailableUnits; SelectButtonNumber++ )
			{
				DrawSelectUnitButton( SelectButtonNumber, NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_1_LARGE, pCustomSetupMenu );
			}
	    }
	}
	else
	{
		// Small buttons
		for ( SelectButtonNumber = NWS_CSETUP_USERIF_NC7__SUBN__BUTTON01; SelectButtonNumber < NWS_CSETUP_USERIF_NC7__SELECT_UNIT_MAX_BUTTONS; SelectButtonNumber++ )
		{
			DrawSelectUnitButton( SelectButtonNumber, NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_1_SMALL, pCustomSetupMenu );
		}
		// Level buttons
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CSETUP_USERIF_NC7__IN__PAGE_UP] );
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CSETUP_USERIF_NC7__IN__PAGE_DN] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Custom setup menu control update (Unit Selection).
 * @param:	pCustomSetupMenu - Pointer to custom setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateControl_SelectUnit(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_CSETUP_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_1:
		case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_2:
		case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_3:
		case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_4:
   		case NWS_CSETUP_USERIF_NC7__TV__BACK_BTN:
   		case NWS_CSETUP_USERIF_NC7__TV__HELP_BTN:
   			sucActiveTag = ucTagValue;
   			RapidUpdateControl_None();
   			break;

		case NWS_CSETUP_USERIF_NC7__TV__LEVEL_UP_BTN:
			// Up decreases offset
			if ( mSelectUnit.ucOffset > 0 )
			{
				mSelectUnit.ucOffset--;
			}
			RapidUpdateControl_UpDnControl( false );
		    break;

		case NWS_CSETUP_USERIF_NC7__TV__LEVEL_DN_BTN:
			// Down increases offset - Must keep max buttons on display always
			if ( ( mSelectUnit.ucAvailableUnits - mSelectUnit.ucOffset ) > NWS_CSETUP_USERIF_NC7__SELECT_UNIT_MAX_BUTTONS )
			{
				mSelectUnit.ucOffset++;
			}
			RapidUpdateControl_UpDnControl( true );
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
			switch ( (NWS_CSETUP_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_1:
					// Set the ID of the selected button
					pCustomSetupMenu->UserAccess.ucUnitIDSelected = mSelectUnit.aucUnitIDList[NWS_CSETUP_USERIF_NC7__SUBN__BUTTON01 + mSelectUnit.ucOffset];
					// Flag select unit button
					pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__UNIT_SELECTED;
			    	break;

				case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_2:
					// Set the ID of the selected button
					pCustomSetupMenu->UserAccess.ucUnitIDSelected = mSelectUnit.aucUnitIDList[NWS_CSETUP_USERIF_NC7__SUBN__BUTTON02 + mSelectUnit.ucOffset];
					// Flag select unit button
					pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__UNIT_SELECTED;
			    	break;

				case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_3:
					// Set the ID of the selected button
					pCustomSetupMenu->UserAccess.ucUnitIDSelected = mSelectUnit.aucUnitIDList[NWS_CSETUP_USERIF_NC7__SUBN__BUTTON03 + mSelectUnit.ucOffset];
					// Flag select unit button
					pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__UNIT_SELECTED;
			    	break;

				case NWS_CSETUP_USERIF_NC7__TV__SELECT_UNIT_BUTTON_4:
					// Set the ID of the selected button
					pCustomSetupMenu->UserAccess.ucUnitIDSelected = mSelectUnit.aucUnitIDList[NWS_CSETUP_USERIF_NC7__SUBN__BUTTON04 + mSelectUnit.ucOffset];
					// Flag select unit button
					pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__UNIT_SELECTED;
					break;

				case NWS_CSETUP_USERIF_NC7__TV__BACK_BTN:
					pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__BACK;
					break;

		    	case NWS_CSETUP_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	Custom setup menu display update (Menu Selection).
 * @param:	pCustomSetupMenu - Pointer to custom setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateDisplay_CustomSetupMenu(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	UICOMMON__TopStatusDetails	TopStatus;
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CSETUP_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_CSETUP_USERIF_NC7__IN__BACK_BTN];

    if ( pCustomSetupMenu->UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__INSTALL )
	{
		TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_CustomSetup;
	}
	else
	{
		TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_ServiceAccess;
	}
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
	//
	// Defaults can always be restored
	if ( pCustomSetupMenu->UserAccess.ParameterType == NWS_PRMAXS_COMMON__APT__SERVICE )
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CSETUP_USERIF_NC7__BN__NWK_RESTORE_DEFAULTS_BTN] );
	}
	else
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CSETUP_USERIF_NC7__BN__NWK_RESET_NWK_ADDX_BTN] );
	}
	// Only a master can access parameters, emilio 04-01-16
	if ( TWBIF502__bIsMasterType02Device )
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CSETUP_USERIF_NC7__BN__NWK_LINE1_PARAM_BTN] );
		//
		// Check for cooler and heater availability
		if ( ( NWS_PRMAXS_CONTROL__bADeviceIsAvailable(NWS_PRMAXS_COMMON__DT__COOLER) ) && ( NWS_PRMAXS_CONTROL__bADeviceIsAvailable(NWS_PRMAXS_COMMON__DT__HEATER) ) )
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CSETUP_USERIF_NC7__BN__EVP_LINE2_PARAM_BTN] );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CSETUP_USERIF_NC7__BN__HTR_LINE3_PARAM_BTN] );
		}
		else
		if ( NWS_PRMAXS_CONTROL__bADeviceIsAvailable( NWS_PRMAXS_COMMON__DT__COOLER ) )
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CSETUP_USERIF_NC7__BN__EVP_LINE2_PARAM_BTN] );
		}
		else
		if ( NWS_PRMAXS_CONTROL__bADeviceIsAvailable( NWS_PRMAXS_COMMON__DT__HEATER ) )
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_CSETUP_USERIF_NC7__BN__HTR_LINE2_PARAM_BTN] );
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Custom setup menu control update (Menu Selection).
 * @param:	pCustomSetupMenu - Pointer to custom setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateControl_CustomSetupMenu(NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_CSETUP_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_CSETUP_USERIF_NC7__TV__RESTORE_DEFAULTS_BTN:
	    case NWS_CSETUP_USERIF_NC7__TV__NWK_PARAM_BTN:
		case NWS_CSETUP_USERIF_NC7__TV__EVP_PARAM_BTN:
		case NWS_CSETUP_USERIF_NC7__TV__HTR_PARAM_BTN:
   		case NWS_CSETUP_USERIF_NC7__TV__BACK_BTN:
   		case NWS_CSETUP_USERIF_NC7__TV__HELP_BTN:
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
			switch ( (NWS_CSETUP_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_CSETUP_USERIF_NC7__TV__RESTORE_DEFAULTS_BTN:
		    		pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__RESTORE_DEFAULTS;
					break;

		    	case NWS_CSETUP_USERIF_NC7__TV__NWK_PARAM_BTN:
		    		pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__NETWORKER_PARAMS;
		    		break;

		    	case NWS_CSETUP_USERIF_NC7__TV__EVP_PARAM_BTN:
		    		pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__COOLER_PARAMS;
		    		break;

		    	case NWS_CSETUP_USERIF_NC7__TV__HTR_PARAM_BTN:
		    		pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__HEATER_PARAMS;
		    		break;

		    	case NWS_CSETUP_USERIF_NC7__TV__BACK_BTN:
		    		pCustomSetupMenu->UserAccess.Control.ButtonPressed = NWS_CSETUP_COMMON__CB__BACK;
		    		break;

		    	case NWS_CSETUP_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	Draw the unit selection button.
 * @param:	vSelectUnitButtonNumber	- Index indicating button to draw: 0 = Button 1
 * 												   					   1 = Button 2
 * 												   					   2 = Button 3
 * 												   					   3 = Button 4
 * 			vButtonNumberStart - Button small/large start value.
 * 			pCustomSetupMenu   - Pointer to custom setup menu details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DrawSelectUnitButton(NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_NUMBER vSelectUnitButtonNumber, NWS_CSETUP_USERIF_NC7__BUTTON_NUMBER vButtonNumberStart, NWS_CSETUP_COMMON__ControlDetails* pCustomSetupMenu)
{
	NWS_CSETUP_USERIF_NC7__BUTTON_NUMBER	ButtonNumber = vButtonNumberStart + vSelectUnitButtonNumber;
	uint8_t									ucUnitID = mSelectUnit.aucUnitIDList[mSelectUnit.ucOffset + vSelectUnitButtonNumber];

	if ( ButtonNumber < NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_1_SMALL ) { return; }

	if ( ButtonNumber > NWS_CSETUP_USERIF_NC7__BN__SELECT_UNIT_BUTTON_4_LARGE ) { return; }
	//
	// Define the text for the button
	// - Init string
	GENSTRDT__InitString( NWS_CSETUP_USERIF_NC7__SELECT_UNIT_BUTTON_TEXT_LENGTH, &mstrSelectUnitButtonText[0], &mSelectUnitButtonString );
	// - Add descriptor
	switch ( pCustomSetupMenu->DeviceTypeSelected )
	{
		case NWS_PRMAXS_COMMON__DT__COOLER:
			GENSTRDT__AddCharToString( 'E', &mSelectUnitButtonString );
			break;

		case NWS_PRMAXS_COMMON__DT__HEATER:
			GENSTRDT__AddCharToString( 'H', &mSelectUnitButtonString );
			break;

		default:
			GENSTRDT__AddCharToString( 'n', &mSelectUnitButtonString );
			break;
	}
	// - Add number
	GENSTRDT__Add2DigitNumberToString( ucUnitID, true, &mSelectUnitButtonString );
	// - Add ' - '
	GENSTRDT__AddCharToString( ' ', &mSelectUnitButtonString );
	GENSTRDT__AddCharToString( '-', &mSelectUnitButtonString );
	GENSTRDT__AddCharToString( ' ', &mSelectUnitButtonString );
	// - Add description
	if ( pCustomSetupMenu->DeviceTypeSelected == NWS_PRMAXS_COMMON__DT__COOLER )
	{
		GENSTRDT__AddTextToString( (char*)mkstrSelectUnit_CoolerControl, &mSelectUnitButtonString );
	}
	else
	if ( pCustomSetupMenu->DeviceTypeSelected == NWS_PRMAXS_COMMON__DT__HEATER )
	{
		GENSTRDT__AddTextToString( (char*)mkstrSelectUnit_HeaterControl, &mSelectUnitButtonString );
	}
	else
	if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsMasterControl( ucUnitID ) )
	{
		GENSTRDT__AddTextToString( (char*)mkstrSelectUnit_MasterNetworker, &mSelectUnitButtonString );
	}
	else
	if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsSlaveControl( ucUnitID ) )
	{
		GENSTRDT__AddTextToString( (char*)mkstrSelectUnit_SlaveNetworker, &mSelectUnitButtonString );
	}
	else
	if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsSensor( ucUnitID ) )
	{
		GENSTRDT__AddTextToString( (char*)mkstrSelectUnit_ZoneSensor, &mSelectUnitButtonString );
	}
	else
	if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsOutdoorSensor( ucUnitID ) )
	{
		GENSTRDT__AddTextToString( (char*)mkstrSelectUnit_OutdoorSensor, &mSelectUnitButtonString );
	}
	else
	if ( NWS_PRMAXS_CONTROL__Networker_bDeviceIsRemoteAccessModule( ucUnitID ) )
	{
		GENSTRDT__AddTextToString( (char*)mkstrSelectUnit_RemoteModule, &mSelectUnitButtonString );
	}
	//
	// Now draw the button
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[ButtonNumber] );

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
 * @brief:	Rapid update control - Up/Down buttons.
 * @param:	vbIncreasingValue - Increasing offset value.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_UpDnControl(bool vbIncreasingValue)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = false;
	TouchTagValue.bIncreasing        = vbIncreasingValue;
	TouchTagValue.ucMinimum			 = 0;
	TouchTagValue.ucMaximum			 = mSelectUnit.ucAvailableUnits-1;
	TouchTagValue.ucActual			 = mSelectUnit.ucOffset;

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

	return;
}
//*********************************************************************************************************************************************************************************



