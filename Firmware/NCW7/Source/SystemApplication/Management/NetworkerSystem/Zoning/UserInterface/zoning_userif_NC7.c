//*********************************************************************************************************************************************************************************
// zoning_userif_NC7.c
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
#include		<zoning_common.h>
#include        <zoning_userif_NC7.h>
#include		<uitftdsp.h>
#include		<uicommon.h>
#include        <tftdspif.h>
#include		<nwsutils.h>
#include		<genstrdt.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Initial display time
#define			ZONING_USERIF_NC7__ON_BUTTON_FLASH_SHOW_INTERVAL_TBUI	( 50 / GENTIMDT__kucUI_TIMEBASE_10MS )
// Time to keep display current with no user activity - 30s
#define			ZONING_USERIF_NC7__SCREEN_CANCEL_NO_ACTIVITY_TBUI1S		30
//
// Zone heading information
static const char 	mkstrTopStatusText_ZoneProgram[]   = "ZONE SELECTION : PROGRAMMING";
static const char 	mkstrTopStatusText_ZoneNormalOpn[] = "ZONE SELECTION : OPERATING";

/*
static const char*	mkastrZoneDefaultDescription[] = {

	"",
	"",
	"",
	"",
	""
};
*/ // Above causes zone descriptions to not be defaulted to null, instead replaced with below, NC7090_OTSCHECK
static const char	mkstrZoneDefaultDescription[] = "";

//
// Zone Descriptions - Placed here for table inlcusion
typedef struct	ZoneDetails {																// Zone Details
																							//
	char				strDescription[ZONING_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH+1];	// - Description container
	GENSTRDT__String	Description;														// - Description string
	bool				bDescriptionAltered;												// - Description altered
																							//
} ZoneDetails;																				//
																							//
