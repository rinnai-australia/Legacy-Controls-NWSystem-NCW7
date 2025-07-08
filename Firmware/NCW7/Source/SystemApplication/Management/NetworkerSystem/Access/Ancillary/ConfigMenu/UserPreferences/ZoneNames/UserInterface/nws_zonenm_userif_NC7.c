//*********************************************************************************************************************************************************************************
// nws_zonenm_userif_NC7.c
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
#include		<nws_zonenm_common.h>
#include		<zoning_userif_NC7.h>
#include		<uicommon.h>
#include		<uikeybrd.h>
#include		<uitftdsp_color.h>
#include		<uitftdsp.h>
#include        <tftdspif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_ZONENM_USERIF_NC7__TAG_VALUE {

	NWS_ZONENM_USERIF_NC7__TV__HELP_BTN	  = 1,
	NWS_ZONENM_USERIF_NC7__TV__BACK_BTN	  = 2,

	NWS_ZONENM_USERIF_NC7__TV__ZONE_A_BTN = 11,
	NWS_ZONENM_USERIF_NC7__TV__ZONE_B_BTN = 12,
	NWS_ZONENM_USERIF_NC7__TV__ZONE_C_BTN = 13,
	NWS_ZONENM_USERIF_NC7__TV__ZONE_D_BTN = 14

} NWS_ZONENM_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_ZONENM_USERIF_NC7__IMAGE_NUMBER {

	NWS_ZONENM_USERIF_NC7__IN__HELP_BTN = 0,
	NWS_ZONENM_USERIF_NC7__IN__BACK_BTN = 1

} NWS_ZONENM_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ZONENM_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ZONENM_USERIF_NC7__TV__BACK_BTN  }, //   1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ZONENM_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_ZONENM_USERIF_NC7__TV__BACK_BTN  }, //   1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Containers
// - Max length allowed
#define						NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH	(ZONING_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH + 20)
// - Zone A
static char					mstrZoneADescription[NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH+1];
static GENSTRDT__String		mZoneADescription;
// - Zone B
static char					mstrZoneBDescription[NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH+1];
static GENSTRDT__String		mZoneBDescription;
// - Zone C
static char					mstrZoneCDescription[NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH+1];
static GENSTRDT__String		mZoneCDescription;
// - Zone D
static char					mstrZoneDDescription[NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH+1];
static GENSTRDT__String		mZoneDDescription;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton[] = {
		// BUTTON																																				   RECTANGLE						      TEXT
		//                Normal State,   HL,         	  		           Persist time, 					    Style,	                                 Tag,		   Color,   X,   Y,   W,   Y,  LW  	      X,   Y,  XO,  YO, Color,          	      FontSize,		      Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, 									   0,   {  {       0,   0,   0,   0,   0,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "" 							 } } },	// 0: ZoneU (Not Used)
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_ZONENM_USERIF_NC7__TV__ZONE_A_BTN,   {  {       0,  12,  56, 458,  44,  0  },  {  -1,  -1,  10,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneADescription[0] } } },	// 1: ZoneA
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_ZONENM_USERIF_NC7__TV__ZONE_B_BTN, 	{  {       0,  12, 110, 458,  44,  0  },  {  -1,  -1,  10,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneBDescription[0] } } },	// 2: ZoneB
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_ZONENM_USERIF_NC7__TV__ZONE_C_BTN,	{  {       0,  12, 164, 458,  44,  0  },  {  -1,  -1,  10,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneCDescription[0] } } },	// 3: ZoneC
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_ZONENM_USERIF_NC7__TV__ZONE_D_BTN,	{  {       0,  12, 218, 458,  44,  0  },  {  -1,  -1,  10,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDDescription[0] } } }	// 4: ZoneD
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TODO - Dynamic string array centralised for top status display, then update this string to define top status text
// - Top status maximum characters
#define			NWS_ZONENM_USERIF_NC7__TOP_STATUS_TEXT_MAX_LENGTH		100
// - Top status text place holder
static char				mstrTopStatusText_ZoneNames[NWS_ZONENM_USERIF_NC7__TOP_STATUS_TEXT_MAX_LENGTH+1];
// - Top status string
static GENSTRDT__String	mTopStatusString_ZoneNames;
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
static void						UpdateDisplay_EnterZoneName(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup);
static void						UpdateControl_EnterZoneName(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup);

static void						UpdateDisplay_ZoneSelection(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup);
static void						UpdateControl_ZoneSelection(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup);

