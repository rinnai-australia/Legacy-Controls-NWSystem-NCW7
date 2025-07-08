//*********************************************************************************************************************************************************************************
// nws_svcinf_userif_NC7.c
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
#include		<nws_svcinf_common.h>
#include		<twbif502.h>
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
static const char 	mkstrTopStatusText[] = "ALERT";
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_SVCINF_USERIF_NC7__TAG_VALUE {

	NWS_SVCINF_USERIF_NC7__TV__HELP_BTN	 = 1,
	NWS_SVCINF_USERIF_NC7__TV__BACK_BTN	 = 61,

	NWS_SVCINF_USERIF_NC7__TV__OK_BTN    = 71,
	NWS_SVCINF_USERIF_NC7__TV__RESET_BTN = 72,
	NWS_SVCINF_USERIF_NC7__TV__CLEAR_BTN = 73

} NWS_SVCINF_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_SVCINF_USERIF_NC7__IMAGE_NUMBER {

	NWS_SVCINF_USERIF_NC7__IN__HELP_BTN    = 0,
	NWS_SVCINF_USERIF_NC7__IN__BACK_BTN    = 1,
	NWS_SVCINF_USERIF_NC7__IN__ALERT_ICON  = 2

} NWS_SVCINF_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SVCINF_USERIF_NC7__TV__HELP_BTN  }, //  0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SVCINF_USERIF_NC7__TV__BACK_BTN  }, //  1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SVCINF_USERIF_NC7__TV__HELP_BTN  }, //  0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_SVCINF_USERIF_NC7__TV__BACK_BTN  }, //  1
#endif
	{   	 UITFTDSP_IMAGE_COMMON__IN__ALERT_ICON, 	               UITFTDSP_IMAGE__NO_IMAGE,  15,  65, 									   	   0,								   -1  }, //  2
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Containers
// - Max length allowed
#define				NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH		100
// - Actual containers for strings (Defined here for array inclusion)
static char 		mstrServiceInfoPromptTextLine1[NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH+1];
static char 		mstrServiceInfoPromptTextLine2[NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH+1];
static char 		mstrServiceInfoPromptTextLine3[NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH+1];
static char 		mstrServiceInfoPromptTextLine4[NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH+1];
static char 		mstrServiceInfoPromptTextLine5[NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH+1];	// ContactInfo 10-11-20
static char 		mstrServiceInfoPromptTextLine6[NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH+1];	// ContactInfo 10-11-20
static char 		mstrServiceInfoPromptTextLine7[NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH+1];	// ContactInfo 10-11-20
//
// Number - Use as index to mkaText[]
typedef enum	NWS_SVCINF_USERIF_NC7__TEXT_NUMBER {

	NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE1 = 0,
	NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE2 = 1,
	NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE3 = 2,
	NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE4 = 3,
	NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE5 = 4,
	NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE6 = 5,
	NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE7 = 6

} NWS_SVCINF_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails	mkaText[] = {

	//   X,   Y,  X0,  Y0, 			        Color,         		      FontSize,  		    Justification, String Label
	{   55,  68,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,   UITFTDSP_TEXT__TJ__LEFT, &mstrServiceInfoPromptTextLine1[0]   },   // 0
	{   55, 118,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, &mstrServiceInfoPromptTextLine2[0]   },   // 1
	{   55, 138,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, &mstrServiceInfoPromptTextLine3[0]   },   // 2
	/*
#ifdef		USE_BRIVIS_LOGO
	{   55, 158,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, &mstrServiceInfoPromptTextLine4[0]   },   // 3
	{   10, 228,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, "P: 1300 BRIVIS (1300 274 847)"   	},   // 5
	{   10, 248,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, "W: brivis.com.au"				  	},   // 6
#else
	{   55, 158,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, "               "   					},   // 3
	{   10, 228,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, "P: 1300 274 847"   					},   // 5
	{   10, 248,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, "               "				  	},   // 6
#endif
*/ // No more brivis logo distinction - All Service Contact Setting related, ContactInfo 10-11-20
	{   55, 158,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT,   &mstrServiceInfoPromptTextLine4[0] },   // 3
	{  240, 188,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__CENTER, &mstrServiceInfoPromptTextLine5[0] },   // 4
	{   10, 228,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT,   &mstrServiceInfoPromptTextLine6[0] },   // 5
	{   10, 248,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT,   &mstrServiceInfoPromptTextLine7[0]	},   // 6
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_SVCINF_USERIF_NC7__BUTTON_NUMBER {	// Button Number - Use as index to : mkaButton[]

	NWS_SVCINF_USERIF_NC7__BN__OK_BTN    	=  0,
	NWS_SVCINF_USERIF_NC7__BN__RESET_BTN    =  1,
	NWS_SVCINF_USERIF_NC7__BN__CLEAR_BTN    =  2

} NWS_SVCINF_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton[] =
{
		// BUTTON																										  				      			      		   RECTANGLE						     TEXT
		//                Normal State,   HL,                              Persist time, 					    Style,	                          			   Tag,	      Color,   X,   Y,   W,   H,  LW  	     X,   Y,  XO,  YO, Color,          		     FontSize,		      Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,           NWS_SVCINF_USERIF_NC7__TV__OK_BTN, {  {      0, 350, 233, 120,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Ok"	 	} } },	 //  8
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,        NWS_SVCINF_USERIF_NC7__TV__RESET_BTN, {  {      0, 350, 233, 120,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Reset"	} } },	 //  1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,        NWS_SVCINF_USERIF_NC7__TV__CLEAR_BTN, {  {      0, 350, 233, 120,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Clear"	} } },	 //  2
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
static void						DefineMessage(NWS_SVCINF_COMMON__ControlDetails* pServiceInfo);

static void						SetInfoPromptTextLine2(const char* pstrPrompt);
static void						SetInfoPromptTextLine3(const char* pstrPrompt);
static void						SetInfoPromptTextLine4(const char* pstrPrompt);

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
 * @brief:	Service info display update - NC7 specific.
 * @param:	pServiceInfo - Pointer to services info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SVCINF_USERIF_NC7__UpdateDisplay(NWS_SVCINF_COMMON__ControlDetails* pServiceInfo)
{
	UICOMMON__TopStatusDetails	TopStatus;
	//
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 			= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_SVCINF_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 			= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_SVCINF_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
	//
	// Prompt
	// - Show image
	UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_SVCINF_USERIF_NC7__IN__ALERT_ICON] );
	// - Define message
	DefineMessage( pServiceInfo );
	// - Show it
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE1] );
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE2] );
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE3] );
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE4] );
    //
    // Button
    switch ( pServiceInfo->Details.MessageType )
    {
    	case NWS_SVCINF_COMMON__MT__FAULT_TRANSIENT:
    		// - Fault : Transient
    		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_SVCINF_USERIF_NC7__BN__OK_BTN] );
    		break;

    	case NWS_SVCINF_COMMON__MT__FAULT_LOCKOUT:
    		// - Fault : Lockout
    	    if ( TWBIF502__bIsMasterType02Device )
    	    {
    	    	// Allow reset only by master
    	    	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_SVCINF_USERIF_NC7__BN__RESET_BTN] );
    	    }
    		break;

    	case NWS_SVCINF_COMMON__MT__SERVICE_NOTIFY:
    		// - Service Notification
    	    if ( TWBIF502__bIsMasterType02Device )
    	    {
    	    	// Allow clear only by master
        		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[NWS_SVCINF_USERIF_NC7__BN__CLEAR_BTN] );
    	    }
    		break;

    	default:
    		break;
    }

    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE5] );
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE6] );
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_SVCINF_USERIF_NC7__TN__PROMPT_LINE7] );

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Service info control update - NC7 specific.
 * @param:	pServiceInfo - Pointer to services info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SVCINF_USERIF_NC7__UpdateControl(NWS_SVCINF_COMMON__ControlDetails* pServiceInfo)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_SVCINF_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
   		case NWS_SVCINF_USERIF_NC7__TV__BACK_BTN:
   		case NWS_SVCINF_USERIF_NC7__TV__OK_BTN:
   		case NWS_SVCINF_USERIF_NC7__TV__RESET_BTN:
   		case NWS_SVCINF_USERIF_NC7__TV__CLEAR_BTN:
   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
    		break;

   		case NWS_SVCINF_USERIF_NC7__TV__HELP_BTN:
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
			switch ( (NWS_SVCINF_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_SVCINF_USERIF_NC7__TV__BACK_BTN:
					pServiceInfo->UserAccess.Control.ButtonPressed = NWS_SVCINF_COMMON__CB__BACK;
					break;

				case NWS_SVCINF_USERIF_NC7__TV__OK_BTN:
					pServiceInfo->UserAccess.Control.ButtonPressed = NWS_SVCINF_COMMON__CB__OK;
					break;

				case NWS_SVCINF_USERIF_NC7__TV__RESET_BTN:
					pServiceInfo->UserAccess.Control.ButtonPressed = NWS_SVCINF_COMMON__CB__RESET;
					break;

				case NWS_SVCINF_USERIF_NC7__TV__CLEAR_BTN:
					pServiceInfo->UserAccess.Control.ButtonPressed = NWS_SVCINF_COMMON__CB__CLEAR;
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
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DefineMessage(NWS_SVCINF_COMMON__ControlDetails* pServiceInfo)				// Function change - ContactInfo 10-11-20
{
	GENSTRDT__String		Text;
	//
	// Line 1 Details
	// - Init string
	GENSTRDT__InitString( NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrServiceInfoPromptTextLine1[0], &Text );
	// - Define
	switch ( pServiceInfo->Details.GroupID )
	{
		case TWBCOMIF__GID__GRP01:
			GENSTRDT__AddTextToString( "Evap Cooler", &Text );
			break;

		case TWBCOMIF__GID__GRP02:
			if ( pServiceInfo->Details.UnitID == 11 )
			{
				GENSTRDT__AddTextToString( "Touch Wi-Fi", &Text );
			}
			else
			{
				// Can we use fw code version details to determine if an N-T1, N-C6 or N-C7???
				GENSTRDT__AddTextToString( "Networker", &Text );
			}
			break;

		case TWBCOMIF__GID__GRP03:
			GENSTRDT__AddTextToString( "Heater", &Text );
			break;

		case TWBCOMIF__GID__GRP04:
			GENSTRDT__AddTextToString( "Add-on Cooler", &Text );
			break;

		default:
			// This cannot happen
			break;
	}
	if ( pServiceInfo->Details.MessageType == NWS_SVCINF_COMMON__MT__SERVICE_NOTIFY )
	{
		GENSTRDT__AddTextToString( " fan run hours above limit!", &Text );
	}
	else
	{
		GENSTRDT__AddTextToString( " fault detected - Code#", &Text );
		GENSTRDT__Add2DigitNumberToString( pServiceInfo->Details.ucFaultCode, true, &Text );
	}
	//
	// Remaining Lines
	if ( pServiceInfo->Details.MessageType == NWS_SVCINF_COMMON__MT__SERVICE_NOTIFY )
	{
	    if ( TWBIF502__bIsMasterType02Device )
	    {
			SetInfoPromptTextLine2( "Pressing the CLEAR button will reset the fan run hours." );
	    }
	    else
	    {
			SetInfoPromptTextLine2( "The fan run hours can be reset from the master networker." );
	    }
	    SetInfoPromptTextLine3( "However to ensure your appliance operates at its optimum level" );

	    SetInfoPromptTextLine4( "of performance it is recommended to have it serviced by:" );
	}
	else
	if ( pServiceInfo->Details.MessageType == NWS_SVCINF_COMMON__MT__FAULT_LOCKOUT )
	{
		SetInfoPromptTextLine2( "This type of fault has disrupted operation of your appliance." );

	    if ( TWBIF502__bIsMasterType02Device )
	    {
			SetInfoPromptTextLine3( "Pressing the RESET button will reset the appliance." );
	    }
	    else
	    {
			SetInfoPromptTextLine3( "The appliance can be RESET from the master networker." );
	    }

	    SetInfoPromptTextLine4( "If the problem persists contact:" );
	}
	else
	{
		SetInfoPromptTextLine2( "This type of fault will not disrupt operation of your appliance" );
		SetInfoPromptTextLine3( "and may clear after some time." );

		SetInfoPromptTextLine4( "However if the problem persists contact:" );
	}
	//
	// Show contact name to LINE 5...
	// - Init string container
	GENSTRDT__InitString( NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrServiceInfoPromptTextLine5[0], &Text );
	// - Add text - P: #### ####
	GENSTRDT__AssignTextToString( NWSUTILS__pstrServiceContactName(), &Text );
	//
	// Show phone number - LINE 6
	// - Init string container
	GENSTRDT__InitString( NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrServiceInfoPromptTextLine6[0], &Text );
	// - Add text - P: #### ####
	GENSTRDT__AssignTextToString( "P: ", &Text ); GENSTRDT__AddTextToString( NWSUTILS__pstrServiceContactPhoneNumber(), &Text );
	//
	// Show website		 - LINE 7
	// - Init string container
	GENSTRDT__InitString( NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrServiceInfoPromptTextLine7[0], &Text );
	// - Add text - W: #### ####
	GENSTRDT__AssignTextToString( "W: ", &Text); GENSTRDT__AddTextToString( NWSUTILS__pstrServiceContactWebsite(), &Text );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:  Initialises the information prompt ready for display.
 * @param:	pstrPrompt - Pointer to the text to display.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						SetInfoPromptTextLine2(const char* pstrPrompt)
{
	GENSTRDT__String		Text;

	GENSTRDT__InitString( NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrServiceInfoPromptTextLine2[0], &Text );
	GENSTRDT__AssignTextToString( (char*)pstrPrompt, &Text );

	return;
}
static void						SetInfoPromptTextLine3(const char* pstrPrompt)
{
	GENSTRDT__String		Text;

	GENSTRDT__InitString(NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrServiceInfoPromptTextLine3[0], &Text);
	GENSTRDT__AssignTextToString((char*)pstrPrompt, &Text);

	return;
}
static void						SetInfoPromptTextLine4(const char* pstrPrompt)
{
	GENSTRDT__String		Text;

	GENSTRDT__InitString(NWS_SVCINF_USERIF_NC7__INFO_PROMPT_LENGTH, &mstrServiceInfoPromptTextLine4[0], &Text);
	GENSTRDT__AssignTextToString((char*)pstrPrompt, &Text);

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
