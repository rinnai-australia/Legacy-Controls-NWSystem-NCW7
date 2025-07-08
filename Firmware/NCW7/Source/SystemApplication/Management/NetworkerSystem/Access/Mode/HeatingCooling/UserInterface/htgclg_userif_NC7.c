//*********************************************************************************************************************************************************************************
// htgclg_userif_NC7.c
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
#include		<htgclg_common.h>
#include		<zoning_control.h>
#include		<htgclg_userif_NC7.h>
#include		<zoning_userif_NC7.h>
#include		<nws_svcinf_control.h>
#include		<servclog_heatcool.h>
#include		<uicommon.h>
#include		<uitftdsp_fonts.h>
#include		<nwsmspcl.h>
#include		<nwsutils.h>
#include		<twbif502.h>
#include		<tempmsif.h>
#include        <tftdspif.h>
#include		<genutils.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Top status text
static const char 	mkstrTopStatusText_ScheduleScreenHeatingMode[]  = "MENU : SCHEDULE - HEATING";
static const char 	mkstrTopStatusText_ScheduleScreenCoolingMode[]  = "MENU : SCHEDULE - COOLING";
// Pointer array holds the memory locations where the text strings of all zone identifiers are kept
static const char* 	mkastrZoneIdentifier[TWBIFUTL__MAXZONES] = { "", "Zone A  ", "Zone B  ", "Zone C  ", "Zone D  " };
// Pointer array holds the memory locations where the text strings of all time periods are kept
static const char*	mkastrTimePeriod[TWBIFUTL__MAX_SCHEDULE_TIME_PERIODS] = { "Wake", "Leave", "Return", "Pre-Sleep", "Sleep" };
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// String definition details - ** Array variables as text to display will vary so define here for pointer inclusion in text table **
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Time period text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			HTGCLG_USERIF_NC7__TIME_PERIOD_TEXT_LENGTH							9
// - Actual container for string
static char				mstrTimePeriodText[HTGCLG_USERIF_NC7__TIME_PERIOD_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mTimePeriodString;
//
// Zone description text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			HTGCLG_USERIF_NC7__ZONE_DESC_LENGTH									32
// - Actual container for string
static char				mstrZoneDescriptionText[HTGCLG_USERIF_NC7__ZONE_DESC_LENGTH+1];
// - String definition
static GENSTRDT__String	mZoneDescriptionString;
//
// Programming time period start time display texts
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			HTGCLG_USERIF_NC7__PROGRAMMING_TIME_PERIOD_START_TIME_TEXT_LENGTH	10
// - Actual container for string
static char 			mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__MAX_SCHEDULE_TIME_PERIODS][HTGCLG_USERIF_NC7__PROGRAMMING_TIME_PERIOD_START_TIME_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	maProgrammingTimePeriodStartTimeString[TWBIFUTL__MAX_SCHEDULE_TIME_PERIODS];
//
// Programming time period set temperature display texts
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			HTGCLG_USERIF_NC7__PROGRAMMING_TIME_PERIOD_SET_TEMP_TEXT_LENGTH		3
// - Actual container for string
static char 			mastrProgrammingTimePeriodSetTempText[TWBIFUTL__MAX_SCHEDULE_TIME_PERIODS][HTGCLG_USERIF_NC7__PROGRAMMING_TIME_PERIOD_SET_TEMP_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	maProgrammingTimePeriodSetTempString[TWBIFUTL__MAX_SCHEDULE_TIME_PERIODS];
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	HTGCLG_USERIF_NC7__IMAGE_NUMBER {

	HTGCLG_USERIF_NC7__IN__FAN_MENU_BTN_PASSIVE   				=  0,
	HTGCLG_USERIF_NC7__IN__FAN_MENU_BTN_ACTIVE    				=  1,
	HTGCLG_USERIF_NC7__IN__MENU_BTN_PASSIVE  	  				=  2,
	HTGCLG_USERIF_NC7__IN__HELP_BTN_PASSIVE       				=  3,
	HTGCLG_USERIF_NC7__IN__BACK_BTN_PASSIVE       				=  4,
	HTGCLG_USERIF_NC7__IN__ONOFF_BTN_PASSIVE      				=  5,
	HTGCLG_USERIF_NC7__IN__ONOFF_BTN_ACTIVE      				=  6,
	HTGCLG_USERIF_NC7__IN__MODE_BTN_PASSIVE       				=  7,
	HTGCLG_USERIF_NC7__IN__ZONE_BTN_PASSIVE       				=  8,
	HTGCLG_USERIF_NC7__IN__BRIVIS_LOGO           				=  9,
	HTGCLG_USERIF_NC7__IN__FAN_ICON               				= 10,
	HTGCLG_USERIF_NC7__IN__SCHEDULE_LEVEL_UP_BTN_PASSIVE        = 11,
	HTGCLG_USERIF_NC7__IN__SCHEDULE_LEVEL_DN_BTN_PASSIVE        = 12,
	HTGCLG_USERIF_NC7__IN__ALERT_ICON							= 13,

	HTGCLG_USERIF_NC7__IN__DF_FAN_MENU_BTN_PASSIVE   			= 14,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__IN__DF_FAN_MENU_BTN_ACTIVE    			= 15,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_PASSIVE 		  			= 16,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_ACTIVE    				= 17,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_PASSIVE   				= 18,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_ACTIVE    				= 19,		// NC7090_DFDISP

} HTGCLG_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails			mkaImage[] = {

	// 				  					 					      ImageNumber, 					                 ImageNumber_ActivePress,   X,   Y, 						Persistence time,   TAG
#if defined(INCLUDE_HELP_MENU_OPTION)
	{  						 UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_PASSIVE, 					   				UITFTDSP_IMAGE__NO_IMAGE, 343,   3, 									    0,	HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN	 			},
	{   					  UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_ACTIVE,  					   			UITFTDSP_IMAGE__NO_IMAGE, 343,   3,                                         0,	HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN	 			},
	{      						 UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_PASSIVE,  			 UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__MENU_BTN 				},
	{      						 UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, 			 	 UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__HELP_BTN 				},
	{      						 UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, 			 	 UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__BACK_BTN 				},
#else
	{  						 UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_PASSIVE, 					   				UITFTDSP_IMAGE__NO_IMAGE, 386,   3, 									    0,	HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN	 			},
	{   					  UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_ACTIVE,  					   			UITFTDSP_IMAGE__NO_IMAGE, 386,   3,                                         0,	HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN	 			},
	{      						 UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_PASSIVE,  			 UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__MENU_BTN 				},
	{      						 UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, 			 	 UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__HELP_BTN 				},
	{      						 UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, 			 	 UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__BACK_BTN 				},
#endif
	{ 	  						UITFTDSP_IMAGE__IN__COMMON__ONOFF_BTN_PASSIVE,     				   				UITFTDSP_IMAGE__NO_IMAGE,  15,  55, 									    0,	HTGCLG_USERIF_NC7__TV__ONOFF_BTN 				},
	{ 	    					 UITFTDSP_IMAGE__IN__COMMON__ONOFF_BTN_ACTIVE,     				   				UITFTDSP_IMAGE__NO_IMAGE,  15,  55, 									    0,	HTGCLG_USERIF_NC7__TV__ONOFF_BTN 				},
	{ 	   						 UITFTDSP_IMAGE__IN__COMMON__MODE_BTN_PASSIVE,   			 UITFTDSP_IMAGE__IN__COMMON__MODE_BTN_ACTIVE,  15, 124, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__MODE_BTN 				},
	{ 	   						 UITFTDSP_IMAGE__IN__COMMON__ZONE_BTN_PASSIVE,   			 UITFTDSP_IMAGE__IN__COMMON__ZONE_BTN_ACTIVE,  15, 197, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__ZONES_BTN 				},
#if defined(USE_BRIVIS_LOGO)
	{  								  UITFTDSP_IMAGE__IN__COMMON__BRIVIS_LOGO,         			  				UITFTDSP_IMAGE__NO_IMAGE, 438, 230, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,  -1												},
#else
	{  								  UITFTDSP_IMAGE__IN__COMMON__BRIVIS_LOGO,         			  				UITFTDSP_IMAGE__NO_IMAGE, 438, 242, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,  -1												},
#endif
	{ 			   						 UITFTDSP_IMAGE__IN__COMMON__FAN_ICON,            		   				UITFTDSP_IMAGE__NO_IMAGE, 425,  80, 									    0,	-1 												},
	{ 	  			UITFTDSP_IMAGE__IN__COMMON__SCHEDULE_LEVEL_UP_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__SCHEDULE_LEVEL_UP_BTN_ACTIVE, 220,  93, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEVEL_UP_BTN 	},
	{ 	  			UITFTDSP_IMAGE__IN__COMMON__SCHEDULE_LEVEL_UP_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__SCHEDULE_LEVEL_UP_BTN_ACTIVE, 220, 131, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEVEL_DN_BTN 	},
	{ 			   					   UITFTDSP_IMAGE__IN__COMMON__ALERT_ICON,            		   				UITFTDSP_IMAGE__NO_IMAGE, 440, 165, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__ALERT_BTN 				},
	//
	// Top status line of buttons when operating dual fuel, NC7090_DFDISP
	//
	{  					     UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_PASSIVE, 				   					UITFTDSP_IMAGE__NO_IMAGE, 300,   3, 									    0,	HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN	 			},
	{   				      UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_ACTIVE,  					   			UITFTDSP_IMAGE__NO_IMAGE, 300,   3,                                         0,	HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN	 			},
	{  					      UITFTDSP_IMAGE__IN__COMMON__DF_ELEC_BTN_PASSIVE, 				   					UITFTDSP_IMAGE__NO_IMAGE, 343,   3, 									    0,	HTGCLG_USERIF_NC7__TV__DF_ELEC_BTN	 			},
	{   				       UITFTDSP_IMAGE__IN__COMMON__DF_ELEC_BTN_ACTIVE,  								UITFTDSP_IMAGE__NO_IMAGE, 343,   3,                                         0,	HTGCLG_USERIF_NC7__TV__DF_ELEC_BTN	 			},
	{  					       UITFTDSP_IMAGE__IN__COMMON__DF_GAS_BTN_PASSIVE, 				   					UITFTDSP_IMAGE__NO_IMAGE, 386,   3, 									    0,	HTGCLG_USERIF_NC7__TV__DF_GAS_BTN	 			},
	{   				   	    UITFTDSP_IMAGE__IN__COMMON__DF_GAS_BTN_ACTIVE,  								UITFTDSP_IMAGE__NO_IMAGE, 386,   3,                                         0,	HTGCLG_USERIF_NC7__TV__DF_GAS_BTN	 			},
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	HTGCLG_USERIF_NC7__BUTTON_NUMBER {

	HTGCLG_USERIF_NC7__BN__MANUAL_PASSIVE_BTN							=  0,
	HTGCLG_USERIF_NC7__BN__MANUAL_ACTIVE_BTN							=  1,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_BTN									=  2,
	HTGCLG_USERIF_NC7__BN__CANCEL_BTN									=  3,

	HTGCLG_USERIF_NC7__BN__SCHEDULE_MONDAY_PASSIVE_BTN					=  4,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_MONDAY_ACTIVE_BTN					=  5,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_TUESDAY_PASSIVE_BTN					=  6,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_TUESDAY_ACTIVE_BTN					=  7,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WEDNESDAY_PASSIVE_BTN				=  8,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WEDNESDAY_ACTIVE_BTN				=  9,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_THURSDAY_PASSIVE_BTN				= 10,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_THURSDAY_ACTIVE_BTN					= 11,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_FRIDAY_PASSIVE_BTN					= 12,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_FRIDAY_ACTIVE_BTN					= 13,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SATURDAY_PASSIVE_BTN				= 14,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SATURDAY_ACTIVE_BTN					= 15,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SUNDAY_PASSIVE_BTN					= 16,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SUNDAY_ACTIVE_BTN					= 17,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKDAYS_PASSIVE_BTN				= 18,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKDAYS_ACTIVE_BTN					= 19,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKENDS_PASSIVE_BTN				= 20,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKENDS_ACTIVE_BTN					= 21,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WAKE_TIME_PASSIVE_BTN				= 22,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_WAKE_TIME_ACTIVE_BTN				= 23,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_LEAVE_TIME_PASSIVE_BTN				= 24,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_LEAVE_TIME_ACTIVE_BTN				= 25,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_RETURN_TIME_PASSIVE_BTN				= 26,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_RETURN_TIME_ACTIVE_BTN				= 27,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_PRESLEEP_TIME_PASSIVE_BTN			= 28,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_PRESLEEP_TIME_ACTIVE_BTN			= 29,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SLEEP_TIME_PASSIVE_BTN				= 30,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SLEEP_TIME_ACTIVE_BTN				= 31,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SLEEP_PRESLPOFF_TIME_PASSIVE_BTN	= 32,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_SLEEP_PRESLPOFF_TIME_ACTIVE_BTN		= 33,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_A_PASSIVE_BTN					= 34,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_A_ACTIVE_BTN					= 35,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_B_PASSIVE_BTN					= 36,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_B_ACTIVE_BTN					= 37,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_C_PASSIVE_BTN					= 38,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_C_ACTIVE_BTN					= 39,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_D_PASSIVE_BTN					= 40,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_D_ACTIVE_BTN					= 41,
	HTGCLG_USERIF_NC7__BN__SCHEDULE_RESTORE_DEFAULTS_BTN				= 42,

	HTGCLG_USERIF_NC7__BN__AUTO_PASSIVE_BTN								= 43,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__BN__AUTO_ACTIVE_BTN								= 44,		// NC7090_DFDISP

} HTGCLG_USERIF_NC7__BUTTON_NUMBER;

#define		HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET	 (HTGCLG_USERIF_NC7__BN__SCHEDULE_LEAVE_TIME_PASSIVE_BTN - HTGCLG_USERIF_NC7__BN__SCHEDULE_WAKE_TIME_PASSIVE_BTN)
//
// Array of images
static const UITFTDSP_BUTTON__ButtonDefinition				mkaButton[] = {

		// BUTTON																																	  	  				 		 RECTANGLE							    TEXT
		//                Normal State,    HL,         	  		  	        Persist time, 					      Style,	                               			   Tag,		     Color,   X,   Y,   W,   H,  LW  	    X,   Y,  XO,  YO, Color,          		    FontSize,		      Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0,  UITFTDSP_BUTTON__STYLE_ROUND,   				 HTGCLG_USERIF_NC7__TV__MANUAL_BTN,   {  {       0,  99, 218,  80,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Manual" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false,										   0,  UITFTDSP_BUTTON__STYLE_ROUND,   				 HTGCLG_USERIF_NC7__TV__MANUAL_BTN,   {  {       0,  99, 218,  80,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Manual" 																										} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,  UITFTDSP_BUTTON__STYLE_ROUND, 			   HTGCLG_USERIF_NC7__TV__SCHEDULE_BTN,   {  {       0, 188, 218,  80,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Schedule" 																									} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,  UITFTDSP_BUTTON__STYLE_ROUND,   				 HTGCLG_USERIF_NC7__TV__CANCEL_BTN,	  {  {       0, 339, 218,  80,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Cancel" 																										} } },

		// - Days
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_MONDAY_BTN,	  {  {       0,  15,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Mon" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_MONDAY_BTN,	  {  {       0,  15,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Mon" 																										} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	   HTGCLG_USERIF_NC7__TV__SCHEDULE_TUESDAY_BTN,	  {  {       0,  51,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Tue" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									  	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	   HTGCLG_USERIF_NC7__TV__SCHEDULE_TUESDAY_BTN,	  {  {       0,  51,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Tue" 																										} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	 HTGCLG_USERIF_NC7__TV__SCHEDULE_WEDNESDAY_BTN,	  {  {       0,  87,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Wed" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	 HTGCLG_USERIF_NC7__TV__SCHEDULE_WEDNESDAY_BTN,	  {  {       0,  87,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Wed" 																										} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	  HTGCLG_USERIF_NC7__TV__SCHEDULE_THURSDAY_BTN,	  {  {       0, 123,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Thu" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,      HTGCLG_USERIF_NC7__TV__SCHEDULE_THURSDAY_BTN,	  {  {       0, 123,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Thu" 																										} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_FRIDAY_BTN,	  {  {       0, 159,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Fri" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_FRIDAY_BTN,	  {  {       0, 159,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Fri" 																										} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	  HTGCLG_USERIF_NC7__TV__SCHEDULE_SATURDAY_BTN,	  {  {       0, 195,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Sat" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	  HTGCLG_USERIF_NC7__TV__SCHEDULE_SATURDAY_BTN,	  {  {       0, 195,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Sat" 																										} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_SUNDAY_BTN,	  {  {       0, 231,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Sun" 																										} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_SUNDAY_BTN,	  {  {       0, 231,  58,  37,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Sun" 																										} } },
		// - Weekdays & Weekends
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,  	  HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKDAYS_BTN,	  {  {       0,  10,  58, 130,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Weekdays" 																									} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	  HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKDAYS_BTN,	  {  {       0,  10,  58, 130,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Weekdays" 																	} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 										   0, UITFTDSP_BUTTON__STYLE_SQUARE,  	  HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKENDS_BTN,	  {  {       0, 139,  58, 130,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Weekends" 																	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	  HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKENDS_BTN,	  {  {       0, 139,  58, 130,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Weekends" 																	} } },
		// - Time periods
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	 HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TIME_BTN,	  {  {       0,  80,  94,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__WAKE][0]  		} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,	 HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TIME_BTN,	  {  {       0,  80,  94,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__WAKE][0]  		} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 										   0, UITFTDSP_BUTTON__STYLE_SQUARE,  	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TIME_BTN,	  {  {       0,  80, 126,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__LEAVE][0] 		} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TIME_BTN,	  {  {       0,  80, 126,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__LEAVE][0] 		} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TIME_BTN,	  {  {       0,  80, 158,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__RETURN][0] 		} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 										   0, UITFTDSP_BUTTON__STYLE_SQUARE,   HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TIME_BTN,	  {  {       0,  80, 158,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__RETURN][0] 		} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 										   0, UITFTDSP_BUTTON__STYLE_SQUARE, HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TIME_BTN,	  {  {       0,  80, 190,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__PRESLEEP][0] 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 										   0, UITFTDSP_BUTTON__STYLE_SQUARE, HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TIME_BTN,	  {  {       0,  80, 190,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__PRESLEEP][0] 	} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									       0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TIME_BTN,	  {  {       0,  80, 222,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__SLEEP][0] 		} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TIME_BTN,	  {  {       0,  80, 222,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__SLEEP][0] 		} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TIME_BTN,	  {  {       0,  80, 190,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__SLEEP][0] 		} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TIME_BTN,	  {  {       0,  80, 190,  69,  32,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodStartTimeText[TWBIFUTL__STP__SLEEP][0] 		} } },
		// - Zones
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_A_BTN,	  {  {       0, 277,  94, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0] 											} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_A_BTN,	  {  {       0, 277,  94, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0] 											} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_B_BTN,	  {  {       0, 277, 126, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0] 											} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_B_BTN,	  {  {       0, 277, 126, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0] 											} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_C_BTN,	  {  {       0, 277, 158, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0]												} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_C_BTN,	  {  {       0, 277, 158, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0] 											} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_D_BTN,	  {  {       0, 277, 190, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0] 											} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 									 	   0, UITFTDSP_BUTTON__STYLE_SQUARE,   		HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_D_BTN,	  {  {       0, 277, 190, 192,  32,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0]												} } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,  UITFTDSP_BUTTON__STYLE_ROUND, HTGCLG_USERIF_NC7__TV__SCHEDULE_RESTORE_DFLTS_BTN,	  {  {       0, 277, 235, 192,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Restore Defaults" 															} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, 									 	   0,  UITFTDSP_BUTTON__STYLE_ROUND,   				HTGCLG_USERIF_NC7__TV__DF_AUTO_BTN,   {  {       0, 363,  55,  60,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "AUTO" 																		} } },	// NC7090_DFDISP
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false,										   0,  UITFTDSP_BUTTON__STYLE_ROUND,   				HTGCLG_USERIF_NC7__TV__DF_AUTO_BTN,   {  {       0, 363,  55,  60,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "AUTO" 																		} } },	// NC7090_DFDISP
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Set Temperature Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaSetTemperatureButton[]
typedef enum	HTGCLG_USERIF_NC7__SET_TEMPERATURE_BUTTON_NUMBER {

	// Heating
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_HEATING_PASSIVE_BTN				=  0,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_HEATING_ACTIVE_BTN				=  1,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_LEAVE_TEMP_HEATING_PASSIVE_BTN				=  2,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_LEAVE_TEMP_HEATING_ACTIVE_BTN				=  3,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_RETURN_TEMP_HEATING_PASSIVE_BTN			=  4,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_RETURN_TEMP_HEATING_ACTIVE_BTN				=  5,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_PRESLEEP_TEMP_HEATING_PASSIVE_BTN			=  6,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_PRESLEEP_TEMP_HEATING_ACTIVE_BTN			=  7,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_TEMP_HEATING_PASSIVE_BTN				=  8,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_TEMP_HEATING_ACTIVE_BTN				=  9,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_HEATING_PASSIVE_BTN	= 10,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_HEATING_ACTIVE_BTN	= 11,
	// Cooling
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_COOLING_PASSIVE_BTN				= 12,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_COOLING_ACTIVE_BTN				= 13,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_LEAVE_TEMP_COOLING_PASSIVE_BTN				= 14,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_LEAVE_TEMP_COOLING_ACTIVE_BTN				= 15,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_RETURN_TEMP_COOLING_PASSIVE_BTN			= 16,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_RETURN_TEMP_COOLING_ACTIVE_BTN				= 17,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_PRESLEEP_TEMP_COOLING_PASSIVE_BTN			= 18,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_PRESLEEP_TEMP_COOLING_ACTIVE_BTN			= 19,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_TEMP_COOLING_PASSIVE_BTN				= 20,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_TEMP_COOLING_ACTIVE_BTN				= 21,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_COOLING_PASSIVE_BTN	= 22,
	HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_COOLING_ACTIVE_BTN	= 23

} HTGCLG_USERIF_NC7__SET_TEMPERATURE_BUTTON_NUMBER;
//
// Array of Set Temperature Buttons
//   BUTTON										      RECTANGLE					 TEXT														 POINT
// { { Normal State, HL, Persist time, Style, Tag,	{ { Color, X, Y, W, H, LW }. { X, Y, XO, YO, Color, FontSize, Justification, Text } } }, { Color, X, Y, XO1, YO1, XO2, YO2, PointSize } }
static const UITFTDSP_BUTTON__TemperatureButtonDefinition	mkaSetTemperatureButton[] = {

	// Heating Set Temperature Buttons
	{ { UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,     HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_HEATING, 149,  94,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__WAKE][0]     } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,     HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149,  94,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__WAKE][0]     } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ { UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,    HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_HEATING, 149, 126,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__LEAVE][0]    } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,    HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 126,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__LEAVE][0]    } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ { UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_HEATING, 149, 158,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__RETURN][0]   } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 158,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__RETURN][0]   } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ { UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE, HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_HEATING, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__PRESLEEP][0] } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE, HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__PRESLEEP][0] } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,  	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_HEATING, 149, 222,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 222,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_HEATING, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	// Cooling Set Temperature Buttons
	{ {	UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,  	 HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_COOLING, 149,  94,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__WAKE][0]  	 } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	 HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149,  94,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__WAKE][0]  	 } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_COOLING, 149, 126,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__LEAVE][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 126,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__LEAVE][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_COOLING, 149, 158,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__RETURN][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 158,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__RETURN][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE, HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_COOLING, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__PRESLEEP][0] } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	 UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE, HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__PRESLEEP][0] } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {	UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_COOLING, 149, 222,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 222,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } }, { 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ { UITFTDSP_BUTTON__STATE_PASSIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_TEMP_COOLING, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } },
	{ {  UITFTDSP_BUTTON__STATE_ACTIVE, false, 0, UITFTDSP_BUTTON__STYLE_SQUARE,   	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN, { { UITFTDSP_COLOR__BUTTON_INNER_ACTIVE, 149, 190,  46,  32,   0 }, { -1, -1, -1, -1,  UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, (char*)&mastrProgrammingTimePeriodSetTempText[TWBIFUTL__STP__SLEEP][0] 	 } } },	{ 0,  -1,  -1,  31,  12,  35,  12, 2 } }
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button With Two Texts Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaTwoTextsButton[]
typedef enum	HTGCLG_USERIF_NC7__TWO_TEXTS_BUTTON_NUMBER {

	HTGCLG_USERIF_NC7__TTBN__CURRENT_PERIOD_ACTIVE_BTN	=  0,
	HTGCLG_USERIF_NC7__TTBN__ADVANCE_PERIOD_ACTIVE_BTN	=  1

} HTGCLG_USERIF_NC7__TWO_TEXTS_BUTTON_NUMBER;
//
// Array of Two Texts Buttons
static const UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition	mkaTwoTextsButton[] = {
		// BUTTON																																	  	  	 		 // RECTANGLE						   	// TEXT1																											 // TEXT2
		//               Normal State,    HL,         	  		  		   Persist time, 						Style,	                             		 Tag, 	 //  Color,   X,   Y,   W,   H,  LW  	    X,   Y,  XO,  YO, Color,          		    FontSize,		    Justification,  Text							     X,   Y, XO,  YO, Color,          		    FontSize,		 	   Justification,  Text
	{	UITFTDSP_BUTTON__STATE_ACTIVE,	true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, HTGCLG_USERIF_NC7__TV__ADVANCE_PERIOD_BTN, {  {       0, 188, 218, 142,  26,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__LEFT,  (char*)&mstrTimePeriodText[0] }, {  -1,  -1,  5,  -1,     0,  UITFTDSP_FONTS__FS__BOLD_2, 	UITFTDSP_TEXT__TJ__RIGHT,  ">>" 						 } } },
	{	UITFTDSP_BUTTON__STATE_ACTIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, HTGCLG_USERIF_NC7__TV__ADVANCE_PERIOD_BTN, {  {       0, 188, 218, 142,  26,  0  },  {  -1,  -1,   5,  -1,     0,  UITFTDSP_FONTS__FS__BOLD_2, UITFTDSP_TEXT__TJ__LEFT,  "<<" 					 	  }, {  -1,  -1, 30,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, 	 UITFTDSP_TEXT__TJ__LEFT,  (char*)&mstrTimePeriodText[0] } } }
};