static void						UpdateDisplay_TopStatusArea(void);
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
 * @param:	pZoneNamesEntry	- Pointer to installer setup menu access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_ZONENM_USERIF_NC7__InitBeforeStateEntry(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup)
{
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Zone names entry display update - NC7 specific.
 * @param:	pZoneNamesEntry - Pointer to zone names access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_ZONENM_USERIF_NC7__UpdateDisplay(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup)
{
	// Init String
	GENSTRDT__InitString( NWS_ZONENM_USERIF_NC7__TOP_STATUS_TEXT_MAX_LENGTH, (char*)&mstrTopStatusText_ZoneNames[0], &mTopStatusString_ZoneNames );

	if ( pZoneNamesSetup->UserAccess.bZoneNameBeingEntered )
	{
		UpdateDisplay_EnterZoneName( pZoneNamesSetup );
	}
	else
	{
		UpdateDisplay_ZoneSelection( pZoneNamesSetup );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Zone names entry control update - NC7 specific.
 * @param:	pZoneNamesEntry	- Pointer to zone names access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_ZONENM_USERIF_NC7__UpdateControl(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup)
{
	if ( pZoneNamesSetup->UserAccess.bZoneNameBeingEntered )
	{
		UpdateControl_EnterZoneName( pZoneNamesSetup );
	}
	else
	{
		UpdateControl_ZoneSelection( pZoneNamesSetup );
	}

	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Zone names entry display update - Name entry.
 * @param:	pZoneNamesEntry	- Pointer to zone names access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateDisplay_EnterZoneName(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup)
{
	// Top status
	// - Define string
	GENSTRDT__AssignTextToString( "MENU : SET ZONE NAMES - Zone ", &mTopStatusString_ZoneNames );
	switch ( pZoneNamesSetup->UserAccess.ZoneBeingUpdated )
	{
		case TWBIFUTL__ZONE__A:
			GENSTRDT__AddCharToString( 'A', &mTopStatusString_ZoneNames );
			break;

		case TWBIFUTL__ZONE__B:
			GENSTRDT__AddCharToString( 'B', &mTopStatusString_ZoneNames );
			break;

		case TWBIFUTL__ZONE__C:
			GENSTRDT__AddCharToString( 'C', &mTopStatusString_ZoneNames );
			break;

		case TWBIFUTL__ZONE__D:
			GENSTRDT__AddCharToString( 'D', &mTopStatusString_ZoneNames );
			break;

		default:
			break;
	}
	// - Update
	UpdateDisplay_TopStatusArea();
	//
	// Alpha keyboard
	if ( pZoneNamesSetup->UserAccess.bZoneWasBeingSelected )
	{
		UIKEYBRD__InitBeforeDisplay( &pZoneNamesSetup->UserAccess.NewZoneName) ;
	}
	pZoneNamesSetup->UserAccess.bZoneWasBeingSelected = false;

	UIKEYBRD__UpdateDisplay();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Zone names entry control update - Name entry.
 * @param:	pZoneNamesEntry	- Pointer to zone names access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateControl_EnterZoneName(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup)
{
	bool		bDoneButtonPressed = false;

	uint8_t    ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( ucTagValue )
	{
		case NWS_ZONENM_USERIF_NC7__TV__BACK_BTN:
			// Back button
			pZoneNamesSetup->UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__ZONE_NAME_CANCEL;
			// No rapid update
			RapidUpdateControl_None();
			break;

		default:
			UIKEYBRD__UpdateControl( ucTagValue, &bDoneButtonPressed );
			break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( bDoneButtonPressed )
	{
		// Copy keyboard string
		GENSTRDT__CopyGENString( UIKEYBRD__pTextEntryString(), &pZoneNamesSetup->UserAccess.NewZoneName );
		// Flag done
		pZoneNamesSetup->UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__ZONE_NAME_DONE;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Zone names entry display update - Zone selection.
 * @param:	pZoneNamesEntry	- Pointer to zone names access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateDisplay_ZoneSelection(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup)
{
	// Flag zone was being selected
	pZoneNamesSetup->UserAccess.bZoneWasBeingSelected = true;
	//
	// Text initialisation
	// - Zone A
	GENSTRDT__InitString( NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH, (char*)&mstrZoneADescription[0], &mZoneADescription );
	// - Zone B
	GENSTRDT__InitString( NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH, (char*)&mstrZoneBDescription[0], &mZoneBDescription );
	// - Zone C
	GENSTRDT__InitString( NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH, (char*)&mstrZoneCDescription[0], &mZoneCDescription );
	// - Zone D
	GENSTRDT__InitString( NWS_ZONENM_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH, (char*)&mstrZoneDDescription[0], &mZoneDDescription );
	//
	// Top status update
	// - Define string
	GENSTRDT__AssignTextToString( "MENU : SET ZONE NAMES - SELECT ZONE", &mTopStatusString_ZoneNames );
	// - Update
	UpdateDisplay_TopStatusArea();
	//
	// Zone A Button
	if ( pZoneNamesSetup->UserAccess.ZoneA.bInstalled )
	{
		// Setup string
		// - Zone identifier
		GENSTRDT__AssignTextToString( "Zone A   ", &mZoneADescription );
		// - Actual description
		GENSTRDT__AddTextToString( pZoneNamesSetup->UserAccess.ZoneA.pDescription->pstrText, &mZoneADescription );
		//
		// Show the button
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[TWBIFUTL__ZONE__A] );
	}
	//
	// Zone B Button
	if ( pZoneNamesSetup->UserAccess.ZoneB.bInstalled )
	{
		// Setup string
		// - Zone identifier
		GENSTRDT__AssignTextToString( "Zone B   ", &mZoneBDescription );
		// - Actual description
		GENSTRDT__AddTextToString( pZoneNamesSetup->UserAccess.ZoneB.pDescription->pstrText, &mZoneBDescription );
		//
		// Show the button
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[TWBIFUTL__ZONE__B] );
	}
	//
	// Zone C Button
	if ( pZoneNamesSetup->UserAccess.ZoneC.bInstalled )
	{
		// Setup string
		// - Zone identifier
		GENSTRDT__AssignTextToString( "Zone C   ", &mZoneCDescription );
		// - Actual description
		GENSTRDT__AddTextToString( pZoneNamesSetup->UserAccess.ZoneC.pDescription->pstrText, &mZoneCDescription );
		//
		// Show the button
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[TWBIFUTL__ZONE__C] );
	}
	//
	// Zone D Button
	if ( pZoneNamesSetup->UserAccess.ZoneD.bInstalled )
	{
		// Setup string
		// - Zone identifier
		GENSTRDT__AssignTextToString( "Zone D   ", &mZoneDDescription );
		// - Actual description
		GENSTRDT__AddTextToString( pZoneNamesSetup->UserAccess.ZoneD.pDescription->pstrText, &mZoneDDescription );
		//
		// Show the button
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[TWBIFUTL__ZONE__D] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Zone names entry control update - Zone selection.
 * @param:	pZoneNamesEntry	- Pointer to zone names access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateControl_ZoneSelection(NWS_ZONENM_COMMON__ControlDetails* pZoneNamesSetup)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;

	ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_ZONENM_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_ZONENM_USERIF_NC7__TV__ZONE_A_BTN:
		case NWS_ZONENM_USERIF_NC7__TV__ZONE_B_BTN:
		case NWS_ZONENM_USERIF_NC7__TV__ZONE_C_BTN:
		case NWS_ZONENM_USERIF_NC7__TV__ZONE_D_BTN:
		case NWS_ZONENM_USERIF_NC7__TV__BACK_BTN:
		case NWS_ZONENM_USERIF_NC7__TV__HELP_BTN:
			sucActiveTag = ucTagValue; RapidUpdateControl_None();
			break;

		default:
			break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )		// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (NWS_ZONENM_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_ZONENM_USERIF_NC7__TV__ZONE_A_BTN:
					pZoneNamesSetup->UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__ZONE_A_SELECTED;
					break;

				case NWS_ZONENM_USERIF_NC7__TV__ZONE_B_BTN:
					pZoneNamesSetup->UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__ZONE_B_SELECTED;
					break;

				case NWS_ZONENM_USERIF_NC7__TV__ZONE_C_BTN:
					pZoneNamesSetup->UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__ZONE_C_SELECTED;
					break;

				case NWS_ZONENM_USERIF_NC7__TV__ZONE_D_BTN:
					pZoneNamesSetup->UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__ZONE_D_SELECTED;
					break;

				case NWS_ZONENM_USERIF_NC7__TV__BACK_BTN:
					pZoneNamesSetup->UserAccess.Control.ButtonPressed = NWS_ZONENM_COMMON__CB__BACK;
					break;

				case NWS_ZONENM_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	Common function to update top status area.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateDisplay_TopStatusArea(void)
{
	UICOMMON__TopStatusDetails	TopStatus;
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
	TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_ZONENM_USERIF_NC7__IN__BACK_BTN];
#if defined(NCLUDE_HELP_MENU_OPTION)
	TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_ZONENM_USERIF_NC7__IN__HELP_BTN];
#endif
	TopStatus.pstrTextToDisplay = mTopStatusString_ZoneNames.pstrText;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );

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
