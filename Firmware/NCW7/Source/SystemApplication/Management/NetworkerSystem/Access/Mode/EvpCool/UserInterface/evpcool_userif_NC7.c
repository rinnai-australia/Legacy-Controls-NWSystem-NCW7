//*********************************************************************************************************************************************************************************
// evpcool_userif_NC7.c
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
#include		<genutils.h>
#include		<evpcool_common.h>
#include		<nws_svcinf_control.h>
#include		<uitftdsp.h>
#include		<uitftdsp_shape.h>
#include		<uitftdsp_color.h>
#include		<uicommon.h>
#include        <tftdspif.h>
#include		<zoning_control.h>
#include		<servclog_evapcool.h>
#include		<nwsmspcl.h>
#include		<nwsutils.h>
#include		<twbif502.h>
#include		<tempmsif.h>
#include		<rtmclkif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Top status text
static const char 	mkstrTopStatusText_ProgTimerOn[]  = "EVAP TIMER : SET ON TIMER";
static const char 	mkstrTopStatusText_ProgTimerOff[] = "EVAP TIMER : SET OFF TIMER";
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// String definition details - ** Array variables as text to display will vary so define here for pointer inclusion in text table **
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Time display text
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			EVPCOOL_USERIF_NC7__TIME_TEXT_LENGTH				10
// - Actual container for string
static char 			mstrTimeText[EVPCOOL_USERIF_NC7__TIME_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mTimeString;
//
// Time programmed display text
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Max length allowed
#define			EVPCOOL_USERIF_NC7__TIME_PROGRAMMED_TEXT_LENGTH		32
// - Actual container for string
static char 			mstrTimeProgrammedText[EVPCOOL_USERIF_NC7__TIME_PROGRAMMED_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mTimeProgrammedString;
//
// Water drop control
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Time to display an icon
#define			EVPCOOL_USERIF_NC7__WATER_DROP_DISPLAY_TIME_TBUI	(50/GENTIMDT__kucUI_TIMEBASE_10MS)
// The drop numbers
typedef enum	EVPCOOL_USERIF_NC7__WATER_DROP_NUMBER {

	EVPCOOL_USERIF_NC7__WDN__DROP_1 = 0,
	EVPCOOL_USERIF_NC7__WDN__DROP_2 = 1,
	EVPCOOL_USERIF_NC7__WDN__DROP_3 = 2,
	EVPCOOL_USERIF_NC7__WDN__RIPPLE = 3

} EVPCOOL_USERIF_NC7__WATER_DROP_NUMBER;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	EVPCOOL_USERIF_NC7__TAG_VALUE {

	EVPCOOL_USERIF_NC7__TV__HELP_BTN 	 = 1,
	EVPCOOL_USERIF_NC7__TV__BACK_BTN 	 = 2,
	EVPCOOL_USERIF_NC7__TV__MENU_BTN 	 = 3,

	EVPCOOL_USERIF_NC7__TV__ONOFF_BTN 	 = 11,
	EVPCOOL_USERIF_NC7__TV__MODE_BTN 	 = 12,
	EVPCOOL_USERIF_NC7__TV__ZONES_BTN 	 = 13,

	EVPCOOL_USERIF_NC7__TV__PUMP_BTN	 = 14,
	EVPCOOL_USERIF_NC7__TV__FAN_BTN		 = 15,
	EVPCOOL_USERIF_NC7__TV__MANUAL_BTN	 = 16,
	EVPCOOL_USERIF_NC7__TV__AUTO_BTN	 = 17,
	EVPCOOL_USERIF_NC7__TV__TIMER_BTN	 = 18,
	EVPCOOL_USERIF_NC7__TV__LEVEL_UP     = 19,
	EVPCOOL_USERIF_NC7__TV__LEVEL_DN     = 20,
	EVPCOOL_USERIF_NC7__TV__TIME_UP      = 21,
	EVPCOOL_USERIF_NC7__TV__TIME_DN      = 22,
	EVPCOOL_USERIF_NC7__TV__DONE_BTN     = 23,
	EVPCOOL_USERIF_NC7__TV__RESET_BTN    = 24,

	EVPCOOL_USERIF_NC7__TV__ALERT_BTN    = 25

} EVPCOOL_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	EVPCOOL_USERIF_NC7__IMAGE_NUMBER {

	EVPCOOL_USERIF_NC7__IN__MENU_BTN        	   =  0,
	EVPCOOL_USERIF_NC7__IN__BACK_BTN  	    	   =  1,
	EVPCOOL_USERIF_NC7__IN__HELP_BTN  	    	   =  2,

	EVPCOOL_USERIF_NC7__IN__ONOFF_PASV_BTN  	   =  3,
	EVPCOOL_USERIF_NC7__IN__ONOFF_ACTV_BTN         =  4,
	EVPCOOL_USERIF_NC7__IN__MODE_BTN        	   =  5,
	EVPCOOL_USERIF_NC7__IN__ZONES_BTN       	   =  6,

	EVPCOOL_USERIF_NC7__IN__FAN_PASV_BTN      	   =  7,
	EVPCOOL_USERIF_NC7__IN__FAN_ACTV_BTN      	   =  8,
	EVPCOOL_USERIF_NC7__IN__PUMP_PASV_BTN      	   =  9,
	EVPCOOL_USERIF_NC7__IN__PUMP_ACTV_BTN      	   = 10,

	EVPCOOL_USERIF_NC7__IN__PROG_TIME_UP    	   = 11,
	EVPCOOL_USERIF_NC7__IN__PROG_TIME_DN    	   = 12,
	EVPCOOL_USERIF_NC7__IN__PROG_PUMP_PASV_BTN 	   = 13,
	EVPCOOL_USERIF_NC7__IN__PROG_PUMP_ACTV_BTN 	   = 14,
	EVPCOOL_USERIF_NC7__IN__BRIVIS_LOGO            = 15,
	EVPCOOL_USERIF_NC7__IN__CLOCK_ICON             = 16,
	EVPCOOL_USERIF_NC7__IN__FAN_ICON               = 17,
	EVPCOOL_USERIF_NC7__IN__DROP_ICON              = 18,
	EVPCOOL_USERIF_NC7__IN__RIPPLE_ICON            = 19,
	EVPCOOL_USERIF_NC7__IN__AUTO_FAN_ICON          = 20,

	EVPCOOL_USERIF_NC7__IN__MAN_OPN_DROP_1_ICON    = 21,
	EVPCOOL_USERIF_NC7__IN__MAN_OPN_DROP_2_ICON    = 22,
	EVPCOOL_USERIF_NC7__IN__MAN_OPN_DROP_3_ICON    = 23,
	EVPCOOL_USERIF_NC7__IN__MAN_OPN_RIPPLE_ICON    = 24,

	EVPCOOL_USERIF_NC7__IN__AUT_OPN_DROP_1_ICON    = 25,
	EVPCOOL_USERIF_NC7__IN__AUT_OPN_DROP_2_ICON    = 26,
	EVPCOOL_USERIF_NC7__IN__AUT_OPN_DROP_3_ICON    = 27,
	EVPCOOL_USERIF_NC7__IN__AUT_OPN_RIPPLE_ICON    = 28,

	EVPCOOL_USERIF_NC7__IN__PGM_OPN_DROP_1_ICON    = 29,
	EVPCOOL_USERIF_NC7__IN__PGM_OPN_DROP_2_ICON    = 30,
	EVPCOOL_USERIF_NC7__IN__PGM_OPN_DROP_3_ICON    = 31,
	EVPCOOL_USERIF_NC7__IN__PGM_OPN_RIPPLE_ICON    = 32,

	EVPCOOL_USERIF_NC7__IN__ALERT_ICON			   = 33

} EVPCOOL_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails		mkaImage[] = {
	//							    Image - Passive,					           Image - Active,   X,   Y, 						      Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__MENU_BTN    }, // 0
	{  UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__BACK_BTN    }, // 1
	{  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__HELP_BTN    }, // 2
#else
	{  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__MENU_BTN    }, // 0
	{  UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__BACK_BTN    }, // 1
	{  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE,  UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__HELP_BTN    }, // 2
#endif
	// - Control
	{ UITFTDSP_IMAGE__IN__COMMON__ONOFF_BTN_PASSIVE, 					  UITFTDSP_IMAGE__NO_IMAGE,  15,  55, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__ONOFF_BTN  }, // 3
	{  UITFTDSP_IMAGE__IN__COMMON__ONOFF_BTN_ACTIVE, 					  UITFTDSP_IMAGE__NO_IMAGE,  15,  55, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__ONOFF_BTN  }, // 4
	{  UITFTDSP_IMAGE__IN__COMMON__MODE_BTN_PASSIVE,   UITFTDSP_IMAGE__IN__COMMON__MODE_BTN_ACTIVE,  15, 124, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__MODE_BTN   }, // 5
	{  UITFTDSP_IMAGE__IN__COMMON__ZONE_BTN_PASSIVE,   UITFTDSP_IMAGE__IN__COMMON__ZONE_BTN_ACTIVE,  15, 197, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__ZONES_BTN  }, // 6
	// - Normal Operation
	{  UITFTDSP_IMAGE__IN__EVPCOOL__FAN_BTN_PASSIVE,  					  UITFTDSP_IMAGE__NO_IMAGE, 278,  77, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__FAN_BTN    }, // 7
	{   UITFTDSP_IMAGE__IN__EVPCOOL__FAN_BTN_ACTIVE,  				      UITFTDSP_IMAGE__NO_IMAGE, 278,  77, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__FAN_BTN    }, // 8
	{ UITFTDSP_IMAGE__IN__EVPCOOL__PUMP_BTN_PASSIVE, 					  UITFTDSP_IMAGE__NO_IMAGE, 339,  77, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__PUMP_BTN   }, // 9
	{  UITFTDSP_IMAGE__IN__EVPCOOL__PUMP_BTN_ACTIVE, 					  UITFTDSP_IMAGE__NO_IMAGE, 339,  77, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__PUMP_BTN   }, // 10
	// - Programming
	{  UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE,   UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE, 425, 120, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__TIME_UP    }, // 11
	{  UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE,   UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE, 375, 120, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__TIME_DN    }, // 12
	{ UITFTDSP_IMAGE__IN__EVPCOOL__PUMP_BTN_PASSIVE, 					  UITFTDSP_IMAGE__NO_IMAGE, 166,  65, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__PUMP_BTN   }, // 13
	{  UITFTDSP_IMAGE__IN__EVPCOOL__PUMP_BTN_ACTIVE, 					  UITFTDSP_IMAGE__NO_IMAGE, 166,  65, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, EVPCOOL_USERIF_NC7__TV__PUMP_BTN  	}, // 14
	// - General
#if defined(USE_BRIVIS_LOGO)
	{  		UITFTDSP_IMAGE__IN__COMMON__BRIVIS_LOGO,         			  UITFTDSP_IMAGE__NO_IMAGE, 438, 230, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1									}, // 15
#else
	{  		UITFTDSP_IMAGE__IN__COMMON__BRIVIS_LOGO,         			  UITFTDSP_IMAGE__NO_IMAGE, 438, 242, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1									}, // 15
#endif
	{ 		 UITFTDSP_IMAGE__IN__COMMON__CLOCK_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 357, 220, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 16
	{ 		   UITFTDSP_IMAGE__IN__COMMON__FAN_ICON,             		  UITFTDSP_IMAGE__NO_IMAGE, 285, 130, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 17
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 355, 130, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 18
	{ 	   UITFTDSP_IMAGE__IN__EVPCOOL__RIPPLE_ICON,         			  UITFTDSP_IMAGE__NO_IMAGE, 335, 202, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 19
	{ 		   UITFTDSP_IMAGE__IN__COMMON__FAN_ICON,             		  UITFTDSP_IMAGE__NO_IMAGE, 283,  77, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 20
	// - Water Drops : Manual Operation
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,          			  UITFTDSP_IMAGE__NO_IMAGE, 355, 130, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 21
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 355, 151, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 22
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 355, 172, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 23
	{ 	   UITFTDSP_IMAGE__IN__EVPCOOL__RIPPLE_ICON,               	      UITFTDSP_IMAGE__NO_IMAGE, 337, 193, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 24
	// - Water Drops : Auto Operation
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,          			  UITFTDSP_IMAGE__NO_IMAGE, 294, 130, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 							   	}, // 25
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 294, 151, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 26
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 294, 172, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 27
	{ 	   UITFTDSP_IMAGE__IN__EVPCOOL__RIPPLE_ICON,               	      UITFTDSP_IMAGE__NO_IMAGE, 276, 193, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 28
	// - Water Drops : Program to switch on
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,          			  UITFTDSP_IMAGE__NO_IMAGE, 182, 117, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 							   	}, // 29
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 182, 138, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 30
	{ 		 UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON,           		      UITFTDSP_IMAGE__NO_IMAGE, 182, 159, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 31
	{ 	   UITFTDSP_IMAGE__IN__EVPCOOL__RIPPLE_ICON,               	      UITFTDSP_IMAGE__NO_IMAGE, 164, 180, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, -1 								}, // 32

	{ 		  UITFTDSP_IMAGE_COMMON__IN__ALERT_ICON,            		  UITFTDSP_IMAGE__NO_IMAGE, 440, 165, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,  EVPCOOL_USERIF_NC7__TV__ALERT_BTN }, // 33
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	EVPCOOL_USERIF_NC7__BUTTON_NUMBER {

	EVPCOOL_USERIF_NC7__BN__MAN_PASV_BTN      =  0,
	EVPCOOL_USERIF_NC7__BN__MAN_ACTV_BTN      =  1,
	EVPCOOL_USERIF_NC7__BN__AUT_PASV_BTN      =  2,
	EVPCOOL_USERIF_NC7__BN__AUT_ACTV_BTN      =  3,
	EVPCOOL_USERIF_NC7__BN__TIMER_BTN         =  4,
	EVPCOOL_USERIF_NC7__BN__DONE_BTN          =  5,
	EVPCOOL_USERIF_NC7__BN__RESET_BTN         =  6,
	EVPCOOL_USERIF_NC7__BN__PROG_MAN_PASV_BTN =  7,
	EVPCOOL_USERIF_NC7__BN__PROG_MAN_ACTV_BTN =  8,
	EVPCOOL_USERIF_NC7__BN__PROG_AUT_PASV_BTN =  9,
	EVPCOOL_USERIF_NC7__BN__PROG_AUT_ACTV_BTN = 10

} EVPCOOL_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton[] = {
		// BUTTON																										  				      		           RECTANGLE						      TEXT
		//                Normal State,    HL,                              Persist time, 					     Style,	                          	   Tag,	      Color,    X,   Y,   W,   H,  LW  	      X,   Y,  XO,  YO, Color,          		  FontSize,		  	    Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, EVPCOOL_USERIF_NC7__TV__MANUAL_BTN, {  {      0,  112, 218,  74,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Manual" } } },	 //  0
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, EVPCOOL_USERIF_NC7__TV__MANUAL_BTN, {  {      0,  112, 218,  74,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Manual" } } },	 //  1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,   EVPCOOL_USERIF_NC7__TV__AUTO_BTN, {  {      0,  195, 218,  74,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Auto" 	} } },	 //  2
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,   EVPCOOL_USERIF_NC7__TV__AUTO_BTN, {  {      0,  195, 218,  74,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Auto" 	} } },	 //  3
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  EVPCOOL_USERIF_NC7__TV__TIMER_BTN, {  {      0,  278, 218,  74,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Timer" 	} } },	 //  4
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,   EVPCOOL_USERIF_NC7__TV__DONE_BTN, {  {      0,  380, 233,  90,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Done" 	} } },	 //  7
	{	UITFTDSP_BUTTON__STATE_PASSIVE,  true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  EVPCOOL_USERIF_NC7__TV__RESET_BTN, {  {      0,  405,  97,  69,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Reset" 	} } },	 //  8
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, EVPCOOL_USERIF_NC7__TV__MANUAL_BTN, {  {      0,  220,  65, 120,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Manual" } } },	 //  9
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, EVPCOOL_USERIF_NC7__TV__MANUAL_BTN, {  {      0,  220,  65, 120,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Manual" } } },	 //  10
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,   EVPCOOL_USERIF_NC7__TV__AUTO_BTN, {  {      0,  350,  65, 120,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Auto" 	} } },	 //  11
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,   EVPCOOL_USERIF_NC7__TV__AUTO_BTN, {  {      0,  350,  65, 120,  26,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, "Auto" 	} } },	 //  12
} ;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaText[]
typedef enum	EVPCOOL_USERIF_NC7__TEXT_NUMBER {

	EVPCOOL_USERIF_NC7__TN__EVAP 		 = 0,
	EVPCOOL_USERIF_NC7__TN__OFFTIME      = 1,
	EVPCOOL_USERIF_NC7__TN__ONTIME       = 2,
	EVPCOOL_USERIF_NC7__TN__TIME_ON_OFF  = 3,
	EVPCOOL_USERIF_NC7__TN__ATTENTION	=  4

} EVPCOOL_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails				mkaText[] = {
	//   X,   Y,  X0,  Y0, 			        Color,          	      FontSize,    	          Justification, String Label
	{  430,  58,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,     UITFTDSP_TEXT__TJ__LEFT, "EVAP" 							},  // 0
	{  220,  98,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,     UITFTDSP_TEXT__TJ__LEFT, "Set Off Time" 					},  // 1
	{  220,  98,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,     UITFTDSP_TEXT__TJ__LEFT, "Set On Time" 						},  // 2
	{  220, 170,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,     UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrTimeProgrammedText[0]	},  // 3
	{  454, 200,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__CENTER, "Alert"    						}	// 4
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Bargraph Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaBarGraph[]
typedef enum	EVPCOOL_USERIF_NC7__BARGRAPH_NUMBER {

	EVPCOOL_USERIF_NC7__BG__FAN             = 0,
	EVPCOOL_USERIF_NC7__BG__COMFORT         = 1,
	EVPCOOL_USERIF_NC7__BG__PROG_FAN        = 2,
	EVPCOOL_USERIF_NC7__BG__PROG_COMFORT    = 3

} EVPCOOL_USERIF_NC7__BARGRAPH_NUMBER;
//
// Array of bargraphs
static const UI_TFTSHAPE__BarGraphDetails		mkaBarGraph[] = {
				// RECTANGLE
	// FS Type, 	   				     Color,   X,   Y,   W,   H,  LW
	{	  true,	{ UITFTDSP_COLOR__BUTTON_OUTER, 111,  77,  78, 129,  18 } },
	{	 false,	{ UITFTDSP_COLOR__BUTTON_OUTER, 111,  77,  78, 129,  18 } },
	{	  true,	{ UITFTDSP_COLOR__BUTTON_OUTER,  14,  62,  78, 129,  18 } },
	{	 false,	{ UITFTDSP_COLOR__BUTTON_OUTER,  14,  62,  78, 129,  18 } }
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Combined Level Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaLevelButtons[]
typedef enum	EVPCOOL_USERIF_NC7__LEVEL_BUTTON_NUMBER {

	EVPCOOL_USERIF_NC7__LB__FAN_BTN              = 0,
	EVPCOOL_USERIF_NC7__LB__COMFORT_BTN          = 1,
	EVPCOOL_USERIF_NC7__LB__PROG_FAN_BTN         = 2,
	EVPCOOL_USERIF_NC7__LB__PROG_COMFORT_BTN     = 3

} EVPCOOL_USERIF_NC7__LEVEL_BUTTON_NUMBER;
//
// Array of combined level buttons
static const UITFTDSP_BUTTON__LevelButtonDetails 	mkaLevelButtons[] =
{
	// FAN LEVEL
	{
		// RECTANGLE						   	  				     TEXT1																									  		             TEXT2
		// 	   					   Color,   X,   Y,   W,   H,  LW 	     X,   Y,  XO,  YO, 						         Color,          		   FontSize,		  	 Justification, Text		 X,   Y,  XO,  YO, Color,          		     FontSize,		       Justification, 	Text
		{ { UITFTDSP_COLOR__BUTTON_OUTER, 196,  77,  56, 129,  18 }, {  -1,  -1,  -1,  -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Fan" }, {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, 	COM_DEFN__STRING_NULL	} },
		// TOP BUTTON
		// 				  				ImageNumber, 					 ImageNumber_ActivePress,  	  X,   Y, 						Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_UP	},
		// BOTTOM BUTTON
		// 				  				ImageNumber, 					 ImageNumber_ActivePress,  	  X,   Y, 						Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_DN 	},
		// POINT
		// Color,	X,	 Y,	XO1, YO1, XO2, YO2, PointSize
		{ 	   0,  -1,  -1,  -1,  -1,  -1,  -1, 0 		}
	},

	// COMFORT LEVEL
	{
		// RECTANGLE						   	  				     TEXT1																									  		                 TEXT2
		// 	   					   Color,   X,   Y,   W,   H,  LW 	     X,   Y,  XO,  YO, 						         Color,          		   FontSize,		  	 Justification, Text		     X,   Y,  XO,  YO, Color,          		     FontSize,		       Justification, 	Text
		{ { UITFTDSP_COLOR__BUTTON_OUTER, 196,  77,  56, 129,  18 }, {  -1,  -1,  -1,  49, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Comfort" }, {  -1,  -1,  -1,  61,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, 	"Level"	} },
		// TOP BUTTON
		// 				  				ImageNumber, 					 ImageNumber_ActivePress,  	  X,   Y, 						Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_UP	},
		// BOTTOM BUTTON
		// 				  				ImageNumber, 					 ImageNumber_ActivePress,  	  X,   Y, 						Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_DN 	},
		// POINT
		// Color,	X,	 Y,	XO1, YO1, XO2, YO2, PointSize
		{ 	   0,  -1,  -1,  -1,  -1,  -1,  -1, 0 		}
	},

	// FAN LEVEL PROGRAMMING
	{
		// RECTANGLE						   	  				     TEXT1																									  		             TEXT2
		// 	   					   Color,   X,   Y,   W,   H,  LW 	     X,   Y,  XO,  YO, 						         Color,          		   FontSize,		  	 Justification, Text	     X,   Y,  XO,  YO, Color,          		     FontSize,		       Justification, 	Text
		{ { UITFTDSP_COLOR__BUTTON_OUTER, 100,  62,  56, 129,  18 }, {  -1,  -1,  -1,  -1, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Fan" }, {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, 	COM_DEFN__STRING_NULL	} },
		// TOP BUTTON
		// 				  				   ImageNumber, 					   ImageNumber_ActivePress,   X,   Y, 						   Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_UP	},
		// BOTTOM BUTTON
		// 				  			  	   ImageNumber, 					   ImageNumber_ActivePress,   X,   Y, 						   Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_DN 	},
		// POINT
		// Color,	X,	 Y,	XO1, YO1, XO2, YO2, PointSize
		{ 	   0,  -1,  -1,  -1,  -1,  -1,  -1, 0 		}
	},

	// COMFORT LEVEL PROGRAMMING
	{
		// RECTANGLE						   	  				     TEXT1																									  		                 TEXT2
		// 	   					   Color,   X,   Y,   W,   H,  LW 	     X,   Y,  XO,  YO, 						         Color,          		   FontSize,		  	 Justification, Text	         X,   Y,  XO,  YO, Color,          		     FontSize,		       Justification, 	Text
		{ { UITFTDSP_COLOR__BUTTON_OUTER, 100,  62,  56, 129,  18 }, {  -1,  -1,  -1,  49, UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, "Comfort" }, {  -1,  -1,  -1,  61,     0, UITFTDSP_FONTS__FS__ROUND_1, UITFTDSP_TEXT__TJ__CENTER, 	"Level"	} },
		// TOP BUTTON
		// 				  				   ImageNumber, 					   ImageNumber_ActivePress,   X,   Y, 						   Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_UP	},
		// BOTTOM BUTTON
		// 				  				   ImageNumber, 					   ImageNumber_ActivePress,   X,   Y, 						   Persistence time,	TAG
		{ UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE,    UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE,	 -1,  -1, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW,	EVPCOOL_USERIF_NC7__TV__LEVEL_DN 	},
		// POINT
		// Color,	X,	 Y,	XO1, YO1, XO2, YO2, PointSize
		{ 	   0,  -1,  -1,  -1,  -1,  -1,  -1, 0 		}
	}
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle with text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaRectangleWithText[]
typedef enum	EVPCOOL_USERIF_NC7__RECTANGLE_WITH_TEXT_NUMBER {

	EVPCOOL_USERIF_NC7__RWTN__PROG_TIME  = 0,

} EVPCOOL_USERIF_NC7__RECTANGLE_WITH_TEXT_NUMBER;
//
// Array of rectangles with text entry
static const UI_TFTSHAPE__RectangleWithTextDetails		mkaRectangleWithText[] = {

	// Rectangle											 Text
	// 			         Color,    X,   Y,   W,   H,  LW        X,   Y,  XO,  YO,                  Color,          	         FontSize,              Justification,  Text to display
	{  { UITFTDSP_COLOR__WHITE,  220, 122, 145,  40,  8 },   { -1,  -1,  -1,  -1,  UITFTDSP_COLOR__BLACK, UITFTDSP_FONTS__FS__ROUND_3,  UITFTDSP_TEXT__TJ__CENTER,  (char*)&mstrTimeText[0]  } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle with text details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaRectangle[]
typedef enum	EVPCOOL_USERIF_NC7__RECTANGLE_NUMBER {

	EVPCOOL_USERIF_NC7__RN__ZONE_BLANK = 0

} EVPCOOL_USERIF_NC7__RECTANGLE_NUMBER;
//
// Array of rectangles
static const UI_TFTSHAPE__Rectangle						mkaRectangle[] = {

	// Rectangle
	// 			                   Color,    X,   Y,   W,   H,  LW
	{  UITFTDSP_COLOR__BUTTON_INNER_PASSIVE,   25, 240,  41,  11,  4 },
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	EVPCOOL_USERIF_NC7__WaterDropDetails {									// Water drop animation control
																						//
	EVPCOOL_USERIF_NC7__WATER_DROP_NUMBER	CurrentlyBeingDisplayed;    				// - This holds the actual water drop that is currently being displayed.
	GENTIMDT__TimingCounter					DelayBeforeNextIconDisplayTimingCounter;   	// - The counter used to time the intervals between updates
																						//
} EVPCOOL_USERIF_NC7__WaterDropDetails;

typedef struct	EVPCOOL_USERIF_NC7__UIDetails {											// Evap Cooling UI Specific
																						//
	EVPCOOL_USERIF_NC7__WaterDropDetails	WaterDrop;									// - Water drop control

} EVPCOOL_USERIF_NC7__UIDetails;

static EVPCOOL_USERIF_NC7__UIDetails			mEvpCool_NC7UI;
static NWS_SVCINF_COMMON__ServiceInfoDetails	mServiceInfo;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	EVPCOOL_USERIF_NC7__kWaterDropDelayIconDisplayTiming = { &mEvpCool_NC7UI.WaterDrop.DelayBeforeNextIconDisplayTimingCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void						UpdateDisplay_Mode_NormalOperation(EVPCOOL_COMMON__ControlDetails* pEvpCool);
static void						UpdateStatusDisplayIndicators(EVPCOOL_COMMON__ControlDetails* pEvpCool);

static void						UpdateDisplay_SetupBottomStatus(EVPCOOL_COMMON__ControlDetails* pEvpCool);
static void						UpdateOnOffTimeText(RTMCLKIF__ShortTime* pTime, EVPCOOL_COMMON__ControlDetails* pEvpCool);
static void						UpdateWaterDropAnimation(EVPCOOL_USERIF_NC7__IMAGE_NUMBER vDrop1Image);

static void						UpdateServiceDetailsStatus(EVPCOOL_COMMON__ControlDetails* pEvpCool);
static bool						bViewingServiceInfoPrompt(EVPCOOL_COMMON__ControlDetails* pEvpCool);

static void						RapidUpdateControl_None(void);
static void						RapidUpdateControl_Time(void);
static void						RapidUpdateControl_FanSpeedLevel(bool vbIncreasingValue, TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel);
static void						RapidUpdateControl_ComfortLevel(bool vbIncreasingValue, uint8_t vucComfortLevel);
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
void							EVPCOOL_USERIF_NC7__Initialise(void)
{
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User display update - N-C7 : Programming Mode.
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							EVPCOOL_USERIF_NC7__UpdateDisplay_Mode_Programming(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	UICOMMON__TopStatusDetails	TopStatus;
	RTMCLKIF__ShortTime			Time;
	bool						bZoningDoneButtonPressed = false, bZoningBackButtonPressed = false;
    //
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	//
	// Do not show bottom status in this mode  
	UICOMMON__ShowBottomStatus( false );
	//
	// Check if zoning screen is being accessed
	if ( pEvpCool->UserAccess.bZoningDetails )
	{
		// Accessing zoning details to be programmed
		// - Display update
		ZONING_CONTROL__UpdateDisplay_ProgrammingZones();
		// - Control update
		ZONING_CONTROL__UpdateControl_ProgrammingZones( &bZoningDoneButtonPressed, &bZoningBackButtonPressed );
		// - Zone list update if user has accepted change
		if ( bZoningDoneButtonPressed )		// Done - Update changes and get out
		{
			ZONING_CONTROL__DefineFromUpdate_ZoneStateList( &pEvpCool->Programmed.ucZoneList );

			pEvpCool->UserAccess.bZoningDetails = false;
		}
		else
		if ( bZoningBackButtonPressed )		// Done - No changes
		{
			pEvpCool->UserAccess.bZoningDetails = false;
		}
		return;
	}
	//
	// IF HERE - NORMAL DISPLAY UPDATE
	//
	// Done button
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__DONE_BTN] );
	//
    // Top status area
	// - Initialize structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define required images
	TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__BACK_BTN];
#if defined(INCLUDE_HELP_MENU_OPTION)
	TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__HELP_BTN];
#endif
	if ( pEvpCool->Requested.Operation == TWBCMDSQ__OPN__SWITCHOFF )
	{
		// Top status description
		TopStatus.pstrTextToDisplay = (char*)mkstrTopStatusText_ProgTimerOn;
		//
		// Progran to switch on
		// - Define the time to be displayed - Switch on time
		Time = pEvpCool->Programmed.SwitchOn.ShortTime;
		// - Indicate on time
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[EVPCOOL_USERIF_NC7__TN__ONTIME] );
    	//
		// Programmed switch on in auto/manual control
		if ( pEvpCool->Programmed.bSwitchOnInManualOperation )
		{
			// Fan speed level
			UITFTDSP__ShowBarGraph( pEvpCool->Programmed.SwitchOnFanSpeedLevel, (UI_TFTSHAPE__BarGraphDetails*)&mkaBarGraph[EVPCOOL_USERIF_NC7__BG__PROG_FAN] );
			UITFTDSP__ShowLevelButtons( (UITFTDSP_BUTTON__LevelButtonDetails*)&mkaLevelButtons[EVPCOOL_USERIF_NC7__LB__PROG_FAN_BTN] );
			//
			// Program manual control
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__PROG_MAN_ACTV_BTN] );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__PROG_AUT_PASV_BTN] );
			//
			// Programmed pump state to switch on
	    	if ( pEvpCool->Programmed.oSwitchOnPumpState )
	    	{
	    		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__PROG_PUMP_ACTV_BTN] );
	    		UpdateWaterDropAnimation( EVPCOOL_USERIF_NC7__IN__PGM_OPN_DROP_1_ICON );
	    	}
	    	else
	    	{
	    		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__PROG_PUMP_PASV_BTN] );
	    	}
		}
		else
		{
			// Comfort level
			UITFTDSP__ShowBarGraph( pEvpCool->Programmed.ucSwitchOnSetTemperature - EVP_COOL_COMMON__SET_TEMP_MIN, (UI_TFTSHAPE__BarGraphDetails*)&mkaBarGraph[EVPCOOL_USERIF_NC7__BG__PROG_COMFORT] );
			UITFTDSP__ShowLevelButtons( (UITFTDSP_BUTTON__LevelButtonDetails*)&mkaLevelButtons[EVPCOOL_USERIF_NC7__LB__PROG_COMFORT_BTN] );
			//
	    	// Program auto control
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__PROG_MAN_PASV_BTN] );
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__PROG_AUT_ACTV_BTN] );
		}
		//
		// Zone control - Only display if at least one switched zone is installed
		if ( ZONING_CONTROL__bAtLeastOneSwitchedZoneIsInstalled() )
	    {
			UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__ZONES_BTN] );
	    }
	}
	else
	{
		// Top status description
		TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_ProgTimerOff;
		//
		// Progran to switch off
		// - Define the time to be displayed - Switch off time
		Time = pEvpCool->Programmed.SwitchOff.ShortTime;
		// Indicate off time
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[EVPCOOL_USERIF_NC7__TN__OFFTIME] );
	}
	//
	// Time up/down buttons
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__PROG_TIME_UP] );
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__PROG_TIME_DN] );
	//
	// Write the on/off time + today/tomorrow on the bottom of the screen
	UpdateOnOffTimeText( &Time, pEvpCool );
	//
	// Display the time in the rectangle
	// Form time in string
	// - Init
	GENSTRDT__InitString( EVPCOOL_USERIF_NC7__TIME_TEXT_LENGTH, &mstrTimeText[0], &mTimeString );
	// - Add time
	UICOMMON__AssignTimeToString( TWBIFUTL__bClockDisplayFormatIn12HourFormat(), Time.ucHour, Time.ucMinute, &mTimeString );
	//
	// Display text in rectangle
	UITFTDSP__ShowRectangleWithText( (UI_TFTSHAPE__RectangleWithTextDetails*)&mkaRectangleWithText[EVPCOOL_USERIF_NC7__RWTN__PROG_TIME] );
	//
	// Yop status update
	UICOMMON__TopStatus_Update( &TopStatus );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Programming Mode.
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							EVPCOOL_USERIF_NC7__UpdateControl_Mode_Programming(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	//
	// Check if zoning screen is being accessed
	if (pEvpCool->UserAccess.bZoningDetails) 	{ return; }
	//
	// IF HERE - NORMAL CONTROL UPDATE
	//
	ucTagValue = TFTDSPIF__ucTouchTagValue();	// Latest tag value of any screen press ( 0 = no press)

	switch ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
        case EVPCOOL_USERIF_NC7__TV__MANUAL_BTN:
	    	// Manual button - change to Manual mode
    	    if ( !pEvpCool->Programmed.bSwitchOnInManualOperation )
    	    {
                pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_AUTO_MANUAL;
    		}
			// No rapid update
    	    RapidUpdateControl_None();
            break;

    	case EVPCOOL_USERIF_NC7__TV__AUTO_BTN:
    		// Auto button - change to Auto mode
        	if ( pEvpCool->Programmed.bSwitchOnInManualOperation )
    	    {
                pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_AUTO_MANUAL;
    	    }
			// No rapid update
        	RapidUpdateControl_None();
    	    break;

    	case EVPCOOL_USERIF_NC7__TV__TIME_UP:
    		// Time up button - increase Time by 1 minute
    		pEvpCool->UserAccess.Control.iTimeAdjust_Minutes = 1;
    		// Allow rapid update
    		RapidUpdateControl_Time();
    		break;

    	case EVPCOOL_USERIF_NC7__TV__TIME_DN:
    		// Time down button - decrease Time by 1 minute
    		pEvpCool->UserAccess.Control.iTimeAdjust_Minutes = -1;
    		// Allow rapid update
    		RapidUpdateControl_Time();
    		break;

    	case EVPCOOL_USERIF_NC7__TV__LEVEL_UP:
    		// Level up button - increase fan or comfort level by 1
    	    if ( pEvpCool->Programmed.bSwitchOnInManualOperation )
    	   	{
        		pEvpCool->UserAccess.Control.iFanSpeedLevelAdjust = 1;

    	    	RapidUpdateControl_FanSpeedLevel( true, pEvpCool->Programmed.SwitchOnFanSpeedLevel );
    	   	}
    	    else
    	    {
        		pEvpCool->UserAccess.Control.iComfortLevelAdjust = 1;

        		RapidUpdateControl_ComfortLevel( false, pEvpCool->Programmed.ucSwitchOnSetTemperature );
    	    }
    		break;

    	case EVPCOOL_USERIF_NC7__TV__LEVEL_DN:
    		// Level down button - decrease fan or comfort level by 1
    		if ( pEvpCool->Programmed.bSwitchOnInManualOperation )
    		{
        		pEvpCool->UserAccess.Control.iFanSpeedLevelAdjust = -1;

    			RapidUpdateControl_FanSpeedLevel( false, pEvpCool->Programmed.SwitchOnFanSpeedLevel );
    		}
    		else
    		{
        		pEvpCool->UserAccess.Control.iComfortLevelAdjust = -1;

        		RapidUpdateControl_ComfortLevel( true, pEvpCool->Programmed.ucSwitchOnSetTemperature );
    		}
    	    break;

    	case EVPCOOL_USERIF_NC7__TV__PUMP_BTN:
    		// Pump button - toggle state
            pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__TOGGLE_PUMP_STATE;
			// No rapid update
    	    RapidUpdateControl_None();
            break;

    	case EVPCOOL_USERIF_NC7__TV__BACK_BTN:
    	case EVPCOOL_USERIF_NC7__TV__DONE_BTN:
    	case EVPCOOL_USERIF_NC7__TV__ZONES_BTN:
   		case EVPCOOL_USERIF_NC7__TV__HELP_BTN:
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
			switch ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
	    		case EVPCOOL_USERIF_NC7__TV__BACK_BTN:
	    			pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__PROG_DEACTIVATE_AND_EXIT;
	    			break;

	    		case EVPCOOL_USERIF_NC7__TV__DONE_BTN:
	    			pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__PROG_ACTIVATE_AND_EXIT;
	    			break;

	    		case EVPCOOL_USERIF_NC7__TV__ZONES_BTN:
	    			// Zone button
	    			// - Initialise before access
	    			ZONING_CONTROL__InitialiseUIBeforeFirstAccess( pEvpCool->Programmed.ucZoneList );
	    			// - Access zoning details
	    			pEvpCool->UserAccess.bZoningDetails = true;
	    			break;

	    		case EVPCOOL_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	User display update - N-C7 : Normal Operating Mode (Off).
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							EVPCOOL_USERIF_NC7__UpdateDisplay_Mode_Off(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	// Service info prompt?
	if ( bViewingServiceInfoPrompt(pEvpCool) )	{ return; }
	//
	// Reset water drop animation details
	// - Start at drop 1
	mEvpCool_NC7UI.WaterDrop.CurrentlyBeingDisplayed = EVPCOOL_USERIF_NC7__WDN__DROP_1;
	// - Start delay timer
	GENTIMDT__StartTimer_TB_UIBased( EVPCOOL_USERIF_NC7__WATER_DROP_DISPLAY_TIME_TBUI, &mEvpCool_NC7UI.WaterDrop.DelayBeforeNextIconDisplayTimingCounter );
	//
	// Normal Operation - Common Display
	UpdateDisplay_Mode_NormalOperation( pEvpCool );

	if ( pEvpCool->bSlaveModeSwitch ) { return; }
	//
	// Mode - SwitchOff Related
	// - Temperature to display
	if ( pEvpCool->bDisplayRoomTemperature )
    {
		if ( TEMPMSIF__bTemperatureMeasurementThermistorIsFaulty() )
		{
            // Thermistor faulty so show "XX"
			UICOMMON__ShowRoomTemperature( 255 );
		}
		else
		{
            // Thermistor ok so show temperature
			UICOMMON__ShowRoomTemperature( TEMPMSIF__ucMeasuredTemperature( TWBIFUTL__bTemperatureDisplayFormatInCelsius() ) );
		}
    }
	// - Switch on check enabled
	if ( pEvpCool->Programmed.bSwitchOnCheckEnabled )
	{
		UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__CLOCK_ICON] );
	}
    // - Images : No button press

	// - Images : Associated with button presses
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__ONOFF_PASV_BTN] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Normal Operating Mode (Off).
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							EVPCOOL_USERIF_NC7__UpdateControl_Mode_Off(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }

	ucTagValue = TFTDSPIF__ucTouchTagValue();	// Latest tag value of any screen press ( 0 = no press)

	if ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue == EVPCOOL_USERIF_NC7__TV__MENU_BTN )			// Allow menu button always
	{
		sucActiveTag = ucTagValue; RapidUpdateControl_None();
	}
	else
	if ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue == EVPCOOL_USERIF_NC7__TV__HELP_BTN )			// Allow help button always
	{
		sucActiveTag = ucTagValue; RapidUpdateControl_None();
	}
	else
	if ( !NWSUTILS__bUserControlIsLockedOut() )														// Allow remaining only if not locked out
	{
		switch ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue )
		{
			case EVPCOOL_USERIF_NC7__TV__ONOFF_BTN:
				// On / Off button - change to On mode
	    		pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_SYS_ON;
				// No rapid update
	    	    RapidUpdateControl_None();
				break;

			case EVPCOOL_USERIF_NC7__TV__MODE_BTN:
			case EVPCOOL_USERIF_NC7__TV__ALERT_BTN:
	   		case EVPCOOL_USERIF_NC7__TV__TIMER_BTN:
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
			switch ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case EVPCOOL_USERIF_NC7__TV__ALERT_BTN:
					NWS_SVCINF_CONTROL__ServiceInfoPrompt_FlagToShowIt( &mServiceInfo );
					break;

				case EVPCOOL_USERIF_NC7__TV__MODE_BTN:
					pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_MODE;
					break;

				case EVPCOOL_USERIF_NC7__TV__TIMER_BTN:
		    		pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__ENABLE_PROG_MODE_ACCESS;
					break;

				case EVPCOOL_USERIF_NC7__TV__MENU_BTN:
		   			pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__ACCESS_CONFIG_MENU;
					break;

	    		case EVPCOOL_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	User display update - N-C7 : Normal Operating Mode (On).
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							EVPCOOL_USERIF_NC7__UpdateDisplay_Mode_On(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	bool		bZoningDoneButtonPressed = false, bZoningBackButtonPressed = false;
	//
	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) 					{ return; }
	// Service info prompt?
	if ( bViewingServiceInfoPrompt(pEvpCool) )	{ return; }
	//
	// Check if zoning screen is being accessed
	if ( pEvpCool->UserAccess.bZoningDetails )
	{
		// Do not show bottom status in zoning screen 
		UICOMMON__ShowBottomStatus( false );
		//
		// Accessing zoning details to be programmed
		// - Display update
		ZONING_CONTROL__UpdateDisplay_InstalledAndEnabledZones();
		// - Control update
		ZONING_CONTROL__UpdateControl_InstalledAndEnabledZones( &bZoningDoneButtonPressed, &bZoningBackButtonPressed );
		// - Zone list update if user has accepted change
		if ( bZoningDoneButtonPressed )		// Done - Update changes and get out
		{
			uint8_t		ucZoneStateList = 0;

			ZONING_CONTROL__DefineFromUpdate_ZoneStateList( &ucZoneStateList ); ZONING_CONTROL__SetUserEnabledZoneStateList( ucZoneStateList );

			pEvpCool->UserAccess.bZoningDetails = false;
		}
		else
		if ( bZoningBackButtonPressed )		// Done - No changes
		{
			pEvpCool->UserAccess.bZoningDetails = false;
		}
		return;
	}
	//
	// IF HERE - NORMAL DISPLAY UPDATE
	//
	// Normal Operation - Common Display
	UpdateDisplay_Mode_NormalOperation( pEvpCool );

	if ( pEvpCool->bSlaveModeSwitch || UICOMMON__bMasterModeSwitch() ) { return; }
	//
	// Mode - SwitchOff Related
	if ( pEvpCool->Programmed.bSwitchOffCheckEnabled )
	{
		UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__CLOCK_ICON] );
	}
    // - Images : No button press

	// - Images : Associated with button presses
	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__ONOFF_ACTV_BTN] );

    if ( ZONING_CONTROL__bAtLeastOneSwitchedZoneIsInstalled() )		// Only display if at least one switched zone is installed
    {
    	UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__ZONES_BTN] );

    	if ( ( ZONING_CONTROL__pZoning()->UserAccess.bFlashIfNoZonesEnabled ) && ( ZONING_CONTROL__pZoning()->UserAccess.ucNumberOfZonesEnabled == 0 ) )
    	{
    		UITFTDSP__FlashRectangle( UITFTDSP__FR__1HZ, (UI_TFTSHAPE__Rectangle*)&mkaRectangle[EVPCOOL_USERIF_NC7__RN__ZONE_BLANK] );
    	}
    }
    // - Auto/Manual distinction
	if ( pEvpCool->bInManualControl )
    {
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__MAN_ACTV_BTN] );
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__AUT_PASV_BTN] );

    	if ( pEvpCool->Requested.oFanState )
    	{
    		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__FAN_ACTV_BTN] );
    	}
    	else
    	{
    		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__FAN_PASV_BTN] );
    	}

    	if ( pEvpCool->Requested.oPumpState )
    	{
    		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__PUMP_ACTV_BTN] );
    	}
    	else
    	{
    		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__PUMP_PASV_BTN] );
    	}
    }
    else
    {
    	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__MAN_PASV_BTN] );
    	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__AUT_ACTV_BTN] );
    }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User control update - N-C7 : Normal Operating Mode (On).
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							EVPCOOL_USERIF_NC7__UpdateControl_Mode_On(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue;

	// Check if cfg menu being accessed - If so no UI update
	if ( NWSUTILS__bAccessingConfigTypeMenu() ) { return; }
	//
	// Check if zoning screen is being accessed
	if ( pEvpCool->UserAccess.bZoningDetails ) 	{ return; }
	//
	// IF HERE - NORMAL CONTROL UPDATE
	//
	ucTagValue = TFTDSPIF__ucTouchTagValue();	// Latest tag value of any screen press ( 0 = no press)

	if ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue == EVPCOOL_USERIF_NC7__TV__MENU_BTN )			// Allow menu button always
	{
		sucActiveTag = ucTagValue; RapidUpdateControl_None();
	}
	else
	if ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue == EVPCOOL_USERIF_NC7__TV__HELP_BTN )			// Allow help button always
	{
		sucActiveTag = ucTagValue; RapidUpdateControl_None();
	}
	else
	if ( !NWSUTILS__bUserControlIsLockedOut() )														// Allow remaining only if not locked out
	{
	    switch ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue )
	    {
	   		case EVPCOOL_USERIF_NC7__TV__ONOFF_BTN:
	   			// On / Off button - change to Off mode
	    		pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_OFF;
				// No rapid update
	    	    RapidUpdateControl_None();
	   			break;

	   		case EVPCOOL_USERIF_NC7__TV__MANUAL_BTN:
	   			// Manual button - change to Manual mode
	    		if ( pEvpCool->Current.Operation == TWBCMDSQ__OPN__SWCHONAUT )
	   		    {
	                pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_AUTO_MANUAL;
	   		    }
				// No rapid update
	    	    RapidUpdateControl_None();
	            break;

	   		case EVPCOOL_USERIF_NC7__TV__AUTO_BTN:
	   			// Auto button - change to Auto mode
	    		if ( pEvpCool->Current.Operation == TWBCMDSQ__OPN__SWCHONMAN )
	   		    {
	                pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_AUTO_MANUAL;
	   		    }
				// No rapid update
	    	    RapidUpdateControl_None();
	    		break;

	    	case EVPCOOL_USERIF_NC7__TV__LEVEL_UP:
	    		// Level up button - increase fan or comfort level by 1
	    		if ( !pEvpCool->bInManualControl )
	    		{
	    			// Comfort level adjust
	    			pEvpCool->UserAccess.Control.iComfortLevelAdjust = 1;

	    			RapidUpdateControl_ComfortLevel( false, pEvpCool->Requested.ucSetTemperature );
	    		}
	    		else
	    		if (pEvpCool->Requested.oFanState == on)
	    		{
	    			// Fan speed adjust
	    			pEvpCool->UserAccess.Control.iFanSpeedLevelAdjust = 1;

	    			RapidUpdateControl_FanSpeedLevel( true, pEvpCool->Requested.FanSpeedLevel );
	    		}
	    		break;

	    	case EVPCOOL_USERIF_NC7__TV__LEVEL_DN:
	   			// Level down button - decrease fan or comfort level by 1
	    		if ( !pEvpCool->bInManualControl )
	    		{
	    			// Comfort level adjust
	    			pEvpCool->UserAccess.Control.iComfortLevelAdjust = -1;

	    			RapidUpdateControl_ComfortLevel( true, pEvpCool->Requested.ucSetTemperature );
	    		}
	    		else
	    		if (pEvpCool->Requested.oFanState == on)
	    		{
	    			// Fan speed adjust
	    			pEvpCool->UserAccess.Control.iFanSpeedLevelAdjust = -1;

	    			RapidUpdateControl_FanSpeedLevel( false, pEvpCool->Requested.FanSpeedLevel );
	    		}
	    	    break;

	   		case EVPCOOL_USERIF_NC7__TV__FAN_BTN:
	   			// Fan button - Switch the fan on/off (Only under manual control)
	    		if ( pEvpCool->bInManualControl )
	   			{
	   				pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__TOGGLE_FAN_STATE;
	   			}
				// No rapid update
	    	    RapidUpdateControl_None();
	   			break;

	   		case EVPCOOL_USERIF_NC7__TV__PUMP_BTN:
	   			// Pump button - Switch the pump on/off
	    		if ( pEvpCool->bInManualControl )
	   			{
	   				pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__TOGGLE_PUMP_STATE;
	   			}
				// No rapid update
	    	    RapidUpdateControl_None();
	   			break;

	   		case EVPCOOL_USERIF_NC7__TV__TIMER_BTN:
	   		case EVPCOOL_USERIF_NC7__TV__ALERT_BTN:
	   		case EVPCOOL_USERIF_NC7__TV__MODE_BTN:
	   		case EVPCOOL_USERIF_NC7__TV__ZONES_BTN:
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
			switch ( (EVPCOOL_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case EVPCOOL_USERIF_NC7__TV__ALERT_BTN:
					NWS_SVCINF_CONTROL__ServiceInfoPrompt_FlagToShowIt( &mServiceInfo );
					break;

				case EVPCOOL_USERIF_NC7__TV__TIMER_BTN:
	   				pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__ENABLE_PROG_MODE_ACCESS;
	   				break;

	   			case EVPCOOL_USERIF_NC7__TV__MODE_BTN:
	   				pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__SWITCH_MODE;
	   				break;

	   			case EVPCOOL_USERIF_NC7__TV__ZONES_BTN:
	   				// Zone EVPCOOL_USERIF_NC7__TV__ZONES_BTN
	   				// - Initialise before access
	   				ZONING_CONTROL__InitialiseUIBeforeFirstAccess( ZONING_CONTROL__ucUserEnabledZoneStateList() );
	   				// - Access zoning details
	   				pEvpCool->UserAccess.bZoningDetails = true;
	   				break;

	   			case EVPCOOL_USERIF_NC7__TV__MENU_BTN:
	   				// Config menu button
	   				pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__ACCESS_CONFIG_MENU;
	   				break;

	    		case EVPCOOL_USERIF_NC7__TV__HELP_BTN:
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
 * @brief:	Common display update under normal operation - N-C7.
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateDisplay_Mode_NormalOperation(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	UICOMMON__TopStatusDetails	TopStatus;
	//
	// Check for mode switch under slave operation
    if ( pEvpCool->bSlaveModeSwitch || UICOMMON__bMasterModeSwitch() )
    {
	    // Update top status area
		// - Initialise structure
		UICOMMON__TopStatus_InitStructure(&TopStatus);
		// - Define what we need to show in this operating mode
		TopStatus.pstrTextToDisplay		= "SWITCHING MODE - PLEASE WAIT";
		TopStatus.TextFlashRate			= UITFTDSP__FR__2HZ;
		TopStatus.bFlashText 			= true;
		// - Commit what is required
		UICOMMON__TopStatus_Update( &TopStatus );

		return;
    }
    //
    // If here no slave mode switch occurring
    // --------------------------------------
    // Update top status area
	// - Initialise structure
    UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define what we need to show in this operating mode
	TopStatus.pClock  			   = RTMCLKIF__pClock();
	TopStatus.bClockIn12HourFormat = TWBIFUTL__bClockDisplayFormatIn12HourFormat();
	// - Outdoor temperature
	TopStatus.pucOutdoorTemperature = NWSUTILS__pucAmbientTemperature();
	// - Define required images
#if defined(INCLUDE_HELP_MENU_OPTION)
	TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__HELP_BTN];
#endif
	TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__MENU_BTN];
	// - Commit what is required
	UICOMMON__TopStatus_Update( &TopStatus );
	// Update bottom status area
	UpdateDisplay_SetupBottomStatus( pEvpCool );
	//
	// TODO - Check if reset can be performed
	if ( pEvpCool->bAllowResetControl )
	{

	}
	// Show Evap Cooling indicator always
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[EVPCOOL_USERIF_NC7__TN__EVAP] );
    // Timer button to enter programming mode
	UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[EVPCOOL_USERIF_NC7__BN__TIMER_BTN] );
    // Images : No button press
	UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__BRIVIS_LOGO] );
	//
	// Check if two or more modes available
	if ( NWSUTILS__bHeatingIsAvailable() )
	{
		// Images : Associated with button presses
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__MODE_BTN] );
	}
    //
    // Update status display indicators here
	UpdateStatusDisplayIndicators( pEvpCool );
	// Service info update
	UpdateServiceDetailsStatus( pEvpCool );

	if ( mServiceInfo.MessageType != NWS_SVCINF_COMMON__MT__NONE )
	{
		// Show alert image
		UITFTDSP__ShowImageWithAssociatedTag( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__ALERT_ICON] );
		// Flash Attention text
		UITFTDSP__FlashTextAbsolute( UITFTDSP__FR__2HZ, (UITFTDSP_TEXT__TextDetails*)&mkaText[EVPCOOL_USERIF_NC7__TN__ATTENTION] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Based on current operation this function updates the state of the status indicators (Fan & Drops).
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateStatusDisplayIndicators(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	switch ( pEvpCool->Current.Operation )
	{
		case TWBCMDSQ__OPN__SWCHONMAN:
	        // Normal operating mode - Manual control
			// - Fan blade state - Flash or rotate?
	        if ( pEvpCool->oOverallBusyState == on )
	        {
	        	// Is at least one cooler operating its fan, emilio 03-12-15
	        	if ( pEvpCool->oOverallFanState == on )
	        	{
		        	UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel( pEvpCool->Requested.FanSpeedLevel, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__FAN_ICON] );
	        	}
	        	else
	            if ( ( pEvpCool->Requested.oFanState == on ) && ( ZONING_CONTROL__bAtLeastOneZoneIsEnabled() ) )
	            {
	            	UITFTDSP__FlashImage( UITFTDSP__FR__1HZ, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__FAN_ICON] );
	            }
	        }
	        else
	        if ( ( pEvpCool->Requested.oFanState == on ) && ( ZONING_CONTROL__bAtLeastOneZoneIsEnabled() ) )
	        {
	        	UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel( pEvpCool->Requested.FanSpeedLevel, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__FAN_ICON] );
	        }
			// - If pump is on drip the drops!
	        if ( pEvpCool->Requested.oPumpState == on )
	        {
	    		UpdateWaterDropAnimation( EVPCOOL_USERIF_NC7__IN__MAN_OPN_DROP_1_ICON );
			}
	        // - Bargraph as fan speed indicator
	        if ( pEvpCool->Requested.oFanState == on )
	        {
	        	UITFTDSP__ShowBarGraph( pEvpCool->Requested.FanSpeedLevel, (UI_TFTSHAPE__BarGraphDetails*)&mkaBarGraph[EVPCOOL_USERIF_NC7__BG__FAN] );
	        	UITFTDSP__ShowLevelButtons( (UITFTDSP_BUTTON__LevelButtonDetails*)&mkaLevelButtons[EVPCOOL_USERIF_NC7__LB__FAN_BTN] );
			}
			break;

		case TWBCMDSQ__OPN__SWCHONAUT:
	        // Normal operating mode - Auto control
			// - Fan blade state - Flash or rotate?
            if ( pEvpCool->oOverallBusyState == on )
			{
	        	// Is at least one cooler operating its fan
	        	if ( pEvpCool->oOverallFanState == on )
	        	{
	        		UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel( pEvpCool->Requested.FanSpeedLevel, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__AUTO_FAN_ICON] );
	        	}
	        	else
	            if ( ZONING_CONTROL__bAtLeastOneZoneIsEnabled() )
	            {
	            	UITFTDSP__FlashImage( UITFTDSP__FR__1HZ, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__AUTO_FAN_ICON] );
	            }
	    	}
			else
	        if ( pEvpCool->oOverallFanState == on )
	        {
	        	UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel( pEvpCool->Requested.FanSpeedLevel, (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[EVPCOOL_USERIF_NC7__IN__AUTO_FAN_ICON] );
	   		}
			// - If overall pump is on drip the drops!
	        if ( pEvpCool->oOverallPumpState == on )
	        {
	    		UpdateWaterDropAnimation( EVPCOOL_USERIF_NC7__IN__AUT_OPN_DROP_1_ICON );
			}
			// - Bargraph as temperature level indicator
	        UITFTDSP__ShowBarGraph( pEvpCool->Requested.ucSetTemperature - EVP_COOL_COMMON__SET_TEMP_MIN, (UI_TFTSHAPE__BarGraphDetails*)&mkaBarGraph[EVPCOOL_USERIF_NC7__BG__COMFORT] );
	        UITFTDSP__ShowLevelButtons( (UITFTDSP_BUTTON__LevelButtonDetails*)&mkaLevelButtons[EVPCOOL_USERIF_NC7__LB__COMFORT_BTN] );
	        break;

		default:
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates bottom status display.
 * @param:	pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateDisplay_SetupBottomStatus(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	// Define what we need to show in this operating mode
	// - Flash spanner for service notification
	if ( pEvpCool->bServiceNotificationRequired )
	{
		UICOMMON__BottomStatus_UpdateImage( UI_COMMON__BSN_SPANNER, true, UITFTDSP__FR__1HZ );
	}
	// - Display heart indicator if in auto switch on mode
	if ( pEvpCool->Current.Operation == TWBCMDSQ__OPN__SWCHONAUT )
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
 * @brief:	Updates On/Off time text on display.
 * @param:	pTime	 - Pointer to the time to display.
 * 			pEvpCool - Pointer to the evap cooling control structure.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateOnOffTimeText(RTMCLKIF__ShortTime* pTime, EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	// Define string
	GENSTRDT__InitString( EVPCOOL_USERIF_NC7__TIME_PROGRAMMED_TEXT_LENGTH, &mstrTimeProgrammedText[0], &mTimeProgrammedString );
	//
	// Switch Off/On label
	if ( pEvpCool->Requested.Operation == TWBCMDSQ__OPN__SWITCHOFF )
    {
		GENSTRDT__AddTextToString( "Switch on: ", &mTimeProgrammedString );
    }
    else
    {
		GENSTRDT__AddTextToString( "Switch off: ", &mTimeProgrammedString );
    }
	//
    // The actual programmed time
    UICOMMON__AssignTimeToString( TWBIFUTL__bClockDisplayFormatIn12HourFormat(), pTime->ucHour, pTime->ucMinute, &mTimeProgrammedString );
	// Tomorrow or today
    if ( RTMCLKIF__bTheCurrentTimeIsGreaterThanOrEqualToTheShortTime(pTime) )
    {
    	GENSTRDT__AddTextToString( " Tomorrow", &mTimeProgrammedString );
    }
    else
    {
    	GENSTRDT__AddTextToString( " Today", &mTimeProgrammedString );
    }
    // Show the texr
    UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[EVPCOOL_USERIF_NC7__TN__TIME_ON_OFF] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Water drop update to animate dripping drops
 * @param:	vDrop1Image	- Drop 1 image number.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateWaterDropAnimation(EVPCOOL_USERIF_NC7__IMAGE_NUMBER vDrop1Image)
{
	if ( mEvpCool_NC7UI.WaterDrop.DelayBeforeNextIconDisplayTimingCounter.ulValue == 0 )
	{
		// Next drop
		if ( mEvpCool_NC7UI.WaterDrop.CurrentlyBeingDisplayed == EVPCOOL_USERIF_NC7__WDN__RIPPLE )
		{
			mEvpCool_NC7UI.WaterDrop.CurrentlyBeingDisplayed = EVPCOOL_USERIF_NC7__WDN__DROP_1;
		}
		else
		{
			mEvpCool_NC7UI.WaterDrop.CurrentlyBeingDisplayed++;
		}
		// Restart display timer
		GENTIMDT__StartTimer_TB_UIBased( EVPCOOL_USERIF_NC7__WATER_DROP_DISPLAY_TIME_TBUI, &mEvpCool_NC7UI.WaterDrop.DelayBeforeNextIconDisplayTimingCounter );
	}
	//
	// Show the current drop - Offset from value passed
	UITFTDSP__ShowImage( (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[vDrop1Image + mEvpCool_NC7UI.WaterDrop.CurrentlyBeingDisplayed] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update service info details variable - mServiceInfo.
 * @param:	pEvpCool - Pointer to the Evap Cooling control details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						UpdateServiceDetailsStatus(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
    if ( NWSUTILS__bAFaultMessageForDisplayExists() )
    {
    	mServiceInfo.MessageType 	 = NWS_SVCINF_COMMON__MT__FAULT_TRANSIENT;
    	mServiceInfo.GroupID     	 = NWSUTILS__DeviceGroupIDForCurrentFault();
    	mServiceInfo.UnitID      	 = NWSUTILS__DeviceUnitIDForCurrentFault();
    	mServiceInfo.ucFaultCode 	 = NWSUTILS__ucFaultCodeRegistered();
    }
    else
    if ( pEvpCool->bServiceNotificationRequired )
    {
    	mServiceInfo.MessageType 	  = NWS_SVCINF_COMMON__MT__SERVICE_NOTIFY;
    	mServiceInfo.GroupID     	  = TWBCOMIF__GID__GRP01;
    	mServiceInfo.UnitID      	  = SERVCLOG_EVAPCOOL__EvaporativeCoolerUnitIDWithFanHoursThatHaveExceededNotificationLimit();
    	mServiceInfo.uiFanHoursLogged = SERVCLOG_EVAPCOOL__uiEvaporativeCoolerFanOperationHoursLogged( mServiceInfo.UnitID );
    }
    else
    {
        // No message - Clear
    	mServiceInfo.MessageType = NWS_SVCINF_COMMON__MT__NONE;
    }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Checks if service prompt screen is being displayed flagging when reset or clear buttons have been pressed.
 * @param:	pEvpCool - Pointer to control structure.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static bool					bViewingServiceInfoPrompt(EVPCOOL_COMMON__ControlDetails* pEvpCool)
{
	bool		bViewingServiceInfo = false;

	if ( NWS_SVCINF_CONTROL__ServiceInfoPrompt_bShowIt() )
	{
		NWS_SVCINF_CONTROL__UserifUpdate();

		if ( NWS_SVCINF_CONTROL__ServiceInfoPrompt_bResetFlagged() )
		{
			pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__RESET_DEVICE;
		}

		if ( NWS_SVCINF_CONTROL__ServiceInfoPrompt_bClearFlagged() )
		{
			pEvpCool->UserAccess.Control.ButtonPressed = EVPCOOL_COMMON__CB__CLEAR_SERVICE_LOG;
		}

		bViewingServiceInfo = true;
	}

	return bViewingServiceInfo;
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

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - Fan speed level.
 * @param:	vbIncreasingValue - Whether value is being increased or decreased.
 * 			vFanSpeedLevel	  - The value of the fan speed level being adjusted.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_FanSpeedLevel(bool vbIncreasingValue, TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel)
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
 * @brief:	Rapid update control - Comfort level.
 * @param:	vbIncreasingValue - Whether value is being increased or decreased.
 * 			vucComfortLevel	  - The value of the comfort level being adjusted.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_ComfortLevel(bool vbIncreasingValue, uint8_t vucComfortLevel)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = false;
	TouchTagValue.bIncreasing        = vbIncreasingValue;
	TouchTagValue.ucMinimum			 = EVP_COOL_COMMON__SET_TEMP_MIN;
	TouchTagValue.ucMaximum			 = EVP_COOL_COMMON__SET_TEMP_MAX;
	TouchTagValue.ucActual			 = vucComfortLevel;

	FT800_TOUCHIFC__RapidUpdateDataDefine(&TouchTagValue);

	return;
}
//*********************************************************************************************************************************************************************************