// Macro definition to simplify function calls : X = HTGCLG_USERIF_NC7__TWO_TEXTS_BUTTON_NUMBER type which indexes array
#define HTGCLG_USERIF_NC7__TwoTextsButton(X)	((UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[X])
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaText[]
typedef enum	HTGCLG_USERIF_NC7__TEXT_NUMBER {

	HTGCLG_USERIF_NC7__TN__CIRCFAN			=  0,
	HTGCLG_USERIF_NC7__TN__ZONEDESC			=  1,
	HTGCLG_USERIF_NC7__TN__ATTENTION		=  2,
	HTGCLG_USERIF_NC7__TN__WAKE				=  3,
	HTGCLG_USERIF_NC7__TN__LEAVE			=  4,
	HTGCLG_USERIF_NC7__TN__RETURN			=  5,
	HTGCLG_USERIF_NC7__TN__PRESLEEP			=  6,
	HTGCLG_USERIF_NC7__TN__SLEEP			=  7,
	HTGCLG_USERIF_NC7__TN__SLEEP_PRESLPOFF	=  8,
	HTGCLG_USERIF_NC7__TN__ZONEDESC_PROG	=  9

} HTGCLG_USERIF_NC7__TEXT_NUMBER;
//
// Array of Texts
static const UITFTDSP_TEXT__TextDetails						mkaText[] = {
	// 	X, 	 Y,  XO,  YO, 			      Color, 		 	   	      Font Size, 	   	     Justification, String Label
	{ 412, 118,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, "CIRC FAN" 							},
	{ 100,  56,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0]	},
	{ 454, 200,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Alert"    							},

	{  75, 110,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "Wake:"    							},
	{  75, 142,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "Leave:"    						},
	{  75, 174,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "Return:"    						},
	{  75, 206,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "Pre-Sleep:"    					},
	{  75, 238,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "Sleep:"    						},
	{  75, 206,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "Sleep:"    						},

	{ 280,  62,  -1,  -1, UITFTDSP_COLOR__WHITE, 	UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrZoneDescriptionText[0]	}
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Bar Graph Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaBarGraph[]
typedef enum	HTGCLG_USERIF_NC7__BARGRAPH_NUMBER {

	HTGCLG_USERIF_NC7__BG__FAN_SPEED =  0

} HTGCLG_USERIF_NC7__BARGRAPH_NUMBER;
//
// Array of Bargraphs
static const UI_TFTSHAPE__BarGraphDetails					mkaBarGraph[] = {
				// RECTANGLE
	// FS Type, //	   				     Color,   X,   Y,   W,   H,  LW
	{	  true,	{ UITFTDSP_COLOR__BUTTON_OUTER, 264,  77,  78, 129,  18 } }
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Level Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaLevelButtons[]
typedef enum	HTGCLG_USERIF_NC7__LEVEL_BUTTON_NUMBER {

	HTGCLG_USERIF_NC7__LB__FAN_SPEED_BTN     =  0

} HTGCLG_USERIF_NC7__LEVEL_BUTTON_NUMBER;
//
// Array of Level Buttons
static const UITFTDSP_BUTTON__LevelButtonDetails			mkaLevelButtons[] = {

	{
		// RECTANGLE						   	  				     // TEXT1																									  				 // TEXT2
		// 	   					   Color,   X,   Y,   W,   H,  LW 		 X,   Y,  XO,  YO, 						   		 Color,          		   FontSize,		  	 Justification, Text		 X,   Y,  XO,  YO, Color,          			 FontSize,		  	  Justification, Text
		{ { UITFTDSP_COLOR__BUTTON_OUTER, 349,  77,  56, 129,  18 }, {  -1,  -1,  -1,  -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Fan" }, {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__RIGHT, COM_DEFN__STRING_NULL	} },
		// TOP BUTTON
		// 				  				   ImageNumber, 					   ImageNumber_ActivePress,   X,   Y, 						   Persistence time,    TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__LEVEL_UP_BTN		},
		// BOTTOM BUTTON
		// 				  				   ImageNumber, 					   ImageNumber_ActivePress,   X,   Y, 						   Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	HTGCLG_USERIF_NC7__TV__LEVEL_DN_BTN 	},
		// POINT
		// Color,	X,	 Y,	XO1, YO1, XO2, YO2, PointSize
		{ 	   0,  -1,  -1,  -1,  -1,  -1,  -1, 0 		}
	}
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle with text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaRectangle[]
typedef enum	HTGCLG_USERIF_NC7__RECTANGLE_NUMBER {

	HTGCLG_USERIF_NC7__RN__ZONE_BLANK = 0

} HTGCLG_USERIF_NC7__RECTANGLE_NUMBER;
//
// Array of Rectangles
static const UI_TFTSHAPE__Rectangle							mkaRectangle[] = {
	// Rectangle
	// 			                      Color,    X,   Y,   W,   H,  LW
	{  UITFTDSP_COLOR__BUTTON_INNER_PASSIVE,   25, 240,  41,  11,  4 },
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pointer to fan menu image specific defined info
static UI_TFTIMAGE__ImageAppSpecificDetails*	mpFanMenuImage  = 0;
static UI_TFTIMAGE__ImageAppSpecificDetails*	mpGasMenuImage  = 0;	// Gas button in top status to use,  NC7090_DFDISP
static UI_TFTIMAGE__ImageAppSpecificDetails*	mpElecMenuImage = 0;	// Elec button in top status to use, NC7090_DFDISP
// Flag indicates temporary override state
static bool										mbTemporaryOverrideEnabled;
// Service info details
static NWS_SVCINF_COMMON__ServiceInfoDetails	mServiceInfo;
// Flag indicates whether or not the schedule access enabled by this networker, jason 10-12-15 
static bool										mbScheduleAccessEnabledByThisNetworker;
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
static void								UpdateDisplay_SetupBottomStatus(HTGCLG_COMMON__ControlDetails* pHeatingCooling);
static void								ShowTemperatureWithAssociatedButton(bool vbAllowUserAccess, uint8_t vucTemperature, UITFTDSP_BUTTON__TemperatureButtonDefinition* pTemperatureButton, UITFTDSP_BUTTON__LevelButtonDetails* pLevelButtons, GENSTRDT__String* pTemperature);
static void								UpdateDisplayWithZoneName(bool vbScheduleScreen, HTGCLG_COMMON__ControlDetails* pHeatingCooling);
static void								UpdateServiceDetailsStatus(HTGCLG_COMMON__ControlDetails* pHeatingCooling);
static void								UpdateDisplay_ElecGasImageButtons(void);
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
 * @brief:	Module initialisation.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__Initialise(void)
{
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Pointer to service details used for N-C7.
 * @param:	None.
 * @retval:	NWS_SVCINF_COMMON__ServiceInfoDetails*.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
NWS_SVCINF_COMMON__ServiceInfoDetails*	HTGCLG_USERIF_NC7__pServiceInfoDetails(void)
{
	return &mServiceInfo;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks if service prompt screen is being displayed flagging when reset or clear buttons have been pressed.
 * @param:	pHeatingCooling	- Pointer to control structure.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool									HTGCLG_USERIF_NC7__bViewingServiceInfoPrompt(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	bool		bViewingServiceInfo = false;

	if ( NWS_SVCINF_CONTROL__ServiceInfoPrompt_bShowIt() )
	{
		NWS_SVCINF_CONTROL__UserifUpdate();

		if ( NWS_SVCINF_CONTROL__ServiceInfoPrompt_bResetFlagged() )
		{
			pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__RESET_APPLIANCE;
		}

		if ( NWS_SVCINF_CONTROL__ServiceInfoPrompt_bClearFlagged() )
		{
			pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__CLEAR_SERVICE_LOG;
		}

		bViewingServiceInfo = true;
	}

	return bViewingServiceInfo;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User display update - N-C7 : Programming Mode.
 * @param:	pHeatingCooling	- Pointer to the heating/cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateDisplay_Mode_Programming(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	UICOMMON__TopStatusDetails							TopStatus;
	char*												pstrTopStatusText;

	RTMCLKIF__DAY_OF_WEEK								Day;
	HTGCLG_USERIF_NC7__SET_TEMPERATURE_BUTTON_NUMBER	ActiveWakeSetTemp, PassiveWakeSetTemp, ActiveSleepSetTempWhenPreSlpOptDisabled, PassiveSleepSetTempWhenPreSlpOptDisabled;
	TWBIFUTL__SCHEDULE_TIME_PERIOD						TimePeriod;

	GENSTRDT__String*									pZoneDescription;
	TWBIFUTL__ZONE										Zone;
	//
	// Update operation mode specific display info
	if ( NWSUTILS__bClimateControlModeIsHeating() )
	{
		// Heating mode
		// - Top status text
		pstrTopStatusText 							= (char*)&mkstrTopStatusText_ScheduleScreenHeatingMode[0];
		// - Wake time period set temperature button numbers
		ActiveWakeSetTemp							= HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_HEATING_ACTIVE_BTN;
		PassiveWakeSetTemp							= HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_HEATING_PASSIVE_BTN;
		// - Sleep time period set temperature button numbers: when Pre-Sleep option disabled
		ActiveSleepSetTempWhenPreSlpOptDisabled		= HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_HEATING_ACTIVE_BTN;
		PassiveSleepSetTempWhenPreSlpOptDisabled	= HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_HEATING_PASSIVE_BTN;
	}
	else
	{
		// Cooling mode
		// - Top status text
		pstrTopStatusText 							= (char*)&mkstrTopStatusText_ScheduleScreenCoolingMode[0];
		// - Wake time period set temperature button numbers
		ActiveWakeSetTemp							= HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_COOLING_ACTIVE_BTN;
		PassiveWakeSetTemp							= HTGCLG_USERIF_NC7__TBN__SCHEDULE_WAKE_TEMP_COOLING_PASSIVE_BTN;
		// - Sleep time period set temperature button numbers: when Pre-Sleep option disabled
		ActiveSleepSetTempWhenPreSlpOptDisabled		= HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_COOLING_ACTIVE_BTN;
		PassiveSleepSetTempWhenPreSlpOptDisabled	= HTGCLG_USERIF_NC7__TBN__SCHEDULE_SLEEP_PRESLPOFF_TEMP_COOLING_PASSIVE_BTN;
	}
	//
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
	TopStatus.pstrTextToDisplay = pstrTopStatusText;
	TopStatus.pImage2 			= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__BACK_BTN_PASSIVE];
#if defined(INCLUDE_HELP_MENU_OPTION)
	TopStatus.pImage3 			= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__HELP_BTN_PASSIVE];
#endif
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
	//
	// Day grouping display
    switch ( pHeatingCooling->UserAccess.DayGrouping )
    {
		case NWSUTILS__DG__INDIVIDUAL_DAYS:
			// Programming by individual days
			for ( Day = RTMCLKIF__DOW__MON; Day <= RTMCLKIF__DOW__SUN; Day++ )
			{
				if ( Day == pHeatingCooling->UserAccess.CurrentDaySelected )
				{
					// Show selected day
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_MONDAY_ACTIVE_BTN + Day * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				}
				else
				{
					// Show unselected day
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_MONDAY_PASSIVE_BTN + Day * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				}
			}
			break;

		case NWSUTILS__DG__WEEK_AND_WEEKEND_DAYS:
			// Programming by Mon-Fri/Sat-Sun groups -> Show current group!
			if ( pHeatingCooling->UserAccess.bWeekdaysSelected )
			{
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKDAYS_ACTIVE_BTN]  );
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKENDS_PASSIVE_BTN] );
			}
			else
			{
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKDAYS_PASSIVE_BTN] );
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WEEKENDS_ACTIVE_BTN]  );
			}
			break;

		default:
			// Programming by Mon-Sun groups -> Show all days!
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_MONDAY_ACTIVE_BTN]    );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_TUESDAY_ACTIVE_BTN]   );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WEDNESDAY_ACTIVE_BTN] );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_THURSDAY_ACTIVE_BTN]  );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_FRIDAY_ACTIVE_BTN]    );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_SATURDAY_ACTIVE_BTN]  );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_SUNDAY_ACTIVE_BTN]    );
			break;
	}
    //
    // Time period display
	for ( TimePeriod = TWBIFUTL__STP__WAKE; TimePeriod <= TWBIFUTL__STP__SLEEP; TimePeriod++ )
	{
		// Check if Pre-Sleep time period option enabled
		if ( !HTGCLG_COMMON__bSchedulePreSleepTimePeriodOptionEnabled() )
		{
			if ( TimePeriod == TWBIFUTL__STP__PRESLEEP )
			{
				// Skip Pre-Sleep time period
				TimePeriod = TWBIFUTL__STP__SLEEP;
			}
		}
	    //
	    // Start time
	    // - Init string
	    GENSTRDT__InitString( HTGCLG_USERIF_NC7__PROGRAMMING_TIME_PERIOD_START_TIME_TEXT_LENGTH, &mastrProgrammingTimePeriodStartTimeText[TimePeriod][0], &maProgrammingTimePeriodStartTimeString[TimePeriod] );
	    // - Time to display if allowed
	    if ( pHeatingCooling->UserAccess.bAllow )
	    {
		    UICOMMON__AssignTimeToString( TWBIFUTL__bClockDisplayFormatIn12HourFormat(),
		    							  pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].StartTime.ucHour,
		    							  pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].StartTime.ucMinute,
		    							  &maProgrammingTimePeriodStartTimeString[TimePeriod] );
	    }
	    else
	    {
	    	GENSTRDT__AssignTextToString( "??:??", &maProgrammingTimePeriodStartTimeString[TimePeriod] );
	    }
	    //
		// Set temperature
	    // - Init string
	    GENSTRDT__InitString( HTGCLG_USERIF_NC7__PROGRAMMING_TIME_PERIOD_SET_TEMP_TEXT_LENGTH, &mastrProgrammingTimePeriodSetTempText[TimePeriod][0], &maProgrammingTimePeriodSetTempString[TimePeriod] );
	    //
	    // Show start time and set temperature buttons
		if ( TimePeriod == pHeatingCooling->UserAccess.CurrentTimePeriodSelected )
		{
			// Current selected time period
			if ( pHeatingCooling->UserAccess.bAlterTime )
			{
				// Alter start time
				if ( ( !HTGCLG_COMMON__bSchedulePreSleepTimePeriodOptionEnabled() ) && ( TimePeriod == TWBIFUTL__STP__SLEEP ) )
				{
					// Show Sleep time period when Pre-Sleep option not enabled
					// - Show active start time
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_SLEEP_PRESLPOFF_TIME_ACTIVE_BTN] );
					// - Show passive set temperature
					ShowTemperatureWithAssociatedButton( pHeatingCooling->UserAccess.bAllow,
														 pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].ucSetTemperature,
														 (UITFTDSP_BUTTON__TemperatureButtonDefinition*)&mkaSetTemperatureButton[PassiveSleepSetTempWhenPreSlpOptDisabled],
														 0,
														 &maProgrammingTimePeriodSetTempString[TimePeriod] );
				}
				else
				{
					// - Show active start time
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WAKE_TIME_ACTIVE_BTN + TimePeriod * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
					// - Show passive set temperature
					ShowTemperatureWithAssociatedButton( pHeatingCooling->UserAccess.bAllow,
														 pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].ucSetTemperature,
														 (UITFTDSP_BUTTON__TemperatureButtonDefinition*)&mkaSetTemperatureButton[PassiveWakeSetTemp + TimePeriod * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET],
														 0,
														 &maProgrammingTimePeriodSetTempString[TimePeriod] );
				}
			}
			else
			{
				// Alter set temperature
				if ( ( !HTGCLG_COMMON__bSchedulePreSleepTimePeriodOptionEnabled() ) && ( TimePeriod == TWBIFUTL__STP__SLEEP ) )
				{
					// Show Sleep time period when Pre-Sleep option not enabled
					// - Show active set temperature
					ShowTemperatureWithAssociatedButton( pHeatingCooling->UserAccess.bAllow,
														 pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].ucSetTemperature,
														 (UITFTDSP_BUTTON__TemperatureButtonDefinition*)&mkaSetTemperatureButton[ActiveSleepSetTempWhenPreSlpOptDisabled],
														 0,
														 &maProgrammingTimePeriodSetTempString[TimePeriod] );
					// - Show passive start time
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_SLEEP_PRESLPOFF_TIME_PASSIVE_BTN] );
				}
				else
				{
					// - Show active set temperature
					ShowTemperatureWithAssociatedButton( pHeatingCooling->UserAccess.bAllow,
														 pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].ucSetTemperature,
														 (UITFTDSP_BUTTON__TemperatureButtonDefinition*)&mkaSetTemperatureButton[ActiveWakeSetTemp + TimePeriod * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET],
														 0,
														 &maProgrammingTimePeriodSetTempString[TimePeriod] );
					// - Show passive start time
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WAKE_TIME_PASSIVE_BTN + TimePeriod * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				}
			}
		}
		else
		{
			// Unselected time period
			if ( ( !HTGCLG_COMMON__bSchedulePreSleepTimePeriodOptionEnabled() ) && ( TimePeriod == TWBIFUTL__STP__SLEEP ) )
			{
				// Show Sleep time period when Pre-Sleep option not enabled
				// - Show passive start time
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_SLEEP_PRESLPOFF_TIME_PASSIVE_BTN] );
				// - Show passive set temperature
				ShowTemperatureWithAssociatedButton( pHeatingCooling->UserAccess.bAllow,
													 pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].ucSetTemperature,
													 (UITFTDSP_BUTTON__TemperatureButtonDefinition*)&mkaSetTemperatureButton[PassiveSleepSetTempWhenPreSlpOptDisabled],
													 0,
													 &maProgrammingTimePeriodSetTempString[TimePeriod] );
			}
			else
			{
				// - Show passive start time
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_WAKE_TIME_PASSIVE_BTN + TimePeriod * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				// - Show passive set temperature
				ShowTemperatureWithAssociatedButton( pHeatingCooling->UserAccess.bAllow,
													 pHeatingCooling->UserAccess.aSelectedDaySchedule[TimePeriod].ucSetTemperature,
													 (UITFTDSP_BUTTON__TemperatureButtonDefinition*)&mkaSetTemperatureButton[PassiveWakeSetTemp + TimePeriod * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET],
													 0,
													 &maProgrammingTimePeriodSetTempString[TimePeriod] );
			}
		}
		//
		// Show time period text
		if ( ( !HTGCLG_COMMON__bSchedulePreSleepTimePeriodOptionEnabled() ) && ( TimePeriod == TWBIFUTL__STP__SLEEP ) )
		{
			// Show Sleep time period when Pre-Sleep option not enabled
			UITFTDSP_TEXT__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__SLEEP_PRESLPOFF] );
		}
		else
		{
			UITFTDSP_TEXT__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__WAKE + TimePeriod] );
		}
	}
	//
	// Manually controlled zones
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		// Update zone display
		for ( Zone = TWBIFUTL__ZONE__A; Zone <= TWBIFUTL__ZONE__D; Zone++ )
		{
			// Only show installed zone
			if ( ZONING_CONTROL__bZoneIsInstalled( Zone ) )
			{
				// - Init container
				GENSTRDT__InitString( HTGCLG_USERIF_NC7__ZONE_DESC_LENGTH, (char*)&mstrZoneDescriptionText[0], &mZoneDescriptionString );
				// - Add zone identifier
				GENSTRDT__AssignTextToString( (char*)mkastrZoneIdentifier[Zone], &mZoneDescriptionString );
				// - Return a pointer to the required zone description
				pZoneDescription = ZONING_USERIF_NC7__pZoneDescription( Zone );
				// - Add zone description
				GENSTRDT__AddTextToString( pZoneDescription->pstrText, &mZoneDescriptionString );
				// - Draw Zone button
				if ( ZONING_CONTROL__bZoneListMaskBitIndicatesEnabled( Zone, pHeatingCooling->UserAccess.pSelectedScheduleTimePeriod->ucZoneList ) )
				{
					// Selected zone
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_A_ACTIVE_BTN + (Zone -1) * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				}
				else
				{
					// Unselected zone
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_A_PASSIVE_BTN + (Zone -1) * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				}
			}
		}
	}
	else
	{
		// Update zone display
		for ( Zone = TWBIFUTL__ZONE__A; Zone <= TWBIFUTL__ZONE__D; Zone++ )
		{
			// Only show installed zone
			if ( ZONING_CONTROL__bZoneIsInstalled( Zone ) )
			{
				// - Init container
				GENSTRDT__InitString( HTGCLG_USERIF_NC7__ZONE_DESC_LENGTH, (char*)&mstrZoneDescriptionText[0], &mZoneDescriptionString );
				// - Add zone identifier
				GENSTRDT__AssignTextToString( (char*)mkastrZoneIdentifier[Zone], &mZoneDescriptionString );
				// - Return a pointer to the required zone description
				pZoneDescription = ZONING_USERIF_NC7__pZoneDescription( Zone );
				// - Add zone description
				GENSTRDT__AddTextToString( pZoneDescription->pstrText, &mZoneDescriptionString );
				// - Only highlight the currently selected zone
				if ( NWSMSPCL__bZoneIsCurrentlyBeingAccessed( Zone ) )
				{
					// Selected zone
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_A_ACTIVE_BTN + (Zone -1) * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				}
				else
				{
					// Unselected zone
					UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_ZONE_A_PASSIVE_BTN + (Zone -1) * HTGCLG_USERIF_NC7__SCHEDULE_INDEX_OFFSET] );
				}
			}
		}
		//
		// Allow restore default schedule settings
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_RESTORE_DEFAULTS_BTN] );
	}
	//
	// Images : Associated with button presses
	// - Show passive level up button
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__SCHEDULE_LEVEL_UP_BTN_PASSIVE] );
	// - Show passive level down button
	UITFTDSP__RotateImage( 180, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__SCHEDULE_LEVEL_DN_BTN_PASSIVE] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Programming Mode.
 * @param:	pHeatingCooling	- Pointer to the heating/cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateControl_Mode_Programming(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;
	//
	// Get the tag value
	ucTagValue = TFTDSPIF__ucTouchTagValue();

	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__BACK_BTN )			// Allow back button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__HELP_BTN )			// Allow help button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( !NWSUTILS__bUserControlIsLockedOut() )													// Allow remaining only if not locked out
	{
		switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
		{
			case HTGCLG_USERIF_NC7__TV__SCHEDULE_RESTORE_DFLTS_BTN:
				// Restore default settings for current zone/time-period
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_RESTORE_DEFAULTS;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TIME_BTN:
				// Wake start time button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_WAKE_TIME;
				pHeatingCooling->UserAccess.bAlterTime			  = true;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TIME_BTN:
				// Leave start time button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_LEAVE_TIME;
				pHeatingCooling->UserAccess.bAlterTime			  = true;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TIME_BTN:
				// Return start time button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_RETURN_TIME;
				pHeatingCooling->UserAccess.bAlterTime			  = true;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TIME_BTN:
				// Pre-Sleep start time button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_PRESLEEP_TIME;
				pHeatingCooling->UserAccess.bAlterTime			  = true;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TIME_BTN:
				// Sleep start time button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_SLEEP_TIME;
				pHeatingCooling->UserAccess.bAlterTime			  = true;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TEMP_BTN:
				// Wake set temperature button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_WAKE_TEMP;
				pHeatingCooling->UserAccess.bAlterTime			  = false;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TEMP_BTN:
				// Leave set temperature button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_LEAVE_TEMP;
				pHeatingCooling->UserAccess.bAlterTime			  = false;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TEMP_BTN:
				// Return set temperature button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_RETURN_TEMP;
				pHeatingCooling->UserAccess.bAlterTime			  = false;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TEMP_BTN:
				// Pre-Sleep set temperature button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_PRESLEEP_TEMP;
				pHeatingCooling->UserAccess.bAlterTime			  = false;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

			case HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN:
				// Sleep set temperature button
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_SLEEP_TEMP;
				pHeatingCooling->UserAccess.bAlterTime			  = false;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_LEVEL_UP_BTN:
	   			if ( !pHeatingCooling->UserAccess.bAllow ) { break; }
	   			//
   				// Check whether time or set temperature to be altered
   				if ( pHeatingCooling->UserAccess.bAlterTime )
   				{
   					// Alter time up button: increase time by 1 minute
   					pHeatingCooling->UserAccess.Control.iTimeAdjust_Minutes = 1;
   					// Allow rapid update
   					HTGCLG_USERIF_NC7__RapidUpdateControl_Time();
   				}
   				else
   				{
   					// Alter set point button: increase by 1
   					pHeatingCooling->UserAccess.Control.iSetPointAdjust = 1;
   					// Allow rapid update
   					HTGCLG_USERIF_NC7__RapidUpdateControl_SetTemperature( true, pHeatingCooling->UserAccess.pSelectedScheduleTimePeriod->Common.ucSetTemperature );
   				}
   				break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_LEVEL_DN_BTN:
	   			if ( !pHeatingCooling->UserAccess.bAllow ) { break; }
	   			//
   				// Check whether time or set temperature to be altered
   				if ( pHeatingCooling->UserAccess.bAlterTime )
   				{
   					// Alter time button: decrease time by 1 minute
   					pHeatingCooling->UserAccess.Control.iTimeAdjust_Minutes = -1;
   					// Allow rapid update
   					HTGCLG_USERIF_NC7__RapidUpdateControl_Time();
   				}
   				else
   				{
   					// Alter set point button: decrease by 1
   					pHeatingCooling->UserAccess.Control.iSetPointAdjust = -1;
   					// Allow rapid update
   					HTGCLG_USERIF_NC7__RapidUpdateControl_SetTemperature( false, pHeatingCooling->UserAccess.pSelectedScheduleTimePeriod->Common.ucSetTemperature );
   				}
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_MONDAY_BTN:
	   			// Individual days - Monday
		   		if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__INDIVIDUAL_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_MONDAY; 	}
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_TUESDAY_BTN:
	   			// Individual days - Tuesdayday
		   		if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__INDIVIDUAL_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_TUESDAY;	}
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_WEDNESDAY_BTN:
	   			// Individual days - Wednesday
		   		if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__INDIVIDUAL_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_WEDNESDAY;	}
		   		break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_THURSDAY_BTN:
	   			// Individual days - Thursday
		   		if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__INDIVIDUAL_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_THURSDAY;	}
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_FRIDAY_BTN:
	   			// Individual days - Friday
		   		if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__INDIVIDUAL_DAYS )	{ pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_FRIDAY;	}
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_SATURDAY_BTN:
	   			// Individual days - Saturday
		   		if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__INDIVIDUAL_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_SATURDAY;	}
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_SUNDAY_BTN:
	   			// Individual days - Sunday
		   		if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__INDIVIDUAL_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_SUNDAY;	}
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKDAYS_BTN:
		   		// Weekends/Weekdays - Weekdays
	   			if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__WEEK_AND_WEEKEND_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_WEEKDAYS; }
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKENDS_BTN:
		   		// Weekends/Weekdays - Weekend days
	   			if ( pHeatingCooling->UserAccess.DayGrouping == NWSUTILS__DG__WEEK_AND_WEEKEND_DAYS ) { pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__PROG_SELECT_WEEKENDS; }
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_A_BTN:
   				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__TOGGLE_ZONE_A_ON_OFF;
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_B_BTN:
   				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__TOGGLE_ZONE_B_ON_OFF;
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_C_BTN:
   				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__TOGGLE_ZONE_C_ON_OFF;
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_D_BTN:
   				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__TOGGLE_ZONE_D_ON_OFF;
	   			break;

	        default:
	        	break;
		}
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;												// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )		// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case HTGCLG_USERIF_NC7__TV__BACK_BTN:
					// Back button
					pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__CANCEL_PROG_MODE_ACCESS;
					// Clear schedule access flag
					mbScheduleAccessEnabledByThisNetworker = false;
		   		    break;

				case HTGCLG_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	User display update - N-C7 : Off Mode.
 * @param:	pHeatingCooling - Pointer to the heating/cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateDisplay_Mode_Off(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	HTGCLG_USERIF_NC7__TemperatureControlSpecificDetails TemperatureControl;
	// Temperature control related display
    // - Define what we need to show in this operating mode
    TemperatureControl.bShowSetTemperature		= false;
    TemperatureControl.ucSetTemperature			= 0;
    TemperatureControl.pLevelButtons			= 0;
    /*
    TemperatureControl.bShowRoomTemperature		= true;
    */ // Show room temperature same as set temperature when switched on, NC7090_OTSCHECK
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		TemperatureControl.bShowRoomTemperature	= true;
	}
	else
	if ( NWSMSPCL__bAZoneIsCurrentlyBeingAccessed() )
	{
		TemperatureControl.bShowRoomTemperature	= true;
	}
	else
	{
		TemperatureControl.bShowRoomTemperature	= false;
	}
    TemperatureControl.pMSPControl				= pHeatingCooling->pMSPControlZone;
	// - Commit what is required
	HTGCLG_USERIF_NC7__UpdateTemperatureControlDetailsWhenInClimateControlState( &TemperatureControl, 0 );
	//
	// Update fan menu button image for top status area
	/*
	mpFanMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_MENU_BTN_PASSIVE];
	*/ // Update to allow for dual fuel buttons (gas/elec), NC7090_DFDISP
	// Heating Mode - Check for Dual Fuel Function to define buttons ** Only HP dual fuel mode returned from nwsutils so need to check this way unless NCX modified **
	if ( ( NWSUTILS__bClimateControlModeIsHeating() ) && ( NWSUTILS__bReverseCycleHeatingIsAvailable() ) && ( NWSUTILS__DualFuelOperation_bIsAvailable() ) )
	{
		// Dual Fuel is active so add buttons according to function - FAN, ELEC, GAS
		// - ELEC / GAS button images
		UpdateDisplay_ElecGasImageButtons();
		// - Fan image
		mpFanMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_FAN_MENU_BTN_PASSIVE];
		//
		// Auto switchover button
		if ( NWSUTILS__DualFuelOperation_bAutoSwitchFunctionEnabled() )
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__AUTO_ACTIVE_BTN] );
		}
		else
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__AUTO_PASSIVE_BTN] );
		}
	}
	else
	{
		mpFanMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_MENU_BTN_PASSIVE];
		mpGasMenuImage  = 0;
		mpElecMenuImage = 0;
	}
	//
	// Images : Associated with button presses
	// - On/Off button
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__ONOFF_BTN_PASSIVE] );
	// - Zones button (Allow zone toggling only if MSP enabled)
	if ( NWSUTILS__bMSPControlEnabled() )
    {
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__ZONE_BTN_PASSIVE] );
    }
	//
	// Text - Display zone being accessed (Flash if accessing another zone))
	UpdateDisplayWithZoneName( false, pHeatingCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User display update - N-C7 : Fan Only Mode.
 * @param:	pHeatingCooling - Pointer to the heating/cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateDisplay_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	HTGCLG_USERIF_NC7__TemperatureControlSpecificDetails TemperatureControl;
	//
	// Temperature control related display
    // - Define what we need to show in this operating mode
	TemperatureControl.bShowSetTemperature		= false;
	TemperatureControl.ucSetTemperature			= 0;
	TemperatureControl.pLevelButtons			= 0;
	/*
	TemperatureControl.bShowRoomTemperature		= true;
    */ // Show room temperature same as set temperature when switched on, NC7090_OTSCHECK
	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		TemperatureControl.bShowRoomTemperature	= true;
	}
	else
	if ( NWSMSPCL__bAZoneIsCurrentlyBeingAccessed() )
	{
		TemperatureControl.bShowRoomTemperature	= true;
	}
	else
	{
		TemperatureControl.bShowRoomTemperature	= false;
	}
	TemperatureControl.pMSPControl				= pHeatingCooling->pMSPControlZone;
	// - Commit what is required
	HTGCLG_USERIF_NC7__UpdateTemperatureControlDetailsWhenInClimateControlState( &TemperatureControl, 0 );
	//
	// Update fan menu button image for top status area
	mpFanMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_MENU_BTN_ACTIVE];
	mpGasMenuImage  = 0;	// NC7090_DFDISP
	mpElecMenuImage = 0;	// NC7090_DFDISP
	//
    // Fan control
	// - Fan animation
	if ( pHeatingCooling->UserAccess.FanDisplayState == HTGCLG_COMMON__FDS__SPIN )
	{
		// Rotate the fan
		UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel( pHeatingCooling->Requested.FanSpeedLevel, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_ICON] );
		// Show circulation fan indicator
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__CIRCFAN] );
	}
	// - Bargraph display and associated level buttons
	UITFTDSP__ShowBarGraph( pHeatingCooling->Requested.FanSpeedLevel, (UI_TFTSHAPE__BarGraphDetails*)&mkaBarGraph[HTGCLG_USERIF_NC7__BG__FAN_SPEED] );
	UITFTDSP__ShowLevelButtons( (UITFTDSP_BUTTON__LevelButtonDetails*)&mkaLevelButtons[HTGCLG_USERIF_NC7__LB__FAN_SPEED_BTN] );
	//
	// Images : Associated with button presses
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__ONOFF_BTN_ACTIVE] );
    //
	// Zone control - Only display if at least one switched zone is installed
	if ( ZONING_CONTROL__bAtLeastOneSwitchedZoneIsInstalled() )
    {
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__ZONE_BTN_PASSIVE] );

    	if ( ( ZONING_CONTROL__pZoning()->UserAccess.bFlashIfNoZonesEnabled ) && ( ZONING_CONTROL__pZoning()->UserAccess.ucNumberOfZonesEnabled == 0 ) )
    	{
    		UITFTDSP__FlashRectangle( UITFTDSP__FR__1HZ, (UI_TFTSHAPE__Rectangle*)&mkaRectangle[HTGCLG_USERIF_NC7__RN__ZONE_BLANK] );
    	}
    }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Fan Only Mode.
 * @param:	pHeatingCooling	- Pointer to the heating/cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateControl_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;
	//
	// Get the tag value
	ucTagValue = TFTDSPIF__ucTouchTagValue();

	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__MENU_BTN )			// Allow menu button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue == HTGCLG_USERIF_NC7__TV__HELP_BTN )			// Allow help button always
	{
		sucActiveTag = ucTagValue; HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	}
	else
	if ( !NWSUTILS__bUserControlIsLockedOut() )													// Allow remaining only if not locked out
	{
		switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
		{
			case HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN:
				// Fan menu button - Switch off
			case HTGCLG_USERIF_NC7__TV__ONOFF_BTN:
				// On/Off button - Switch off
				pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_OFF;
				// No rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_None();
				break;

	   		case HTGCLG_USERIF_NC7__TV__LEVEL_UP_BTN:
	   			// Fan speed up button - Increase fan speed level by 1
	   			pHeatingCooling->UserAccess.Control.iFanSpeedAdjust = 1;
	   			// Rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_FanSpeedLevel( true, pHeatingCooling->Requested.FanSpeedLevel );
	   			break;

	   		case HTGCLG_USERIF_NC7__TV__LEVEL_DN_BTN:
	   			// Fan speed down button - Decrease fan speed level by 1
	   			pHeatingCooling->UserAccess.Control.iFanSpeedAdjust = -1;
	   			// Rapid update
	   			HTGCLG_USERIF_NC7__RapidUpdateControl_FanSpeedLevel( false, pHeatingCooling->Requested.FanSpeedLevel );
	   			break;

			case HTGCLG_USERIF_NC7__TV__ALERT_BTN:
	   		case HTGCLG_USERIF_NC7__TV__ZONES_BTN:
			case HTGCLG_USERIF_NC7__TV__MODE_BTN:
	    		// Remember this tag as it causes screen to switch
	   			sucActiveTag = ucTagValue;
	   			// No rapid update
	    	    HTGCLG_USERIF_NC7__RapidUpdateControl_None();
	   			break;

	        default:
	        	break;
		}
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if (sucActiveTag != 0)
	{
		ucTagValue = sucActiveTag;												// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )		// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (HTGCLG_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case HTGCLG_USERIF_NC7__TV__ALERT_BTN:
					NWS_SVCINF_CONTROL__ServiceInfoPrompt_FlagToShowIt( &mServiceInfo );
					break;

				case HTGCLG_USERIF_NC7__TV__ZONES_BTN:
					// Initialise before access
					ZONING_CONTROL__InitialiseUIBeforeFirstAccess( ZONING_CONTROL__ucUserEnabledZoneStateList() );
					// Access zoning details
					pHeatingCooling->UserAccess.bZoningDetails = true;
					break;

				case HTGCLG_USERIF_NC7__TV__MODE_BTN:
					pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__SWITCH_MODE;
					break;

				case HTGCLG_USERIF_NC7__TV__MENU_BTN:
					pHeatingCooling->UserAccess.Control.ButtonPressed = HTGCLG_COMMON__CB__ACCESS_CONFIG_MENU;
		   		    break;

				case HTGCLG_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	User display update - N-C7 : On Mode.
 * @param:	pHeatingCooling - Pointer to the heating/cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateDisplay_Mode_On(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	bool		bUserInfoAvailable = false;
	//
	// Fan blade animation
	switch (pHeatingCooling->UserAccess.FanDisplayState)
	{
		case HTGCLG_COMMON__FDS__SPIN:
			// Rotate the fan
			UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel( TWBIFUTL__FSL__11, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_ICON] );
			// Show circulation fan indicator
			if ( ( HTGCLG_COMMON__bCirculationFanOptionEnabled() ) && ( pHeatingCooling->Requested.oCirculationFanState == on ) && ( !pHeatingCooling->bFlaggedAsOperating ) )
			{
				UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__CIRCFAN] );
			}
			break;

		case HTGCLG_COMMON__FDS__FLASH:
			// Flash the fan
			UITFTDSP__FlashImage( UITFTDSP__FR__1HZ, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_ICON] );
			break;

		default:
			break;
	}
	//
	// Check if circulation fan control enabled
	if ( HTGCLG_COMMON__bCirculationFanOptionEnabled() )
    {
		// Check for circulation fan state
		if ( NWSUTILS__DualFuelOperation_bIsAvailable() )				// When dual fuel active no circ fan function, NC7090_DFDISP
		{
			mpFanMenuImage = 0;
		}
		else
		if ( pHeatingCooling->Requested.oCirculationFanState == on )
		{
			// Update fan menu button image for top status area
			mpFanMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_MENU_BTN_ACTIVE];
		}
		else
		{
			// Update fan menu button image for top status area
			mpFanMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__FAN_MENU_BTN_PASSIVE];
		}
	}
	else
	{
		// Do not show fan menu button
		mpFanMenuImage = 0;
	}
	//
	// Elec / Gas buttons, NC7090_DFDISP - Check for Dual Fuel Function to define buttons ** Only HP dual fuel mode returned from nwsutils so need to check this way unless NCX modified **
	if ( ( NWSUTILS__bClimateControlModeIsHeating() ) && ( NWSUTILS__bReverseCycleHeatingIsAvailable() ) && ( NWSUTILS__DualFuelOperation_bIsAvailable() ) )
	{
		// Dual Fuel is active so add buttons according to function - ELEC / GAS
		UpdateDisplay_ElecGasImageButtons();
		// Auto switchover button
		if ( NWSUTILS__DualFuelOperation_bAutoSwitchFunctionEnabled() )
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__AUTO_ACTIVE_BTN] );
		}
		else
		{
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__AUTO_PASSIVE_BTN] );
		}
	}
	else
	{
		mpGasMenuImage  = 0;
		mpElecMenuImage = 0;
	}
	//
	// Set user info available flag
	if ( pHeatingCooling->pMSPControlZone == COM_DEFN__NULL )
	{
		bUserInfoAvailable = true;
	}
	else
	if ( pHeatingCooling->pMSPControlZone->bSetPointAccessible )
	{
		bUserInfoAvailable = true;
	}
	else
	{
		bUserInfoAvailable = false;
	}
	//
	// Check user info flag
	bool 	bShowManualScheduleButtons;

	if ( !NWSUTILS__bMSPControlEnabled() )
	{
		bShowManualScheduleButtons	= true;
	}
	else
	if ( NWSMSPCL__bAZoneIsCurrentlyBeingAccessed() )
	{
		bShowManualScheduleButtons	= true;
	}
	else
	{
		bShowManualScheduleButtons	= false;
	}
	/*
	if ( bUserInfoAvailable )
	*/ // Show manual/schedule same as set temperature, NC7090_OTSCHECK
	if ( ( bUserInfoAvailable ) && ( bShowManualScheduleButtons ) )
	{
		// Manual/Auto control specific
		if ( pHeatingCooling->pMSPUserRequest->ScheduleControl.bEnabled )
		{
			// Auto program operating mode
			// - Initialise strings used for text displays
			GENSTRDT__InitString( HTGCLG_USERIF_NC7__TIME_PERIOD_TEXT_LENGTH , &mstrTimePeriodText[0], &mTimePeriodString );
			// - Check for temporary override
			if ( pHeatingCooling->pMSPUserRequest->ScheduleControl.bTemporaryOverrideEnabled )
			{
				// Set temporary override flag - flash icon in bottom status update.
				mbTemporaryOverrideEnabled = true;
				// Check for advance period button press
				if ( pHeatingCooling->pMSPUserRequest->ScheduleControl.bTemporaryOverrideEnabledDueToTimePeriodBeingAdvanced )
				{
					// Advance period button pressed
			    	// - Update string
					GENSTRDT__AssignTextToString( (char*)mkastrTimePeriod[pHeatingCooling->pMSPUserRequest->ScheduleControl.TimePeriodAdvancedTo], &mTimePeriodString );
					// - Show advance time period button
					UITFTDSP__ShowButtonWithTwoTexts( (UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[HTGCLG_USERIF_NC7__TTBN__ADVANCE_PERIOD_ACTIVE_BTN] );
				}
				else
				{
					// Set temperature adjusted
			    	// - Update string
			    	GENSTRDT__AssignTextToString( (char*)mkastrTimePeriod[pHeatingCooling->pMSPUserRequest->ScheduleControl.CurrentTimePeriod], &mTimePeriodString );
			    	// - Show current time period button
					UITFTDSP__ShowButtonWithTwoTexts( (UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[HTGCLG_USERIF_NC7__TTBN__CURRENT_PERIOD_ACTIVE_BTN] );
				}
				// Show cancel button
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__CANCEL_BTN] );
			}
			else
			{
				// No temporary override - show current time period
		    	// - Update string
		    	GENSTRDT__AssignTextToString( (char*)mkastrTimePeriod[pHeatingCooling->pMSPUserRequest->ScheduleControl.CurrentTimePeriod], &mTimePeriodString );
		    	// - Show current time period button
				UITFTDSP__ShowButtonWithTwoTexts( (UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition*)&mkaTwoTextsButton[HTGCLG_USERIF_NC7__TTBN__CURRENT_PERIOD_ACTIVE_BTN] );
			}
			//
			// - Show passive manual button
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__MANUAL_PASSIVE_BTN] );
		}
		else
		{
			// Manual operating mode
			// - Show active manual button
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__MANUAL_ACTIVE_BTN] );
			// - Show passive schedule button
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[HTGCLG_USERIF_NC7__BN__SCHEDULE_BTN] );
		}
	}
	//
	// Images : Associated with button presses
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__ONOFF_BTN_ACTIVE] );
    //
	// Zone control - Only display if at least one switched zone is installed
	if ( ZONING_CONTROL__bAtLeastOneSwitchedZoneIsInstalled() )
    {
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__ZONE_BTN_PASSIVE] );

    	if ( ( ZONING_CONTROL__pZoning()->UserAccess.bFlashIfNoZonesEnabled ) && ( ZONING_CONTROL__pZoning()->UserAccess.ucNumberOfZonesEnabled == 0 ) )
    	{
    		UITFTDSP__FlashRectangle( UITFTDSP__FR__1HZ, (UI_TFTSHAPE__Rectangle*)&mkaRectangle[HTGCLG_USERIF_NC7__RN__ZONE_BLANK] );
    	}    }
	//
	// Text - Display zone being accessed (Flash if accessing another zone))
	UpdateDisplayWithZoneName( false, pHeatingCooling );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Common display update under normal operation - N-C7.
 * @param:	vMode			- Mode of operation (HTG or CLG)
 * 			pHeatingCooling	- Pointer to the heating/Cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateDisplay_Mode_NormalOperation(NWSUTILS__CLIMATE_CONTROL_MODE vMode, HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	UICOMMON__TopStatusDetails	TopStatus;
	//
	// Check for mode switch under either master or slave operation
	if ( pHeatingCooling->bSlaveModeSwitch || UICOMMON__bMasterModeSwitch() )
	{
	    // Update top status area
		// - Initialise structure
		UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define what we need to show in this operating mode
		TopStatus.pstrTextToDisplay		= "SWITCHING MODE - PLEASE WAIT";
		TopStatus.TextFlashRate			= UITFTDSP__FR__2HZ;
		TopStatus.bFlashText 			= true;
		// - Commit what is required
		UICOMMON__TopStatus_Update( &TopStatus );

		return;
	}
	//
    // Update top status area
	// - Initialise structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define what we need to show in this operating mode
	TopStatus.pClock  			   	= RTMCLKIF__pClock();
	TopStatus.bClockIn12HourFormat 	= TWBIFUTL__bClockDisplayFormatIn12HourFormat();
	TopStatus.pucOutdoorTemperature = NWSUTILS__pucAmbientTemperature();
	TopStatus.pImage1 				= mpFanMenuImage;
	TopStatus.pImage2 				= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__MENU_BTN_PASSIVE];