static ZoneDetails	maZone[TWBIFUTL__MAXZONES];												// Containers
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	ZONING_USERIF_NC7__TAG_VALUE {

	ZONING_USERIF_NC7__TV__HELP_BTN  = 1,
	ZONING_USERIF_NC7__TV__BACK_BTN  = 2,

	ZONING_USERIF_NC7__TV__ON_A_BTN  = 11,
	ZONING_USERIF_NC7__TV__OFF_A_BTN = 12,
	ZONING_USERIF_NC7__TV__ON_B_BTN  = 13,
	ZONING_USERIF_NC7__TV__OFF_B_BTN = 14,
	ZONING_USERIF_NC7__TV__ON_C_BTN  = 15,
	ZONING_USERIF_NC7__TV__OFF_C_BTN = 16,
	ZONING_USERIF_NC7__TV__ON_D_BTN  = 17,
	ZONING_USERIF_NC7__TV__OFF_D_BTN = 18,
	ZONING_USERIF_NC7__TV__DONE_BTN  = 19

} ZONING_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	ZONING_USERIF_NC7__IMAGE_NUMBER {

	ZONING_USERIF_NC7__IN__HELP_BTN = 0,
	ZONING_USERIF_NC7__IN__BACK_BTN = 1

} ZONING_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						   		 Image - Active,   X,   Y, 						   		Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, ZONING_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, ZONING_USERIF_NC7__TV__BACK_BTN  }, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, ZONING_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, ZONING_USERIF_NC7__TV__BACK_BTN  }, // 1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Outer Rectangle
static const UI_TFTSHAPE__Rectangle					mkaOuterRectangle[] = {
	// 			             Color,    X,   Y,   W,   H,  LW
	{ UITFTDSP_COLOR__BUTTON_OUTER,    0,   0,   0,   0,   0 },	//  0: ZoneU (Not used)
	{ UITFTDSP_COLOR__BUTTON_OUTER,   15,  60, 207,  75,  18 },	//  1: ZoneA
	{ UITFTDSP_COLOR__BUTTON_OUTER,  252,  60, 207,  75,  18 },	//  2: ZoneB
	{ UITFTDSP_COLOR__BUTTON_OUTER,   15, 146, 207,  75,  18 },	//  3: ZoneC
	{ UITFTDSP_COLOR__BUTTON_OUTER,  252, 146, 207,  75,  18 },	//  4: ZoneD
};
//
// Inner Rectangle
static const UI_TFTSHAPE__Rectangle					mkaInnerRectangle[] = {

	// 			        Color,    X,   Y,   W,   H,  LW
	{ UITFTDSP_COLOR__BACKGROUND,    0,   0,   0,   0,   0 },	//  0: ZoneU (Not used)
	{ UITFTDSP_COLOR__BACKGROUND,   17,  62, 203,  71,  16 },	//  1: ZoneA
	{ UITFTDSP_COLOR__BACKGROUND,  254,  62, 203,  71,  16 },	//  2: ZoneB
	{ UITFTDSP_COLOR__BACKGROUND,   17, 148, 203,  71,  16 },	//  3: ZoneC
	{ UITFTDSP_COLOR__BACKGROUND,  254, 148, 203,  71,  16 },	//  4: ZoneD
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details - Split into On buttons, Off buttons and general buttons
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Zone 'On' buttons
// - Passive
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton_ZoneOnPassive[] = {
		// BUTTON																									           RECTANGLE						      TEXT
		//                Normal State,    HL, Persist time, 					    Style,	                           Tag,        Color,   X,   Y,   W,   Y,  LW  	      X,   Y,  XO,  YO, Color,          		  FontSize,		        Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, 							     0, {  {       0,   0,   0,   0,   0,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "" 	 } } },	 //  0: ZoneU (Not Used)
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_A_BTN, {  {       0,  27,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  1: ZoneA
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_B_BTN, {  {       0, 264,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  2: ZoneB
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_C_BTN, {  {       0,  27, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  3: ZoneC
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_D_BTN, {  {       0, 264, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  4: ZoneD
};
// - Active
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton_ZoneOnActive[] = {
		// BUTTON																										       RECTANGLE						      TEXT
		//                Normal State,    HL, Persist time, 					    Style,	                           Tag,	  	   Color,   X,   Y,   W,   Y,  LW  	      X,   Y,  XO,  YO, Color,          		  FontSize,		        Justification, Text
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, 							     0, {  {       0,   0,   0,   0,   0,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "" 	 } } },	 //  0: ZoneU (Not Used)
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_A_BTN, {  {       0,  27,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  1: ZoneA
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_B_BTN, {  {       0, 264,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  2: ZoneB
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_C_BTN, {  {       0,  27, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  3: ZoneC
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__ON_D_BTN, {  {       0, 264, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "On"  } } },	 //  4: ZoneD
};
//
// Zone 'Off' buttons
// - Passive
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton_ZoneOffPassive[] = {
		// BUTTON																										         RECTANGLE						        TEXT
		//                Normal State,    HL, Persist time, 					    Style,	                            Tag,	 	 Color,   X,   Y,   W,   Y,  LW  	    X,   Y,  XO,  YO, Color,          	        FontSize,		   	  Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, 								  0,  {  {       0,   0,   0,   0,   0,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "" 	 } } },	 //  0: ZoneU (Not Used)
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_A_BTN,  {  {       0, 127,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  1: ZoneA
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_B_BTN,  {  {       0, 364,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  2: ZoneB
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_C_BTN,  {  {       0, 127, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  3: ZoneC
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_D_BTN,  {  {       0, 364, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  4: ZoneD
};
// - Active
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton_ZoneOffActive[] = {
		// BUTTON																									           RECTANGLE						      TEXT
		//             	 Normal State,    HL, Persist time, 					   Style,	                      	   Tag,    	   Color,   X,   Y,   W,   Y,  LW  	      X,   Y,  XO,  YO, Color,          		  FontSize,		  		Justification, Text
	{	UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, 								 0, {  {       0,   0,   0,   0,   0,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "" 		 } } },	 //  0: ZoneU (Not Used)
	{	UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_A_BTN, {  {       0, 127,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  1: ZoneA
	{	UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_B_BTN, {  {       0, 364,  95,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  2: ZoneB
	{	UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_C_BTN, {  {       0, 127, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  3: ZoneC
	{	UITFTDSP_BUTTON__STATE_ACTIVE, false,            0, UITFTDSP_BUTTON__STYLE_ROUND, ZONING_USERIF_NC7__TV__OFF_D_BTN, {  {       0, 364, 181,  83,  28,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Off" 	 } } },	 //  4: ZoneD
};
//
// General buttons
typedef enum	ZONING_USERIF_NC7__BUTTON_NUMBER {

	ZONING_USERIF_NC7__BN__DONE_BTN	= 0

} ZONING_USERIF_NC7__BUTTON_NUMBER;

static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton[] = {
		// BUTTON																									  				      		     		  RECTANGLE						        TEXT
		//                Normal State,    HL,                              Persist time, 					     Style,	               			       Tag,	     Color,   X,   Y,   W,   H,  LW  	    X,   Y,  XO,  YO, Color,          	   	    FontSize,		      Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,    ZONING_USERIF_NC7__TV__DONE_BTN, { {      0, 368, 235,  90,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Done" } } },	 //  0
} ;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Identifier
static const UITFTDSP_TEXT__TextDetails				mkaText_ZoneIdentifier[] = {
	//   X,   Y,  XO,  YO,                 Color,              	  		 Font,    	     Justification, String Label
	{    0,   0,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__LEFT, "" 	   		},   // 0 : ZoneU (Not used)
	{   30,  70,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__LEFT, "Zone A" 	},   // 1 : ZoneA
	{  267,  70,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__LEFT, "Zone B" 	},   // 2 : ZoneB
	{   30, 157,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__LEFT, "Zone C" 	},   // 3 : ZoneC
	{  267, 157,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__LEFT, "Zone D" 	},   // 4 : ZoneD
};
//
// Description
static const UITFTDSP_TEXT__TextDetails				mkaText_ZoneDescription[] = {
	//   X,   Y,  XO,  YO,                 Color,              	         Font,    	      Justification, String Label
	{    0,   0,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__RIGHT, "" 							 					  },   // 0 : ZoneU (Not used)
	{  207,  70,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__RIGHT, (char*)&maZone[TWBIFUTL__ZONE__A].strDescription[0]  },   // 1 : ZoneA
	{  444,  70,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__RIGHT, (char*)&maZone[TWBIFUTL__ZONE__B].strDescription[0]  },   // 2 : ZoneB
	{  207, 157,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__RIGHT, (char*)&maZone[TWBIFUTL__ZONE__C].strDescription[0]  },   // 3 : ZoneC
	{  444, 157,  -1,  -1, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__RIGHT, (char*)&maZone[TWBIFUTL__ZONE__D].strDescription[0]  },   // 4 : ZoneD
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct		ZONING_USERIF_NC7__UIDetails {									// Zoning UI Based Details
																					//
	GENTIMDT__TimingCounter	aOnButtonFlashShowTimingCounter[TWBIFUTL__MAXZONES];	// - When on pressed time to keep on before flashing
	GENTIMDT__TimingCounter	CancelAccessTimingCounter;								// - Timer to time how long to keep screen current with no user activity
																					//
} ZONING_USERIF_NC7__UIDetails;														//
																					//
static ZONING_USERIF_NC7__UIDetails	mZoning_NC7UI;									// Container
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneADescription = { (char*)&mkstrZoneDefaultDescription[0]/*&mkastrZoneDefaultDescription[TWBIFUTL__ZONE__A]*/, &maZone[TWBIFUTL__ZONE__A].Description };
const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneBDescription = { (char*)&mkstrZoneDefaultDescription[0]/*&mkastrZoneDefaultDescription[TWBIFUTL__ZONE__B]*/, &maZone[TWBIFUTL__ZONE__B].Description };
const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneCDescription = { (char*)&mkstrZoneDefaultDescription[0]/*&mkastrZoneDefaultDescription[TWBIFUTL__ZONE__C]*/, &maZone[TWBIFUTL__ZONE__C].Description };
const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneDDescription = { (char*)&mkstrZoneDefaultDescription[0]/*&mkastrZoneDefaultDescription[TWBIFUTL__ZONE__D]*/, &maZone[TWBIFUTL__ZONE__D].Description };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	ZONING_USERIF_NC7__kZoningUIZoneAOnButtonFlashShowTiming = { &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__A] };
const struct	GENTIMDT__Timing	ZONING_USERIF_NC7__kZoningUIZoneBOnButtonFlashShowTiming = { &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__B] };
const struct	GENTIMDT__Timing	ZONING_USERIF_NC7__kZoningUIZoneCOnButtonFlashShowTiming = { &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__C] };
const struct	GENTIMDT__Timing	ZONING_USERIF_NC7__kZoningUIZoneDOnButtonFlashShowTiming = { &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__D] };
const struct	GENTIMDT__Timing	ZONING_USERIF_NC7__kZoningUICancelAccessTiming 			 = { &mZoning_NC7UI.CancelAccessTimingCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void    					ShowZoneInformation(TWBIFUTL__ZONE vZone, ZONING_COMMON__ControlDetails* pZoning);
static bool						bTimeToCancelScreen(uint8_t vucTagValue);

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
 * @brief:	Initialises UI related data.
 * @param:	pZoning - Pointer to zoning control data.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ZONING_USERIF_NC7__Initialise(ZONING_COMMON__ControlDetails* pZoning)
{
	TWBIFUTL__ZONE	Zone;

	for ( Zone = TWBIFUTL__ZONE__A; Zone <= TWBIFUTL__ZONE__D; Zone++ )
	{
		// Text pointer
		maZone[Zone].Description.pstrText = &maZone[Zone].strDescription[0];
		// String init
		GENSTRDT__InitString( ZONING_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH, maZone[Zone].Description.pstrText, &maZone[Zone].Description );
		// User access
		pZoning->UserAccess.pDescription[Zone] = (GENSTRDT__String*)&maZone[Zone].Description;
		// Assume altered on start
		maZone[Zone].bDescriptionAltered = true;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns a pointer to the required zone description.
 * @param:	vZone - Zone to retrieve.
 * @retval:	Pointer to GENSTRDT__String.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
GENSTRDT__String*				ZONING_USERIF_NC7__pZoneDescription(TWBIFUTL__ZONE vZone)
{
	GENSTRDT__String*	pDescription = COM_DEFN__NULL;

	switch ( vZone )
	{
		case TWBIFUTL__ZONE__A:
		case TWBIFUTL__ZONE__B:
		case TWBIFUTL__ZONE__C:
		case TWBIFUTL__ZONE__D:
			pDescription = &maZone[vZone].Description;
			break;

		default:
			break;
	}

	return pDescription;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags zone name changed.
 * @param:	vZone - Zone name to flag as changed.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ZONING_USERIF_NC7__FlagZoneNameAltered(TWBIFUTL__ZONE vZone)
{
	switch ( vZone )
	{
		case TWBIFUTL__ZONE__A:
		case TWBIFUTL__ZONE__B:
		case TWBIFUTL__ZONE__C:
		case TWBIFUTL__ZONE__D:
			maZone[vZone].bDescriptionAltered = true;
			break;

		default:
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates if zone name has been altered.
 * @param:	vZone - Zone to retrieve.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							ZONING_USERIF_NC7__bZoneNameAltered(TWBIFUTL__ZONE vZone)
{
	bool		bAltered = false;

	switch ( vZone )
	{
		case TWBIFUTL__ZONE__A:
		case TWBIFUTL__ZONE__B:
		case TWBIFUTL__ZONE__C:
		case TWBIFUTL__ZONE__D:
			bAltered = maZone[vZone].bDescriptionAltered; maZone[vZone].bDescriptionAltered = false;
			break;

		default:
			break;
	}

	return bAltered;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to initialise data before screen access.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ZONING_USERIF_NC7__InitialiseBeforeFirstAccess(void)
{
	// Reset timeout timer
	GENTIMDT__StartTimer_TB_UIBased_1SEC( ZONING_USERIF_NC7__SCREEN_CANCEL_NO_ACTIVITY_TBUI1S, &mZoning_NC7UI.CancelAccessTimingCounter );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display update : Programming Zones (NC7).
 * @param:	pZoning	- Zoning information.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ZONING_USERIF_NC7__UpdateDisplay_ProgrammingZones(ZONING_COMMON__ControlDetails* pZoning)
{
	UICOMMON__TopStatusDetails	TopStatus;
    TWBIFUTL__ZONE				Zone;
    //
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[ZONING_USERIF_NC7__IN__HELP_BTN];
#endif
	TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[ZONING_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_ZoneProgram;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
	//
    // Zone details
    for ( Zone = TWBIFUTL__ZONE__A; Zone <= TWBIFUTL__ZONE__D; Zone++ )
    {
    	ShowZoneInformation( Zone, pZoning );
    }
    // Done button
    UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[ZONING_USERIF_NC7__BN__DONE_BTN] );

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Control update : Programming Zones.
 * @param:	pZoning	- Pointer to zoning information.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ZONING_USERIF_NC7__UpdateControl_ProgrammingZones(ZONING_COMMON__ControlDetails* pZoning)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;
    //
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }

	ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (ZONING_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
	    case ZONING_USERIF_NC7__TV__ON_A_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_A_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_A_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_A_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__ON_B_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_B_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_B_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_B_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__ON_C_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_C_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_C_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_C_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__ON_D_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_D_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_D_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_D_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__DONE_BTN:
		case ZONING_USERIF_NC7__TV__BACK_BTN:
  		case ZONING_USERIF_NC7__TV__HELP_BTN:
   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
			break;

		default:
        	break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( bTimeToCancelScreen(ucTagValue) )
	{
		pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__BACK;
	}
	else
	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )				// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (ZONING_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case ZONING_USERIF_NC7__TV__DONE_BTN:
					pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__DONE;
					break;

				case ZONING_USERIF_NC7__TV__BACK_BTN:
					pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__BACK;
					break;

		   		case ZONING_USERIF_NC7__TV__HELP_BTN:
		   			// TODO - Help screen
		   			break;

		   		default:
					break;
			}
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display update : Installed/enabled zones (NC7).
 * @param:	pZoning	- Pointer to zoning information.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ZONING_USERIF_NC7__UpdateDisplay_InstalledAndEnabledZones(ZONING_COMMON__ControlDetails* pZoning)
{
	UICOMMON__TopStatusDetails	TopStatus;
    TWBIFUTL__ZONE				Zone;
    //
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[ZONING_USERIF_NC7__IN__HELP_BTN];
#endif
	TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[ZONING_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_ZoneNormalOpn;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
    //
    // Zone details
    for ( Zone = TWBIFUTL__ZONE__A; Zone <= TWBIFUTL__ZONE__D; Zone++ )
    {
    	ShowZoneInformation( Zone, pZoning );
    }
    // Done button
    UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[ZONING_USERIF_NC7__BN__DONE_BTN] );

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Control update : Installed/enabled zones (NC7).
 * @param:	pZoning	- Pointer to zoning information.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							ZONING_USERIF_NC7__UpdateControl_InstalledAndEnabledZones(ZONING_COMMON__ControlDetails* pZoning)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;
    //
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }

	ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (ZONING_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
	    case ZONING_USERIF_NC7__TV__ON_A_BTN:
	    	// Start timer to keep on button highlighted before flashing
	    	GENTIMDT__StartTimer_TB_UIBased( ZONING_USERIF_NC7__ON_BUTTON_FLASH_SHOW_INTERVAL_TBUI, &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__A] );
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_A_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_A_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_A_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__ON_B_BTN:
	    	// Start timer to keep on button highlighted before flashing
	    	GENTIMDT__StartTimer_TB_UIBased( ZONING_USERIF_NC7__ON_BUTTON_FLASH_SHOW_INTERVAL_TBUI, &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__B] );
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_B_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_B_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_B_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__ON_C_BTN:
	    	// Start timer to keep on button highlighted before flashing
	    	GENTIMDT__StartTimer_TB_UIBased( ZONING_USERIF_NC7__ON_BUTTON_FLASH_SHOW_INTERVAL_TBUI, &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__C] );
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_C_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_C_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_C_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__ON_D_BTN:
	    	// Start timer to keep on button highlighted before flashing
	    	GENTIMDT__StartTimer_TB_UIBased( ZONING_USERIF_NC7__ON_BUTTON_FLASH_SHOW_INTERVAL_TBUI, &mZoning_NC7UI.aOnButtonFlashShowTimingCounter[TWBIFUTL__ZONE__D] );
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_D_ON;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__OFF_D_BTN:
		    pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__ZONE_D_OFF;
		    RapidUpdateControl_None();
		    break;

	    case ZONING_USERIF_NC7__TV__DONE_BTN:
	    case ZONING_USERIF_NC7__TV__BACK_BTN:
   		case ZONING_USERIF_NC7__TV__HELP_BTN:
   			sucActiveTag = ucTagValue; RapidUpdateControl_None();
			break;

        default:
        	break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( bTimeToCancelScreen(ucTagValue) )
	{
		pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__BACK;
	}
	else
	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )				// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (ZONING_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case ZONING_USERIF_NC7__TV__DONE_BTN:
					pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__DONE;
					break;

				case ZONING_USERIF_NC7__TV__BACK_BTN:
					pZoning->UserAccess.Control.ButtonPressed = ZONING_COMMON__CB__BACK;
					break;

		   		case ZONING_USERIF_NC7__TV__HELP_BTN:
		   			// TODO - Help screen
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
 * @brief:	Show zone information.
 * @param:	vZone	- Zone to update.
 * 			pZoning	- Pointer to zoning info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void    					ShowZoneInformation(TWBIFUTL__ZONE vZone, ZONING_COMMON__ControlDetails* pZoning)
{
	if ( !pZoning->UserAccess.bInstalled[vZone] ) { return; }
	//
	// Zone installed
	// - Outer rectangle
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaOuterRectangle[vZone] );
	// - Inner rectangle
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaInnerRectangle[vZone] );
	//
	// Zone Text info
	// - Identifier
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText_ZoneIdentifier[vZone] );
	// - Description
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText_ZoneDescription[vZone] );
	//
	// Control Buttons
	if ( !pZoning->UserAccess.bUserEnabled[vZone] )
	{
		// On passive/Off active
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_ZoneOnPassive[vZone] );
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_ZoneOffActive[vZone] );
	}
	else
	if ( !pZoning->UserAccess.bAutoEnabled[vZone] )
	{
		// On active & passive/Off passive
		if ( mZoning_NC7UI.aOnButtonFlashShowTimingCounter[vZone].ulValue != 0 )
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_ZoneOnActive[vZone] );
		}
		else
		{
			UITFTDSP__FlashButton( UITFTDSP__FR__1HZ, (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_ZoneOnActive[vZone] );
		}
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_ZoneOffPassive[vZone] );
	}
	else
	{
		// On active/Off passive
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_ZoneOnActive[vZone] );
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_ZoneOffPassive[vZone] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks if no user input has occured for timeout period.  If user input detected, timeout timer restarted.
 * @param:	vucTagValue	- Tag value pressed.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static bool					bTimeToCancelScreen(uint8_t vucTagValue)
{
	bool		bCancel = false;

	if ( vucTagValue != 0 )
	{
		// Touch detected so restart timeout timer
		GENTIMDT__StartTimer_TB_UIBased_1SEC( ZONING_USERIF_NC7__SCREEN_CANCEL_NO_ACTIVITY_TBUI1S, &mZoning_NC7UI.CancelAccessTimingCounter );
	}
	else
	if ( mZoning_NC7UI.CancelAccessTimingCounter.ulValue == 0 )
	{
		// Timeout - Cancel screen
		bCancel = true;
	}

	return bCancel;
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