#if defined(INCLUDE_HELP_MENU_OPTION)
	TopStatus.pImage3 				= (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__HELP_BTN_PASSIVE];
#endif
	TopStatus.pImage4 				= mpGasMenuImage;		// Allow for xtra image, NC7090_DFDISP
	TopStatus.pImage5 				= mpElecMenuImage;		// Allow for xtra image, NC7090_DFDISP
	// - Commit what is required
	UICOMMON__TopStatus_Update( &TopStatus );
	//
	// Update bottom status area
	UpdateDisplay_SetupBottomStatus( pHeatingCooling );
	// Update attention status
	UpdateServiceDetailsStatus( pHeatingCooling );

	if ( mServiceInfo.MessageType != NWS_SVCINF_COMMON__MT__NONE )
	{
		// Show alert image
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__ALERT_ICON] );
		// Flash Attention text
		UITFTDSP__FlashTextAbsolute( UITFTDSP__FR__2HZ, (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__ATTENTION] );
	}
	//
    // Images : No button press
	UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__BRIVIS_LOGO] );
	//
	// Check if two or more modes available
	if ( ( NWSUTILS__bEvaporativeCoolingIsAvailable() ) || ( NWSUTILS__bCoolingIsAvailable() ) )
	{
		// Images : Associated with button presses
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__MODE_BTN_PASSIVE] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - None.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__RapidUpdateControl_None(void)
{
	UITFTDSP__TouchControl_RapidUpdateDataInit();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - Time update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__RapidUpdateControl_Time(void)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = true;
	TouchTagValue.ucMinimum 		 = 0;
	TouchTagValue.ucMaximum 		 = 59;

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - Fan speed level.
 * @param:	vbIncreasingValue - Whether value is being increased or decreased.
 * 			vFanSpeedLevel	  - The value of the fan speed level being adjusted.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__RapidUpdateControl_FanSpeedLevel(bool vbIncreasingValue, TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = false;
	TouchTagValue.bIncreasing        = vbIncreasingValue;
	TouchTagValue.ucMinimum			 = TWBIFUTL__FSL__01;
	TouchTagValue.ucMaximum			 = TWBIFUTL__FSL__16;
	TouchTagValue.ucActual			 = (uint8_t)vFanSpeedLevel;

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - Set Temperature.
 * @param:	vbIncreasingValue - Whether value is being increased or decreased.
 * 			vucSetTemperature - The value of the set temperature being adjusted.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__RapidUpdateControl_SetTemperature(bool vbIncreasingValue, uint8_t vucSetTemperature)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = false;
	TouchTagValue.bIncreasing        = vbIncreasingValue;
	TouchTagValue.ucMinimum			 = HTGCLG_COMMON__SET_TEMPERATURE_MIN;
	TouchTagValue.ucMaximum			 = HTGCLG_COMMON__SET_TEMPERATURE_MAX;
	TouchTagValue.ucActual			 = vucSetTemperature;

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates temperature control specific details when in climate control state.
 * @param:	pTempCtrlSpecific - Pointer to temperature control specific defined info.
 * 			pSetTemp		  - Pointer to set temperature string.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__UpdateTemperatureControlDetailsWhenInClimateControlState(HTGCLG_USERIF_NC7__TemperatureControlSpecificDetails* pTempCtrlSpecific, GENSTRDT__String* pSetTemp)
{
    // 1) Set temperature
	if ( pTempCtrlSpecific->bShowSetTemperature )
	{
		if ( pTempCtrlSpecific->pMSPControl == COM_DEFN__NULL )
		{
			ShowTemperatureWithAssociatedButton( true, pTempCtrlSpecific->ucSetTemperature, 0, pTempCtrlSpecific->pLevelButtons, pSetTemp );
		}
		else
		{
			ShowTemperatureWithAssociatedButton( pTempCtrlSpecific->pMSPControl->bSetPointAccessible, pTempCtrlSpecific->pMSPControl->ucRequestSetPoint, 0, pTempCtrlSpecific->pLevelButtons, pSetTemp );
		}
	}
	//
	// 2) Room temperature
	if ( pTempCtrlSpecific->bShowRoomTemperature )
	{
		if ( pTempCtrlSpecific->pMSPControl == COM_DEFN__NULL )
		{
			if ( TEMPMSIF__bTemperatureMeasurementThermistorIsFaulty() )
			{
	            // Thermistor faulty so show "XX"
				UICOMMON__ShowRoomTemperature(255);
	        }
			else
			{
	            // Thermistor ok so show temperature
				UICOMMON__ShowRoomTemperature( TEMPMSIF__ucMeasuredTemperature( TWBIFUTL__bTemperatureDisplayFormatInCelsius() ) );
			}
		}
		else
		{
			if ( pTempCtrlSpecific->pMSPControl->uiTemperature == 0xFFFF )
			{
				// Thermistor faulty so show "XX"
				UICOMMON__ShowRoomTemperature(255);
			}
			else
	    	if ( TWBIFUTL__bTemperatureDisplayFormatInCelsius() ) // Thermistor ok so show temperature
	    	{
	    		UICOMMON__ShowRoomTemperature( GENUTILS__ucValueConvertedFromUI16(pTempCtrlSpecific->pMSPControl->uiTemperature) );
	    	}
	    	else
	    	{
	    		UICOMMON__ShowRoomTemperature( TEMPMSIF__ucTemperatureInFahrenheit( GENUTILS__ucValueConvertedFromUI16( pTempCtrlSpecific->pMSPControl->uiTemperature ) ) );
	    	}
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the flag indicating whether or not the schedule access is being enabled by this networker.
 * @param:	vbEnableAccess - Whether or not the schedule access flag is to be set.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void									HTGCLG_USERIF_NC7__EnableScheduleAccessByThisNetworker(bool vbEnableAccess)
{
	mbScheduleAccessEnabledByThisNetworker = vbEnableAccess;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the flag indicating whether or not the schedule access is being enabled by this networker.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool									HTGCLG_USERIF_NC7__bScheduleAccessEnabledByThisNetworker(void)
{
	return mbScheduleAccessEnabledByThisNetworker;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates bottom status display.
 * @param:	pHeatingCooling	- Pointer to the heating/Cooling control details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								UpdateDisplay_SetupBottomStatus(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	// Define what we need to show in this operating mode
	// - Flash temporary override indicator
	if ( mbTemporaryOverrideEnabled )
	{
		// Update bottom status text
		UICOMMON__BottomStatus_UpdateText();
		// Clear temporary override flag
		mbTemporaryOverrideEnabled = false;
	}
	// - Display heart indicator if in switch on mode (auto or manual)
	if ( ( pHeatingCooling->Current.Operation == TWBCMDSQ__OPN__SWCHONAUT ) || ( pHeatingCooling->Current.Operation == TWBCMDSQ__OPN__SWCHONMAN ) )
	{
		if ( NWSMSPCL__bThisType02IsTemperatureSensingAZone() )
		{
			// Flash heart indicator
			UICOMMON__BottomStatus_UpdateImage( UI_COMMON__BSN_HEART, true, UITFTDSP__FR__2HZ );
		}
		else
		{
			// Show heart indicator
			UICOMMON__BottomStatus_UpdateImage( UI_COMMON__BSN_HEART, false, 0 );
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows temperature with associated button(s).
 * @param:	vbAllowUserAccess  - A flag indicating whether or not to allow for user access.
 * 			vucTemperature	   - Temperature to display.
 * 			pTemperatureButton - Pointer to temperature definition details.
 * 			pLevelButtons	   - Pointer to level button application specific defined info.
 * 			pTemperature	   - Pointer to temperature string.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								ShowTemperatureWithAssociatedButton(bool vbAllowUserAccess, uint8_t vucTemperature, UITFTDSP_BUTTON__TemperatureButtonDefinition* pTemperatureButton, UITFTDSP_BUTTON__LevelButtonDetails* pLevelButtons, GENSTRDT__String* pTemperature)
{
	UI_TFTSHAPE__PointDetails	OuterPoint, InnerPoint;
	//
	// Copy data
	// - Check if temperature button or level buttons defined
	if ( pTemperatureButton != 0 )
	{
		// - Outer point color and size
	    OuterPoint.ulColor	   = pTemperatureButton->Button.Container.Text.ulColor;
	    OuterPoint.ulPointSize = pTemperatureButton->DegSymbol.ulPointSize;
		// - Inner point color
	    InnerPoint.ulColor	   = pTemperatureButton->Button.Container.Rectangle.ulColor;
	}
	else
	if ( pLevelButtons != 0 )
	{
		// - Outer point color and size
	    OuterPoint.ulColor	   = pLevelButtons->Container.Text1.ulColor;
	    OuterPoint.ulPointSize = pLevelButtons->DegSymbol.ulPointSize;
		// - Inner point color
	    InnerPoint.ulColor	   = UITFTDSP_COLOR__BACKGROUND;
	}
	// - Inner point size
	InnerPoint.ulPointSize = OuterPoint.ulPointSize - 1;

	if ( !vbAllowUserAccess )
	{
		// Show "??" when temperature not accessible
		GENSTRDT__AddTextToString( "??", pTemperature );
		// Define outer point posn
		// - Check if temperature button or level buttons defined
		if ( pTemperatureButton != 0 )
		{
	    	OuterPoint.iX = pTemperatureButton->Button.Container.Rectangle.iX + pTemperatureButton->DegSymbol.iXOffset2;
	    	OuterPoint.iY = pTemperatureButton->Button.Container.Rectangle.iY + pTemperatureButton->DegSymbol.iYOffset2;
		}
		else
		if ( pLevelButtons != 0 )
		{
	    	OuterPoint.iX = pLevelButtons->Container.Rectangle.iX + pLevelButtons->DegSymbol.iXOffset2;
	    	OuterPoint.iY = pLevelButtons->Container.Rectangle.iY + pLevelButtons->DegSymbol.iYOffset2;
		}
	}
	else
	{
		if ( vucTemperature != 0 )
		{
			if ( !TWBIFUTL__bTemperatureDisplayFormatInCelsius() )
			{
				vucTemperature = TEMPMSIF__ucTemperatureInFahrenheit( vucTemperature );
			}
			// Convert temperature to string
			GENSTRDT__Add2DigitNumberToString( vucTemperature, false, pTemperature );
			//
		    // Define outer point posn
		    if ( vucTemperature < 10 )
		    {
		    	// - Check if temperature button or level buttons defined
		    	if ( pTemperatureButton != 0 )
		    	{
			    	OuterPoint.iX = pTemperatureButton->Button.Container.Rectangle.iX + pTemperatureButton->DegSymbol.iXOffset1;
			    	OuterPoint.iY = pTemperatureButton->Button.Container.Rectangle.iY + pTemperatureButton->DegSymbol.iYOffset1;
		    	}
		    	else
		    	if ( pLevelButtons != 0 )
		    	{
			    	OuterPoint.iX = pLevelButtons->Container.Rectangle.iX + pLevelButtons->DegSymbol.iXOffset1;
			    	OuterPoint.iY = pLevelButtons->Container.Rectangle.iY + pLevelButtons->DegSymbol.iYOffset1;
		    	}
		    }
		    else
		    {
		    	// - Check if temperature button or level buttons defined
		    	if ( pTemperatureButton != 0 )
		    	{
			    	OuterPoint.iX = pTemperatureButton->Button.Container.Rectangle.iX + pTemperatureButton->DegSymbol.iXOffset2;
			    	OuterPoint.iY = pTemperatureButton->Button.Container.Rectangle.iY + pTemperatureButton->DegSymbol.iYOffset2;
		    	}
		    	else
		    	if ( pLevelButtons != 0 )
		    	{
			    	OuterPoint.iX = pLevelButtons->Container.Rectangle.iX + pLevelButtons->DegSymbol.iXOffset2;
			    	OuterPoint.iY = pLevelButtons->Container.Rectangle.iY + pLevelButtons->DegSymbol.iYOffset2;
		    	}
		    }
		}
		else
		{
			// Dash the set temperature (Switched off)
			GENSTRDT__AddTextToString( "Off", pTemperature );
			// Do not show degree symbol
			OuterPoint.ulPointSize = 0;
			InnerPoint.ulPointSize = 0;
		}
	}
	//
	// Show temperature and associated button(s)
	// - Check if temperature button or level buttons defined
	if ( pTemperatureButton != 0 )
	{
		UITFTDSP__ShowButtonWithColor( &(pTemperatureButton->Button) );
	}
	else
	if ( pLevelButtons != 0 )
	{
		UITFTDSP__ShowLevelButtons( pLevelButtons );
	}
	//
    // Define inner point posn
    InnerPoint.iX = OuterPoint.iX;
    InnerPoint.iY = OuterPoint.iY;
    //
	// Show temperature degree symbol
    UITFTDSP__ShowDegreeSymbol( &OuterPoint, &InnerPoint );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates display with zone name.
 * @param:	pHeatingCooling	- Pointer to the heating/Cooling control details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								UpdateDisplayWithZoneName(bool vbScheduleScreen, HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	GENSTRDT__String*	pZoneDescription;

	if ( !NWSUTILS__bMSPControlEnabled() ) 	{ return; }

	if ( pHeatingCooling->pMSPControlZone == COM_DEFN__NULL ) { return; }			// No zone description if this is null, emilio 11-07-18

	if ( ( pZoneDescription = ZONING_USERIF_NC7__pZoneDescription( pHeatingCooling->pMSPControlZone->ZoneBeingAccessed ) ) == 0) { return; }
	//
	// Init container
	GENSTRDT__InitString( HTGCLG_USERIF_NC7__ZONE_DESC_LENGTH, (char*)&mstrZoneDescriptionText[0], &mZoneDescriptionString );
	// Form the string
	switch ( pHeatingCooling->pMSPControlZone->ZoneBeingAccessed )
	{
		case TWBIFUTL__ZONE__A:
			GENSTRDT__AssignTextToString( "Zone A", &mZoneDescriptionString );
			break;

		case TWBIFUTL__ZONE__B:
			GENSTRDT__AssignTextToString( "Zone B", &mZoneDescriptionString );
			break;

		case TWBIFUTL__ZONE__C:
			GENSTRDT__AssignTextToString( "Zone C", &mZoneDescriptionString );
			break;

		case TWBIFUTL__ZONE__D:
			GENSTRDT__AssignTextToString( "Zone D", &mZoneDescriptionString );
			break;

		default:
			// This can never happen - Needed to keep compiler happy!
			break;
	}

	if ( pZoneDescription->uiLength != 0 ) { GENSTRDT__AddTextToString( " : ", &mZoneDescriptionString ); }

	GENSTRDT__AddTextToString( pZoneDescription->pstrText, &mZoneDescriptionString );

	if ( vbScheduleScreen )
	{
		if ( pHeatingCooling->pMSPControlZone->bFlash )
		{
			UITFTDSP__FlashTextAbsolute( UITFTDSP__FR__1HZ, (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__ZONEDESC_PROG] );
		}
		else
		{
			UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__ZONEDESC_PROG] );
		}
	}
	else
	{
		if ( pHeatingCooling->pMSPControlZone->bFlash )
		{
			UITFTDSP__FlashTextAbsolute( UITFTDSP__FR__1HZ, (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__ZONEDESC] );
		}
		else
		{
			UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[HTGCLG_USERIF_NC7__TN__ZONEDESC] );
		}

	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update service info details variable - mServiceInfo.
 * @param:	pHeatingCooling - Pointer to the heating/Cooling control details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void								UpdateServiceDetailsStatus(HTGCLG_COMMON__ControlDetails* pHeatingCooling)
{
	if ( NWSUTILS__bAFaultMessageForDisplayExists() )
    {
        // Appliance fault detected - Fault code will determine messsage to display: HTR 40-49 		 (BUSY)
    	//																			 HTR <40 or >=50 (FAULT)
        // Check what type of fault
        if ( NWSUTILS__DeviceGroupIDForCurrentFault() != TWBCOMIF__GID__GRP03 )
        {
    		// Refrig cooling - All faults if any of equal standing
        	mServiceInfo.MessageType = NWS_SVCINF_COMMON__MT__FAULT_TRANSIENT;
        	mServiceInfo.GroupID     = NWSUTILS__DeviceGroupIDForCurrentFault();
        	mServiceInfo.UnitID      = NWSUTILS__DeviceUnitIDForCurrentFault();
        	mServiceInfo.ucFaultCode = NWSUTILS__ucFaultCodeRegistered();
        }
        else
        if ( NWSUTILS__ucFaultCodeRegistered() < 50 )
        {
    		// Heating with transient/busy fault
        	mServiceInfo.MessageType = NWS_SVCINF_COMMON__MT__FAULT_TRANSIENT;
        	mServiceInfo.GroupID     = NWSUTILS__DeviceGroupIDForCurrentFault();
        	mServiceInfo.UnitID      = NWSUTILS__DeviceUnitIDForCurrentFault();
        	mServiceInfo.ucFaultCode = NWSUTILS__ucFaultCodeRegistered();
    	}
        else
        if ( NWSUTILS__ucFaultCodeRegistered() > 49 )
        {
    		// Heating with resettable lockout fault
        	mServiceInfo.MessageType = NWS_SVCINF_COMMON__MT__FAULT_LOCKOUT;
        	mServiceInfo.GroupID     = NWSUTILS__DeviceGroupIDForCurrentFault();
        	mServiceInfo.UnitID      = NWSUTILS__DeviceUnitIDForCurrentFault();
        	mServiceInfo.ucFaultCode = NWSUTILS__ucFaultCodeRegistered();
        }
        else
        {
    		// Heating with busy fault code - Set general message to indicate heater busy
        	mServiceInfo.MessageType = NWS_SVCINF_COMMON__MT__NONE;
    	}
    }
    else
    if ( pHeatingCooling->bServiceNotificationRequired )
    {
    	mServiceInfo.MessageType 		= NWS_SVCINF_COMMON__MT__SERVICE_NOTIFY;
    	mServiceInfo.GroupID     		= TWBCOMIF__GID__GRP03;
    	mServiceInfo.UnitID      		= SERVCLOG_HEATCOOL__HeaterUnitIDWithFanHoursThatHaveExceededNotificationLimit();
    	mServiceInfo.uiFanHoursLogged   = SERVCLOG_HEATCOOL__uiHeaterFanOperationHoursLogged( mServiceInfo.UnitID );
    }
    else
    {
        // No message - Clear
    	mServiceInfo.MessageType = NWS_SVCINF_COMMON__MT__NONE;
    }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates state of ELEC/GAS image buttons.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void							UpdateDisplay_ElecGasImageButtons(void)
{
	if ( NWSUTILS__DualFuelOperation_bHeatPumpActive() )
	{
		// Heat Pump mode
		if ( NWSUTILS__DualFuelOperation_bAutoSwitchFunctionEnabled() )
		{
			// Auto - Follow Mode
			mpGasMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_PASSIVE];
			mpElecMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_ACTIVE];
		}
		else
		{
			// Not Auto - Follow what has been forced as user may have toggled it...
			if ( NWSUTILS__DualFuelOperation_bGasHeatUserForced() )
			{
				mpGasMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_ACTIVE];
				mpElecMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_PASSIVE];
			}
			else
			{
				mpGasMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_PASSIVE];
				mpElecMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_ACTIVE];
			}
		}
	}
	else
	{
		// Mode - Gas Heat
		if ( NWSUTILS__DualFuelOperation_bAutoSwitchFunctionEnabled() )
		{
			// Auto - Follow Mode
			mpGasMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_ACTIVE];
			mpElecMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_PASSIVE];
		}
		else
		{
			// Not Auto - Follow what has been forced as user may have toggled it...
			if ( !NWSUTILS__DualFuelOperation_bGasHeatUserForced() )
			{
				mpGasMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_PASSIVE];
				mpElecMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_ACTIVE];
			}
			else
			{
				mpGasMenuImage  = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_GAS_BTN_ACTIVE];
				mpElecMenuImage = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[HTGCLG_USERIF_NC7__IN__DF_ELEC_BTN_PASSIVE];
			}
		}
	}

	return;
}
//*********************************************************************************************************************************************************************************
