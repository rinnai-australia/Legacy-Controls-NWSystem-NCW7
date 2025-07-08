//*********************************************************************************************************************************************************************************
// tstifmgr.c
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
#include		<gentimdt.h>
#include		<tstifmgr.h>
#include		<nws_access.h>
#include		<twbifmgr.h>
#include		<twbif502.h>
#include		<nvmifmgr.h>
#include		<nc3pmdat.h>
#include		<nc4pmdat.h>
#include		<usrstdat.h>
#include		<svvaldat.h>
#include		<savuldat.h>
#include		<svlogdat.h>
#include		<scheddat.h>
#include		<strngdat.h>
#include		<tempmsif.h>
#include		<rtmclkif.h>
#include		<mcuaxess.h>

#include		<nws_userif_NC7.h>
#include		<uitftdsp.h>
#include		<uitftdsp_shape.h>
#include		<uicommon.h>
#include		<tftdspif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			TSTIFMGR__MAX_GENERAL_BYTES	        	10		// Max bytes with command

#define		   	TSTIFMGR__TEST_SEQUENCE_STATUS      	'S'		// Status response command
#define		   	TSTIFMGR__TEST_SEQUENCE_CMDACK     		0xE9	// ACK response
#define		   	TSTIFMGR__TEST_SEQUENCE_CMDNAK	   		0xA3	// NAK response

#define			TSTIFMGR__DELAY_CLOCK_VERIFY_100MS		40		// Delay time before checking clock
#define			TSTIFMGR__BACKLIGHT_CYCLE_TIME_10MS		50		// Toggle backlight at 500ms interval
#define			TSTIFMGR__BUTTON_HIGHLIGHT_FLASH_10MS	20		// Flash rate of button highlight

typedef enum	TSTIFMGR__TESTSTEP {							// Test Steps
																//
	TSTIFMGR__TS__NOTESTACTN = 0,       						// - No test being performed
	TSTIFMGR__TS__RSETEEPROM = 1,       						// - Reset eeprom before test
	TSTIFMGR__TS__TFTBAKLITE = 2,       						// - TFT backlight test
	TSTIFMGR__TS__TFTDISPIMG = 3,       						// - TFT display images
	TSTIFMGR__TS__TFTDISPTXT = 4,       						// - TFT display text
	TSTIFMGR__TS__TFTTCH_CAL = 5,       						// - TFT touch calibration
	TSTIFMGR__TS__TFTTCH_BTN = 6,       						// - TFT touch buttons
	TSTIFMGR__TS__TWOWIREBUS = 7,  	    						// - Test the two-wire bus comms
	TSTIFMGR__TS__CLCKVERIFY = 8 	    						// - Test the RTC operation

} TSTIFMGR__TESTSTEP;

typedef enum 	TSTIFMGR__TESTSTEP_STATUS {						// Test Step Status
																//
	TSTIFMGR__TSS__INITIATE = 1,   								// - Initiate action
	TSTIFMGR__TSS__PRFRMING = 2, 								// - Performing action
	TSTIFMGR__TSS__COMPLETE = 3,  								// - Completed action
	TSTIFMGR__TSS__RESPONSE = 4  								// - Response sent

} TSTIFMGR__TESTSTEP_STATUS;

typedef enum	TSTIFMGR__EEPROM_TEST_ACTION {					// Actions - EEPROM test
																//
	TSTIFMGR__ETA__IDLE 			 = 0,						// - Idle
	TSTIFMGR__ETA__CHK_SAVE_COMPLETE = 1,						// - Check save done
	TSTIFMGR__ETA__CHK_LOAD_COMPLETE = 2						// - Check load done

} TSTIFMGR__EEPROM_TEST_ACTION;

typedef enum	TSTIFMGR__TOUCHCAL_TEST_ACTION {				// Actions - Touch Cal Test
																//
	TSTIFMGR__TTA__IDLE 					  = 0,				// - Idle
	TSTIFMGR__TTA__CALIBRATE_TOUCH 			  = 1,				// - Calibrate touch screen
	TSTIFMGR__TTA__SAVE_TRANSFORM_VALUES_WAIT = 2,				// - Transform values : Wait save
	TSTIFMGR__TTA__LOAD_TRANSFORM_VALUES_WAIT = 3,				// - Transform values : Wait load to check
	TSTIFMGR__TTA__FATAL					  = 4				// - Fatal error

} TSTIFMGR__TOUCHCAL_TEST_ACTION;

typedef enum	TSTIFMGR__TOUCH_BUTTON_TEST_STATE {				// Test State - Touch Button
																//
	TSTIFMGR__TBTS__NOT_TESTED = 0,								// - Not tested yet
	TSTIFMGR__TBTS__TO_TEST    = 1,								// - Next to test
	TSTIFMGR__TBTS__TESTED     = 2								// - Tested

} TSTIFMGR__TOUCH_BUTTON_TEST_STATE;

typedef enum	TSTIFMGR__TWB_TEST_ACTION {						// Actions - EEPROM test
																//
	TSTIFMGR__TWB_TA__IDLE 			 = 0,						// - Idle
	TSTIFMGR__TWB_TA__WAIT_REGISTER  = 1,						// - Wait for registration
	TSTIFMGR__TWB_TA__WAIT_PARAMETER = 2						// - Wait for parameter retrieval

} TSTIFMGR__TWB_TEST_ACTION;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Display - Test Step : TSTIFMGR__TS__TFTDISPIMG
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	TSTIFMGR__RECTANGLE_NUMBER {

	TSTIFMGR__RN__R1C1 = 0,	 TSTIFMGR__RN__R1C2 = 1,  TSTIFMGR__RN__R1C3 = 2,	TSTIFMGR__RN__R1C4 = 3,	 TSTIFMGR__RN__R1C5 = 4,  TSTIFMGR__RN__R1C6 = 5,
	TSTIFMGR__RN__R2C1 = 6,	 TSTIFMGR__RN__R2C2 = 7,  TSTIFMGR__RN__R2C3 = 8,	TSTIFMGR__RN__R2C4 = 9,	 TSTIFMGR__RN__R2C5 = 10, TSTIFMGR__RN__R2C6 = 11,
	TSTIFMGR__RN__R3C1 = 12, TSTIFMGR__RN__R3C2 = 13, TSTIFMGR__RN__R3C3 = 14,	TSTIFMGR__RN__R3C4 = 15, TSTIFMGR__RN__R3C5 = 16, TSTIFMGR__RN__R3C6 = 17,
	TSTIFMGR__RN__R4C1 = 18, TSTIFMGR__RN__R4C2 = 19, TSTIFMGR__RN__R4C3 = 20, 	TSTIFMGR__RN__R4C4 = 21, TSTIFMGR__RN__R4C5 = 22, TSTIFMGR__RN__R4C6 = 23,

} TSTIFMGR__RECTANGLE_NUMBER;

static const UI_TFTSHAPE__Rectangle		mkaRectangle[] =
{
	// RECTANGLE
	//	   Color,   X,   Y,    W,  H,  LW
	{ 0x00000000,   1,   1,   80, 68,  2 },		// R1, C1
	{ 0x00000033,  81,   1,   80, 68,  2 },		// R1, C2
	{ 0x00000066, 161,   1,   80, 68,  2 },		// R1, C3
	{ 0x00000099, 241,   1,   80, 68,  2 },		// R1, C4
	{ 0x000000CC, 321,   1,   80, 68,  2 },		// R1, C5
	{ 0x000000FF, 401,   1,   80, 68,  2 },		// R1, C6

	{ 0x00000000,   1,  69,   80, 68,  2 },		// R2, C1
	{ 0x00003300,  81,  69,   80, 68,  2 },		// R2, C2
	{ 0x00006600, 161,  69,   80, 68,  2 },		// R2, C3
	{ 0x00009900, 241,  69,   80, 68,  2 },		// R2, C4
	{ 0x0000CC00, 321,  69,   80, 68,  2 },		// R2, C5
	{ 0x0000FF00, 401,  69,   80, 68,  2 },		// R2, C6

	{ 0x00000000,   1, 137,   80, 68,  2 },		// R3, C1
	{ 0x00330000,  81, 137,   80, 68,  2 },		// R3, C2
	{ 0x00660000, 161, 137,   80, 68,  2 },		// R3, C3
	{ 0x00990000, 241, 137,   80, 68,  2 },		// R3, C4
	{ 0x00CC0000, 321, 137,   80, 68,  2 },		// R3, C5
	{ 0x00FF0000, 401, 137,   80, 68,  2 },		// R3, C6

	{ 0x00FFFFFF,   1, 205,   80, 68,  2 },		// R3, C1
	{ 0x00FFFF00,  81, 205,   80, 68,  2 },		// R3, C2
	{ 0x00FF00FF, 161, 205,   80, 68,  2 },		// R3, C3
	{ 0x0000FFFF, 241, 205,   80, 68,  2 },		// R3, C4
	{ 0x0000FF00, 321, 205,   80, 68,  2 },		// R3, C5
	{ 0x000000FF, 401, 205,   80, 68,  2 },		// R3, C6
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Display - Test Step : TSTIFMGR__TS__TFTDISPTXT
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	TSTIFMGR__TEXT_NUMBER {

	TSTIFMGR__TN__TEXT01 = 0,
	TSTIFMGR__TN__TEXT02 = 1,
	TSTIFMGR__TN__TEXT03 = 2,
	TSTIFMGR__TN__TEXT04 = 3,
	TSTIFMGR__TN__TEXT05 = 4,
	TSTIFMGR__TN__TEXT06 = 5,
	TSTIFMGR__TN__TEXT07 = 6,
	TSTIFMGR__TN__TEXT08 = 7,
	TSTIFMGR__TN__TEXT09 = 8,
	TSTIFMGR__TN__TEXT10 = 9

} TSTIFMGR__TEXT_NUMBER;

static const UITFTDSP_TEXT__TextDetails		mkaText[] = {
	//    X,   Y,  X0,  Y0, 	  Color,          		    FontSize,    	    	  Justification, String Label
	{   240,   4,  -1,  -1,  0x00FF00FF, UITFTDSP_FONTS__FS__ROUND_6,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240,  44,  -1,  -1,  0x0000FFFF, UITFTDSP_FONTS__FS__ROUND_5,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240,  74,  -1,  -1,  0x00FFFF00, UITFTDSP_FONTS__FS__ROUND_4,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240,  99,  -1,  -1,  0x0000FF00, UITFTDSP_FONTS__FS__ROUND_3,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240, 119,  -1,  -1,  0x00FFFFFF, UITFTDSP_FONTS__FS__ROUND_2,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240, 134,  -1,  -1,  0x00FFFFFF, UITFTDSP_FONTS__FS__ROUND_2,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240, 149,  -1,  -1,  0x0000FF00, UITFTDSP_FONTS__FS__ROUND_3,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240, 169,  -1,  -1,  0x00FFFF00, UITFTDSP_FONTS__FS__ROUND_4,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240, 194,  -1,  -1,  0x0000FFFF, UITFTDSP_FONTS__FS__ROUND_5,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
	{   240, 224,  -1,  -1,  0x00FF00FF, UITFTDSP_FONTS__FS__ROUND_6,     UITFTDSP_TEXT__TJ__CENTER, "BRIVIS TOUCH" },
};

static const uint8_t	mkucNumberL = 7;
static const uint8_t	mkucNumberR = 7;

typedef enum	TSTIFMGR__NUMBER_NUMBER {

	TSTIFMGR__NN__L = 0,
	TSTIFMGR__NN__R = 1,

} NWS_CLNSCN_USERIF_NC7__NUMBER_NUMBER;

static const UI_TFTNUMBER__NumberDetails	mkaNumber[] = {

	{    75,  100, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__CUSTOM_0,   UITFTDSP_TEXT__TJ__CENTER, (uint8_t*)&mkucNumberL },
	{   400,  100, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__CUSTOM_0,   UITFTDSP_TEXT__TJ__CENTER, (uint8_t*)&mkucNumberR },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Display - Test Step : TSTIFMGR__TS__TFTTCH_BTN
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			TSTIFMGR__TEST_MAX_BUTTONS		9

typedef enum	TSTIFMGR__BUTTON {

	TSTIFMGR__BUTTON01 = 0, TSTIFMGR__BUTTON02 = 1, TSTIFMGR__BUTTON03 = 2,
	TSTIFMGR__BUTTON04 = 3, TSTIFMGR__BUTTON05 = 4, TSTIFMGR__BUTTON06 = 5,
	TSTIFMGR__BUTTON07 = 6, TSTIFMGR__BUTTON08 = 7, TSTIFMGR__BUTTON09 = 8,

} TSTIFMGR__BUTTON;

typedef enum	TSTIFMGR__TAG_VALUE {

	TSTIFMGR__TV__01_BTN = 10,
	TSTIFMGR__TV__02_BTN = 11,
	TSTIFMGR__TV__03_BTN = 12,
	TSTIFMGR__TV__04_BTN = 13,
	TSTIFMGR__TV__05_BTN = 14,
	TSTIFMGR__TV__06_BTN = 15,
	TSTIFMGR__TV__07_BTN = 16,
	TSTIFMGR__TV__08_BTN = 17,
	TSTIFMGR__TV__09_BTN = 18

} TSTIFMGR__TAG_VALUE;

typedef enum	TSTIFMGR__BUTTON_NUMBER {

	TSTIFMGR__BN__01_PASV_BTN = 0,  TSTIFMGR__BN__01_ACTV_BTN = 1,
	TSTIFMGR__BN__02_PASV_BTN = 2,  TSTIFMGR__BN__02_ACTV_BTN = 3,
	TSTIFMGR__BN__03_PASV_BTN = 4,  TSTIFMGR__BN__03_ACTV_BTN = 5,
	TSTIFMGR__BN__04_PASV_BTN = 6,  TSTIFMGR__BN__04_ACTV_BTN = 7,
	TSTIFMGR__BN__05_PASV_BTN = 8,  TSTIFMGR__BN__05_ACTV_BTN = 9,
	TSTIFMGR__BN__06_PASV_BTN = 10, TSTIFMGR__BN__06_ACTV_BTN = 11,
	TSTIFMGR__BN__07_PASV_BTN = 12, TSTIFMGR__BN__07_ACTV_BTN = 13,
	TSTIFMGR__BN__08_PASV_BTN = 14, TSTIFMGR__BN__08_ACTV_BTN = 15,
	TSTIFMGR__BN__09_PASV_BTN = 16, TSTIFMGR__BN__09_ACTV_BTN = 17,

} TSTIFMGR__BUTTON_NUMBER;

static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton[] =
{    	// BUTTON																										  		   		         RECTANGLE						          TEXT
		//                Normal State,    HL,                              Persist time, 					     Style,	                 Tag,	      Color,    X,   Y,   W,   H,  LW  	      X,   Y,  XO,  YO, Color,          		  FontSize,		  	    Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__01_BTN, {  {        0,   90,  38,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "1" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__01_BTN, {  {        0,   90,  38,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "1" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__02_BTN, {  {        0,  210,  38,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "2" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__02_BTN, {  {        0,  210,  38,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "2" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__03_BTN, {  {        0,  330,  38,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "3" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__03_BTN, {  {        0,  330,  38,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "3" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__04_BTN, {  {        0,   90, 106,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "4" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__04_BTN, {  {        0,   90, 106,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "4" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__05_BTN, {  {        0,  210, 106,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "5" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__05_BTN, {  {        0,  210, 106,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "5" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__06_BTN, {  {        0,  330, 106,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "6" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__06_BTN, {  {        0,  330, 106,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "6" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__07_BTN, {  {        0,   90, 174,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "7" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__07_BTN, {  {        0,   90, 174,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "7" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__08_BTN, {  {        0,  210, 174,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "8" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__08_BTN, {  {        0,  210, 174,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "8" 	} } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__09_BTN, {  {        0,  330, 174,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "9" 	} } },
	{	 UITFTDSP_BUTTON__STATE_ACTIVE, false, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, TSTIFMGR__TV__09_BTN, {  {        0,  330, 174,  60,  60,  0  },  {  -1,  -1,  -1,  -1,     0, UITFTDSP_FONTS__FS__ROUND_4, UITFTDSP_TEXT__TJ__CENTER, "9" 	} } },
} ;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	TestDetails_EEPROM {														// EEPROM Test Details
																							//
	TSTIFMGR__EEPROM_TEST_ACTION		TestAction;											// - Test action to perform
  	uint8_t                        		aucSerialNumber[TWBIFUTL__MAXCHARS__SERIAL_NUMBER];
  																							//
} TestDetails_EEPROM;																		//

typedef struct	TestDetails_Backlight {														// Backlight Test Details
																							//
	bool								bSwitchOn;											// - Backlight toggle
  																							//
} TestDetails_Backlight;																	//

typedef struct	TestDetails_TouchCal {														// Touch Calibration Test Details
																							//
	TSTIFMGR__TOUCHCAL_TEST_ACTION		TestAction;											// - Test action to perform
	bool								bInit;												// - Initialise flag
																							//
	uint32_t							ulTransformA;										// - Cal Transform A value
	uint32_t							ulTransformB;										// - Cal Transform B value
	uint32_t							ulTransformC;										// - Cal Transform C value
	uint32_t							ulTransformD;										// - Cal Transform D value
	uint32_t							ulTransformE;										// - Cal Tramsform E value
	uint32_t							ulTransformF;										// - Cal Transform F value

} TestDetails_TouchCal;

typedef struct	TestDetails_TouchButton {													// Touch Button Test Details
																							//
  	TSTIFMGR__TOUCH_BUTTON_TEST_STATE	aTestState[TSTIFMGR__TEST_MAX_BUTTONS];				// - Button states
  	bool								bHighlighted;										// - Highlight flag when flashing
																							//
} TestDetails_TouchButton;																	//

typedef struct	TestDetails_TwoWireBus {													// Two-Wire Bus Test Details
																							//
	TSTIFMGR__TWB_TEST_ACTION			TestAction;											// - Test action
	TWBCOMIF__TWBDeviceAddress   		TargetAddress;										// - Address of device to communicate with
  	GENPRMDT__Parameter              	Parameter;											// - Parameter data for test
																							//
} TestDetails_TwoWireBus;																	//

typedef struct	TestDetails_Clock {															// Clock Test Details
																							//
  	uint8_t             				ucLastSecond;										// - Last second saved
																							//
} TestDetails_Clock;																		//

typedef struct	TestDetails {																// Test Details
																							//
	TSTIFMGR__TESTSTEP					Step;												// - Test Step
	TSTIFMGR__TESTSTEP_STATUS			StepStatus;											// - Status of test step
	TSTIFMGR__ACTION_STATUS				ActionStatus;										// - Action to take to report status
																							//
	uint8_t								aucCmdData[TSTIFMGR__MAX_GENERAL_BYTES];			// - Data associated with command
	uint8_t								aucRspData[TSTIFMGR__MAX_GENERAL_BYTES];			// - Data associated with response
	bool								bStepPassed;										// - Flag indicating test step passed
  	GENTIMDT__TimingCounter				ActionTimingCounter;								// - General timing counter for action processing
  	GENTIMDT__TimingCounter				UserifTimingCounter;								// - Timer to flash highlight
  																							//
  	TestDetails_EEPROM					EEPROM;												// - EEPROM test details
  	TestDetails_Backlight				Backlight;											// - Backlight test details
  	TestDetails_TouchCal				TouchCal;											// - Touch calibration test details
  	TestDetails_TouchButton				TouchButton;										// - Touch button test details
  	TestDetails_TwoWireBus				TwoWireBus;											// - Two-wire bus test details
  	TestDetails_Clock					Clock;												// - Clock details

} TestDetails;

static TestDetails	mTest;																	// Container Variable
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	TSTIFMGR__kTestActionTiming = {	&mTest.ActionTimingCounter };
const struct	GENTIMDT__Timing	TSTIFMGR__kTestUserifTiming = {	&mTest.UserifTimingCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void    					TestSequence_UpdateControl_EEPROMTest(void);
static void						TestSequence_UpdateControl_TFTBacklightTest(void);
static void						TestSequence_UpdateControl_TFTImageDisplayTest(void);
static void						TestSequence_UpdateControl_TFTTextDisplayTest(void);
static void						TestSequence_UpdateControl_TFTTouchCalibrationTest(void);
static void						TestSequence_UpdateControl_TFTTouchButtonTest(void);
static void						TestSequence_UpdateControl_TwowireBus(void);
static void						TestSequence_UpdateControl_ClockVerify(void);

static void    					TestSequence_UpdateUserif_EEPROMTest(void);
static void						TestSequence_UpdateUserif_TFTBacklightTest(void);
static void						TestSequence_UpdateUserif_TFTImageDisplayTest(void);
static void						TestSequence_UpdateUserif_TFTTextDisplayTest(void);
static void						TestSequence_UpdateUserif_TFTTouchCalibrationTest(void);
static void						TestSequence_UpdateUserif_TFTTouchButtonTest(void);
static void						TestSequence_UpdateUserif_TwowireBus(void);
static void						TestSequence_UpdateUserif_ClockVerify(void);

static void						TestSequence_UpdateUserif_TFTTouchButtonDisplay(TSTIFMGR__BUTTON_NUMBER vucPassiveIndex, TSTIFMGR__BUTTON_NUMBER vucActiveIndex, TSTIFMGR__TOUCH_BUTTON_TEST_STATE vTestState);

static void						StartTest(TSTIFMGR__TESTSTEP vTestStep);
static void						IndicateTestIsBeingPerformed(uint8_t vucTestActionTimeInterval);
static void   					IndicateTestCompleted(bool vbPassed);
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
 * @brief:  Test interface init.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TSTIFMGR__Initialise(void)
{
	mTest.Step = TSTIFMGR__TS__NOTESTACTN;
	return;
}

#if defined(ALLOW_DUMMY_TEST)
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:  Functions that allow a test to be started for verification.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TSTIFMGR__StartDummyTest_EEP(void)
{
	mTest.aucCmdData[0] = 'T';
	mTest.aucCmdData[1] = 'E';
	mTest.aucCmdData[2] = 'S';
	mTest.aucCmdData[3] = 'T';
	mTest.aucCmdData[4] = '0';
	mTest.aucCmdData[5] = '0';
	mTest.aucCmdData[6] = '1';
	mTest.aucCmdData[7] = 1;

	StartTest( TSTIFMGR__TS__RSETEEPROM );
	return;
}
void							TSTIFMGR__StartDummyTest_BAK(void)
{
	StartTest( TSTIFMGR__TS__TFTBAKLITE );
	return;
}
void							TSTIFMGR__StartDummyTest_IMG(void)
{
	StartTest( TSTIFMGR__TS__TFTDISPIMG );
	return;
}
void							TSTIFMGR__StartDummyTest_TXT(void)
{
	StartTest( TSTIFMGR__TS__TFTDISPTXT );
	return;
}
void							TSTIFMGR__StartDummyTest_CAL(void)
{
	StartTest( TSTIFMGR__TS__TFTTCH_CAL );
	return;
}
void							TSTIFMGR__StartDummyTest_BTN(void)
{
	StartTest( TSTIFMGR__TS__TFTTCH_BTN );
	return;
}
void							TSTIFMGR__StartDummyTest_TWB(void)
{
	mTest.aucCmdData[0] = 50;			// Time allowed for registration 	 - 5000ms
	mTest.aucCmdData[1] = 25;			// Time allowed for comms with slave - 2500ms

	StartTest( TSTIFMGR__TS__TWOWIREBUS );
	return;
}
void							TSTIFMGR__StartDummyTest_CLK(void)
{
	StartTest( TSTIFMGR__TS__CLCKVERIFY );
	return;
}
#endif
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when test is being performed.
 * @param:  None.
 * @retval: bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TSTIFMGR__bATestIsBeingPerformed(void)
{
	return (bool)( mTest.Step != TSTIFMGR__TS__NOTESTACTN );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates any test that is beonbg performed that has been completed.
 * @param:  None.
 * @retval: bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TSTIFMGR__bATestBeingPerformedHasBeenCompleted(void)
{
	return (bool)( ( TSTIFMGR__bATestIsBeingPerformed() ) && ( mTest.StepStatus == TSTIFMGR__TSS__COMPLETE ) );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test sequence update - Command process for comms.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TSTIFMGR__TestSequence_UpdateCmdProcess(uint8_t* pucRxBuffer)
{
	uint8_t		ucIndex;

	switch ( pucRxBuffer[0] )
	{
		case TSTIFMGR__TS__NOTESTACTN:
        case TSTIFMGR__TS__RSETEEPROM:
        case TSTIFMGR__TS__TFTBAKLITE:
        case TSTIFMGR__TS__TFTDISPIMG:
        case TSTIFMGR__TS__TFTDISPTXT:
        case TSTIFMGR__TS__TFTTCH_CAL:
        case TSTIFMGR__TS__TFTTCH_BTN:
        case TSTIFMGR__TS__TWOWIREBUS:
        case TSTIFMGR__TS__CLCKVERIFY:
        	if ( pucRxBuffer[0] != TSTIFMGR__TS__NOTESTACTN )
        	{
        		// Start a test!
    			// - Define general test bytes
    			for ( ucIndex = 0; ucIndex < TSTIFMGR__MAX_GENERAL_BYTES; ucIndex++ )
    			{
    				mTest.aucCmdData[ucIndex] = pucRxBuffer[ucIndex+1];
    			}
    			// - Test step
    			StartTest( (TSTIFMGR__TESTSTEP)pucRxBuffer[0] );
        	}
        	else
        	{
        		mTest.Step = pucRxBuffer[0];
        	}

        	mTest.ActionStatus = TSTIFMGR__AS__RETURN_ACK;
			break;

		case TSTIFMGR__TEST_SEQUENCE_STATUS:
			mTest.ActionStatus = TSTIFMGR__AS__RETURN_STS;
			break;

		default:
			mTest.ActionStatus = TSTIFMGR__AS__RETURN_NAK;
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test sequence update - Response process for comms.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TSTIFMGR__TestSequence_UpdateRspProcess(uint8_t* pucTxBuffer)
{
	uint8_t		ucIndex;

	switch ( mTest.ActionStatus )
	{
		case TSTIFMGR__AS__RETURN_STS:
		    // Returning status
			pucTxBuffer[0] = TSTIFMGR__TEST_SEQUENCE_STATUS;
			// Test Step
			pucTxBuffer[1] = mTest.Step;
		    // Test Step Status
			pucTxBuffer[2] = mTest.StepStatus;
		    // Test step Pass/Fail indicator
			pucTxBuffer[3] = (uint8_t)mTest.bStepPassed;
		    // Associated return data
			for ( ucIndex = 0; ucIndex < TSTIFMGR__MAX_GENERAL_BYTES; ucIndex++ )
			{
				pucTxBuffer[ucIndex+4] = mTest.aucRspData[ucIndex];
			}
			break;

		case TSTIFMGR__AS__RETURN_ACK:
			pucTxBuffer[0] = TSTIFMGR__TEST_SEQUENCE_CMDACK;
			break;

		default:
			pucTxBuffer[0] = TSTIFMGR__TEST_SEQUENCE_CMDNAK;
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test sequence update - Control.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TSTIFMGR__TestSequence_UpdateControl(void)
{
	// Test Update
	switch ( mTest.Step )
	{
        case TSTIFMGR__TS__RSETEEPROM:
        	TestSequence_UpdateControl_EEPROMTest();
            break;

        case TSTIFMGR__TS__TFTBAKLITE:
        	TestSequence_UpdateControl_TFTBacklightTest();
            break;

        case TSTIFMGR__TS__TFTDISPIMG:
        	TestSequence_UpdateControl_TFTImageDisplayTest();
            break;

        case TSTIFMGR__TS__TFTDISPTXT:
        	TestSequence_UpdateControl_TFTTextDisplayTest();
            break;

        case TSTIFMGR__TS__TFTTCH_CAL:
        	TestSequence_UpdateControl_TFTTouchCalibrationTest();
            break;

        case TSTIFMGR__TS__TFTTCH_BTN:
        	TestSequence_UpdateControl_TFTTouchButtonTest();
            break;

        case TSTIFMGR__TS__TWOWIREBUS:
        	TestSequence_UpdateControl_TwowireBus();
			break;

		case TSTIFMGR__TS__CLCKVERIFY:
        	TestSequence_UpdateControl_ClockVerify();
            break;

		default:
        	break;
	}

	NWS_ACCESS__ControlUpdate();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test sequence update - User Interface.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TSTIFMGR__TestSequence_UpdateUserif(void)
{
	// Test Update
	switch ( mTest.Step )
	{
        case TSTIFMGR__TS__RSETEEPROM:
        	TestSequence_UpdateUserif_EEPROMTest();
            break;

        case TSTIFMGR__TS__TFTBAKLITE:
        	TestSequence_UpdateUserif_TFTBacklightTest();
            break;

        case TSTIFMGR__TS__TFTDISPIMG:
        	TestSequence_UpdateUserif_TFTImageDisplayTest();
            break;

        case TSTIFMGR__TS__TFTDISPTXT:
        	TestSequence_UpdateUserif_TFTTextDisplayTest();
            break;

        case TSTIFMGR__TS__TFTTCH_CAL:
        	TestSequence_UpdateUserif_TFTTouchCalibrationTest();
            break;

        case TSTIFMGR__TS__TFTTCH_BTN:
        	TestSequence_UpdateUserif_TFTTouchButtonTest();
            break;

        case TSTIFMGR__TS__TWOWIREBUS:
        	TestSequence_UpdateUserif_TwowireBus();
			break;

		case TSTIFMGR__TS__CLCKVERIFY:
			TestSequence_UpdateUserif_ClockVerify();
            break;

		default:
        	break;
	}

	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - EEPROM.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void    					TestSequence_UpdateControl_EEPROMTest(void)
{
	uint8_t		ucIndex;
	bool		bPassed = true;

	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
		// Force all eeprom data to default values
		// - All parameter data
		NC3PMDAT__ResetAllNC3ParametersToTheirDefaultValues();
		//NC4PMDAT_ResetAllNC4ParametersToTheirDefaultValues();
		// - User setting data
		USRSTDAT__RestoreAllUserSettingsBackToTheirDefaultValues();
		// - General saved data
		SVVALDAT__RestoreAllSavedDataBackToTheirDefaultSettings();
		// - UL data
		SAVULDAT__RestoreAllULValuesBackToTheirDefaultSettings();
		// - Auto program data
		SCHEDDAT__RestoreAllScheduleDataBackToTheirDefaultSettings();
		// - String settings
		STRNGDAT__RestoreAllStringValuesBackToTheirDefaultSettings();
		// - Service log
		SVLOGDAT__RestoreAllServiceLogDataBackToTheirDefaultSettings();
		//
		// Define parameter data to be saved
		// - Serial number sent down
		TWBIF502__SetSerialNumberOfThisUnit( mTest.aucCmdData );
		// - Unit ID sent down
        *TWBIF502__kThisType02DeviceUnitID.pucActual = mTest.aucCmdData[7];
        //
		// Initiate save of all data to eeprom
        NVMIFMGR__BeginSaveProcess_AllData( false );
		//
		// EEPROM Test Acrion - Check for save completed
		mTest.EEPROM.TestAction = TSTIFMGR__ETA__CHK_SAVE_COMPLETE; IndicateTestIsBeingPerformed( 0 );
	}
	else
	if ( mTest.StepStatus == TSTIFMGR__TSS__PRFRMING )
	{
		switch ( mTest.EEPROM.TestAction )
		{
			case TSTIFMGR__ETA__CHK_SAVE_COMPLETE:
				if ( !NVMIFMGR__bSaveProcessComplete() ) { break; }

				NVMIFMGR__LoadProcess_Start(); mTest.EEPROM.TestAction = TSTIFMGR__ETA__CHK_LOAD_COMPLETE;
				break;

			case TSTIFMGR__ETA__CHK_LOAD_COMPLETE:
				if ( !NVMIFMGR__bLoadProcessComplete() ) { break; }
				//
				// Check serial number
				// - Get serial number loaded from eeprom
				TWBIF502__GetSerialNumberOfThisUnit( mTest.EEPROM.aucSerialNumber );
		        // - Check it against that sent down
		        for ( ucIndex = 0; ucIndex < TWBIFUTL__MAXCHARS__SERIAL_NUMBER; ucIndex++ )
		        {
		        	if ( mTest.aucCmdData[ucIndex] != mTest.EEPROM.aucSerialNumber[ucIndex] )
		        	{
		        		bPassed = false;
		        	}
				}
		        // Compare UnitID against that sent down
		        if ( *TWBIF502__kThisType02DeviceUnitID.pucActual != mTest.aucCmdData[7] )
		        {
		        	bPassed = false;
		        }
		        // Test completed!
		        IndicateTestCompleted( bPassed ); mTest.EEPROM.TestAction = TSTIFMGR__ETA__IDLE;
				break;

			default:
				break;
		}
	}

	mTest.aucRspData[0] = mTest.EEPROM.TestAction;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - TFT Backlight.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateControl_TFTBacklightTest(void)
{
	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
		GENTIMDT__StartTimer_TB_10MS( TSTIFMGR__BACKLIGHT_CYCLE_TIME_10MS, &mTest.UserifTimingCounter ); mTest.Backlight.bSwitchOn = true;

		IndicateTestIsBeingPerformed( 0 );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - TFT image display.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateControl_TFTImageDisplayTest(void)
{
	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
		IndicateTestIsBeingPerformed( 0 );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - TFT text display..
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateControl_TFTTextDisplayTest(void)
{
	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
		IndicateTestIsBeingPerformed( 0 );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - TFT touch calibration.
 * 			This test forces the user to recalibrate the touch screen with the transform values generated saved to eeprom.  A reload of the eeprom is forced to ensure the transform
 * 			values were saved successfully.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateControl_TFTTouchCalibrationTest(void)
{
	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
        // Blank display
		NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BLACK, 0 ); NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();
		// Redo cal
		TFTDSPIF__TouchCalRedo();
		// Start test
		mTest.TouchCal.TestAction = TSTIFMGR__TTA__CALIBRATE_TOUCH; IndicateTestIsBeingPerformed( 0 );
	}

	switch ( mTest.TouchCal.TestAction )
	{
		case TSTIFMGR__TTA__CALIBRATE_TOUCH:
			// Update touch to cal
			TFTDSPIF__TouchUpdate();
			// Check for result
			if ( TFTDSPIF__bTouchCalDoneValid() )
			{
				IndicateTestCompleted( true );
				// Test action idle
				mTest.TouchCal.TestAction = TSTIFMGR__TTA__IDLE;
			}
			else
			if ( TFTDSPIF__bTouchCalDoneValidSaveValues() )
			{
				// Backup the transform values A-F
				mTest.TouchCal.ulTransformA = TFTDSPIF__ulTransformAValue();
				mTest.TouchCal.ulTransformB = TFTDSPIF__ulTransformBValue();
				mTest.TouchCal.ulTransformC = TFTDSPIF__ulTransformCValue();
				mTest.TouchCal.ulTransformD = TFTDSPIF__ulTransformDValue();
				mTest.TouchCal.ulTransformE = TFTDSPIF__ulTransformEValue();
				mTest.TouchCal.ulTransformF = TFTDSPIF__ulTransformFValue();
				// Save the values
				NVMIFMGR__BeginSaveProcess_TouchCalValues( false );

				mTest.TouchCal.TestAction = TSTIFMGR__TTA__SAVE_TRANSFORM_VALUES_WAIT;	// Calibration passed - Save values to eeprom
			}
			else
			if ( TFTDSPIF__bTouchCalDoneFatal() )
			{
				mTest.TouchCal.TestAction = TSTIFMGR__TTA__FATAL;
			}
			break;

		case TSTIFMGR__TTA__SAVE_TRANSFORM_VALUES_WAIT:
			if ( !NVMIFMGR__bSaveProcessComplete() ) { break; }
			//
			// Reset transform values to verify they load ok
			TFTDSPIF__ResetTransformValues(); NVMIFMGR__LoadProcess_Start();

			mTest.TouchCal.TestAction = TSTIFMGR__TTA__LOAD_TRANSFORM_VALUES_WAIT;
			break;

		case TSTIFMGR__TTA__LOAD_TRANSFORM_VALUES_WAIT:
			if ( !NVMIFMGR__bLoadProcessComplete() ) { break; }
			//
			// All transform values must match or else test failure
			if ( mTest.TouchCal.ulTransformA != TFTDSPIF__ulTransformAValue() )
			{
				IndicateTestCompleted( false );
			}
			else
			if ( mTest.TouchCal.ulTransformB != TFTDSPIF__ulTransformBValue() )
			{
				IndicateTestCompleted( false );
			}
			else
			if ( mTest.TouchCal.ulTransformC != TFTDSPIF__ulTransformCValue() )
			{
				IndicateTestCompleted( false );
			}
			else
			if ( mTest.TouchCal.ulTransformD != TFTDSPIF__ulTransformDValue() )
			{
				IndicateTestCompleted( false );
			}
			else
			if ( mTest.TouchCal.ulTransformE != TFTDSPIF__ulTransformEValue() )
			{
				IndicateTestCompleted( false );
			}
			else
			if ( mTest.TouchCal.ulTransformF != TFTDSPIF__ulTransformFValue() )
			{
				IndicateTestCompleted( false );
			}
			else
			{
				IndicateTestCompleted( true );
			}
			// Test action idle
			mTest.TouchCal.TestAction = TSTIFMGR__TTA__IDLE;
			break;

		case TSTIFMGR__TTA__FATAL:
			IndicateTestCompleted( false );

			mTest.TouchCal.TestAction = TSTIFMGR__TTA__IDLE;
			break;

		default:
			break;
	}

	mTest.aucRspData[0] = mTest.TouchCal.TestAction;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - TFT touch button.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateControl_TFTTouchButtonTest(void)
{
	bool		bTestCompleted = true;
	uint8_t		ucIndex;

	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
		// Clear all buttons states
		for ( ucIndex = 0; ucIndex < TSTIFMGR__TEST_MAX_BUTTONS; ucIndex++ )
		{
			mTest.TouchButton.aTestState[ucIndex] = TSTIFMGR__TBTS__NOT_TESTED;
		}
		//
		// Start flash timer
		mTest.TouchButton.bHighlighted = true; GENTIMDT__StartTimer_TB_10MS( TSTIFMGR__BUTTON_HIGHLIGHT_FLASH_10MS, &mTest.UserifTimingCounter );
		// Start with button 1 to test
		mTest.TouchButton.aTestState[TSTIFMGR__BUTTON01] = TSTIFMGR__TBTS__TO_TEST; IndicateTestIsBeingPerformed( 0 );
	}
	else
	if ( mTest.StepStatus == TSTIFMGR__TSS__PRFRMING )
	{
		// Check for test completed
		for ( ucIndex = 0; ucIndex < TSTIFMGR__TEST_MAX_BUTTONS; ucIndex++ )
		{
			if ( mTest.TouchButton.aTestState[ucIndex] != TSTIFMGR__TBTS__TESTED )
			{
				bTestCompleted = false;
			}
		}
		if ( bTestCompleted ) { IndicateTestCompleted( true ); }
	}

	for ( ucIndex = 0; ucIndex < TSTIFMGR__TEST_MAX_BUTTONS; ucIndex++ )
	{
		mTest.aucRspData[ucIndex] = mTest.TouchButton.aTestState[ucIndex];
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - Two-wire bus.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateControl_TwowireBus(void)
{
	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
        // Force registration of slave NC6
        // - Full configuration/initialisation
        NWS_ACCESS__Initialise();
        // - Force a request hail command to be issued
        TWBIF502__FlagCmdToTx_RequestHail();
        //
        // Update control
        mTest.TwoWireBus.TestAction = TSTIFMGR__TWB_TA__WAIT_REGISTER; IndicateTestIsBeingPerformed( mTest.aucCmdData[0] );
	}
	else
	if ( mTest.StepStatus == TSTIFMGR__TSS__PRFRMING )
	{

		switch ( mTest.TwoWireBus.TestAction )
		{
			case TSTIFMGR__TWB_TA__WAIT_REGISTER:
				if ( mTest.ActionTimingCounter.ulValue != 0 ) { break; }
				//
	            // Define the target address
	            mTest.TwoWireBus.TargetAddress.GroupID = TWBCOMIF__GID__GRP02;
	            mTest.TwoWireBus.TargetAddress.UnitID  = TWBCOMIF__UID__UNT02;
	            // Define the parameter to retrieve for testing of comms!
	            mTest.TwoWireBus.Parameter.Type              = GENPRMDT__PT__INSTALL;
	            mTest.TwoWireBus.Parameter.ucIdentification  = 0;
	            mTest.TwoWireBus.Parameter.ucActualValue     = 0;
	            mTest.TwoWireBus.Parameter.bHasBeenRetrieved = false;
	            // Issue the command
	            TWBIFMGR__RetrieveParameter( true, &mTest.TwoWireBus.Parameter, &mTest.TwoWireBus.TargetAddress, TWBIFUTL__NWTYPE__NC3 );
	            // Update control
	            mTest.TwoWireBus.TestAction = TSTIFMGR__TWB_TA__WAIT_PARAMETER; IndicateTestIsBeingPerformed( mTest.aucCmdData[1] );
				break;

			case TSTIFMGR__TWB_TA__WAIT_PARAMETER:
				if ( mTest.ActionTimingCounter.ulValue != 0 ) { break; }

				if ( ( mTest.TwoWireBus.Parameter.bHasBeenRetrieved ) && ( mTest.TwoWireBus.Parameter.ucActualValue == 2 ) )
		        {
		        	IndicateTestCompleted( true );
		        }
		        else
		        {
		         	IndicateTestCompleted( false );
		        }
		        mTest.TwoWireBus.TestAction = TSTIFMGR__TWB_TA__IDLE;
				break;

			default:
				break;
		}
	}

	mTest.aucRspData[0] = mTest.TwoWireBus.TestAction;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test Control Update - Clock.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateControl_ClockVerify(void)
{
	if ( RTMCLKIF__bClockHasFailed() ) { NWS_ACCESS__ForceClockToBeSet(); }

	if ( mTest.StepStatus == TSTIFMGR__TSS__INITIATE )
	{
		mTest.Clock.ucLastSecond = RTMCLKIF__pClock()->ucCurrentSecond; IndicateTestIsBeingPerformed( TSTIFMGR__DELAY_CLOCK_VERIFY_100MS );
	}
	else
	if ( mTest.StepStatus == TSTIFMGR__TSS__PRFRMING )
	{
		if ( mTest.ActionTimingCounter.ulValue == 0 )
		{
	    	IndicateTestCompleted( (bool)( RTMCLKIF__pClock()->ucCurrentSecond != mTest.Clock.ucLastSecond ) );
		}
	}

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User UI Update - EEPROM.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void    					TestSequence_UpdateUserif_EEPROMTest(void)
{
	UITFTDSP__BacklightControl_Update( false, true, 0 );

	NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BLACK, 0 ); NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test UI Update - TFT Backlight.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_TFTBacklightTest(void)
{
	if ( mTest.UserifTimingCounter.ulValue == 0 )
	{
		mTest.Backlight.bSwitchOn = !mTest.Backlight.bSwitchOn;	GENTIMDT__StartTimer_TB_10MS( TSTIFMGR__BACKLIGHT_CYCLE_TIME_10MS, &mTest.UserifTimingCounter );
	}

	if ( mTest.Backlight.bSwitchOn )
	{
		UITFTDSP__BacklightControl_Update( false, true, 128 );
	}
	else
	{
		UITFTDSP__BacklightControl_Update( false, true, 0 );
	}

	NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BACKGROUND, 0 ); NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();

	IndicateTestCompleted( true );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test UI Update - TFT image display.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_TFTImageDisplayTest(void)
{
	UITFTDSP__BacklightControl_Update( false, true, 128 );

	NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BLACK, 0 );
	//
	// Update the display with test pattern
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R1C1] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R1C2] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R1C3] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R1C4] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R1C5] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R1C6] );

	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R2C1] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R2C2] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R2C3] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R2C4] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R2C5] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R2C6] );

	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R3C1] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R3C2] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R3C3] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R3C4] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R3C5] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R3C6] );

	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R4C1] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R4C2] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R4C3] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R4C4] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R4C5] );
	UITFTDSP__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[TSTIFMGR__RN__R4C6] );

	NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();
	//
	// When performing just indicate passed once image displayed
	IndicateTestCompleted( true );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test UI Update - TFT text display..
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_TFTTextDisplayTest(void)
{
	UITFTDSP__BacklightControl_Update( false, true, 128 );

	NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BLACK, 0 );
	//
	// Update the display with test pattern
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT01] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT02] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT03] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT04] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT05] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT06] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT07] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT08] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT09] );
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[TSTIFMGR__TN__TEXT10] );

	UICOMMON__ShowLargeNumber( (UI_TFTNUMBER__NumberDetails*)&mkaNumber[TSTIFMGR__NN__L] );
	UICOMMON__ShowLargeNumber( (UI_TFTNUMBER__NumberDetails*)&mkaNumber[TSTIFMGR__NN__R] );

	NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();
	//
	// When performing just indicate passed once text displayed
	IndicateTestCompleted( true );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test UI Update - TFT touch calibration.
 * 			This test forces the user to recalibrate the touch screen with the transform values generated saved to eeprom.  A reload of the eeprom is forced to ensure the transform
 * 			values were saved successfully.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_TFTTouchCalibrationTest(void)
{
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test UI Update - TFT touch button.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_TFTTouchButtonTest(void)
{
	TSTIFMGR__BUTTON	Button;

	UITFTDSP__BacklightControl_Update( false, true, 128 );
	UITFTDSP__TouchControl_Update();

	NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BLACK, 0 );
	//
	// Highlight flash timer
	if ( mTest.UserifTimingCounter.ulValue == 0 )
	{
		mTest.TouchButton.bHighlighted = !mTest.TouchButton.bHighlighted; GENTIMDT__StartTimer_TB_10MS( TSTIFMGR__BUTTON_HIGHLIGHT_FLASH_10MS, &mTest.UserifTimingCounter );
	}
	//
	// Update the state of each button
	// - Button 01
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__01_PASV_BTN, TSTIFMGR__BN__01_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON01] );
	// - Button 02
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__02_PASV_BTN, TSTIFMGR__BN__02_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON02] );
	// - Button 03
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__03_PASV_BTN, TSTIFMGR__BN__03_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON03] );
	// - Button 04
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__04_PASV_BTN, TSTIFMGR__BN__04_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON04] );
	// - Button 05
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__05_PASV_BTN, TSTIFMGR__BN__05_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON05] );
	// - Button 06
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__06_PASV_BTN, TSTIFMGR__BN__06_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON06] );
	// - Button 07
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__07_PASV_BTN, TSTIFMGR__BN__07_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON07] );
	// - Button 08
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__08_PASV_BTN, TSTIFMGR__BN__08_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON08] );
	// - Button 09
	TestSequence_UpdateUserif_TFTTouchButtonDisplay( TSTIFMGR__BN__09_PASV_BTN, TSTIFMGR__BN__09_ACTV_BTN, mTest.TouchButton.aTestState[TSTIFMGR__BUTTON09] );
	//
	// Check any tag value pressed
	switch ( (TSTIFMGR__TAG_VALUE)TFTDSPIF__ucTouchTagValue() )
	{
		case TSTIFMGR__TV__01_BTN: Button = TSTIFMGR__BUTTON01; 	break;
		case TSTIFMGR__TV__02_BTN: Button = TSTIFMGR__BUTTON02; 	break;
		case TSTIFMGR__TV__03_BTN: Button = TSTIFMGR__BUTTON03; 	break;
		case TSTIFMGR__TV__04_BTN: Button = TSTIFMGR__BUTTON04; 	break;
		case TSTIFMGR__TV__05_BTN: Button = TSTIFMGR__BUTTON05; 	break;
		case TSTIFMGR__TV__06_BTN: Button = TSTIFMGR__BUTTON06; 	break;
		case TSTIFMGR__TV__07_BTN: Button = TSTIFMGR__BUTTON07; 	break;
		case TSTIFMGR__TV__08_BTN: Button = TSTIFMGR__BUTTON08; 	break;
		case TSTIFMGR__TV__09_BTN: Button = TSTIFMGR__BUTTON09; 	break;
		default:				   Button = 255;					break;				// Nothing pressed
	}

	NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();

	if ( Button != 255 )
	{
		if ( mTest.TouchButton.aTestState[Button] == TSTIFMGR__TBTS__TO_TEST )			// Is button pressed the one to test?
		{
			mTest.TouchButton.aTestState[Button] = TSTIFMGR__TBTS__TESTED;

			if ( Button != TSTIFMGR__BUTTON09 )
			{
				Button++; mTest.TouchButton.aTestState[Button] = TSTIFMGR__TBTS__TO_TEST;
			}

			mTest.TouchButton.bHighlighted = true; GENTIMDT__StartTimer_TB_10MS( TSTIFMGR__BUTTON_HIGHLIGHT_FLASH_10MS, &mTest.UserifTimingCounter );
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test UI Update - Two-wire bus.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_TwowireBus(void)
{
	UITFTDSP__BacklightControl_Update( false, true, 0 );

	NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BLACK, 0 ); NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Test UI Update - Clock.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_ClockVerify(void)
{
	UITFTDSP__BacklightControl_Update( false, true, 0 );

	NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate( UITFTDSP_COLOR__BLACK, 0 ); NWS_ACCESS_USERIF_NC7__ActionAfterUpdate();
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	How to display the button during test.
 * @param:  None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TestSequence_UpdateUserif_TFTTouchButtonDisplay(TSTIFMGR__BUTTON_NUMBER vucPassiveIndex, TSTIFMGR__BUTTON_NUMBER vucActiveIndex, TSTIFMGR__TOUCH_BUTTON_TEST_STATE vTestState)
{
	switch ( vTestState )
	{
		case TSTIFMGR__TBTS__NOT_TESTED:
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[vucPassiveIndex] );
			break;

		case TSTIFMGR__TBTS__TESTED:
			UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[vucActiveIndex] );
			break;

		default:
			if ( mTest.TouchButton.bHighlighted )
			{
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[vucActiveIndex] );
			}
			else
			{
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[vucPassiveIndex] );
			}
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Starts a test.
 * @param:  vTestStep - Test step to start.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						StartTest(TSTIFMGR__TESTSTEP vTestStep)
{
	// Specifics
	mTest.TouchCal.TestAction = TSTIFMGR__TTA__IDLE;
	//
	// General
	mTest.StepStatus = TSTIFMGR__TSS__INITIATE; mTest.bStepPassed = false;

	GENTIMDT__StartTimer_TB_10MS( 0, &mTest.ActionTimingCounter );

	if ( mTest.Step == TSTIFMGR__TS__NOTESTACTN )
	{
		UITFTDSP__StartDisplayRefresh();
	}
	mTest.Step = vTestStep;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Marks test is being performed.
 * @param:  vucTestActionTimeInterval - Time interval to start timer with
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						IndicateTestIsBeingPerformed(uint8_t vucTestActionTimeInterval)
{
    GENTIMDT__StartTimer_TB_100MS( vucTestActionTimeInterval, &mTest.ActionTimingCounter );

	mTest.StepStatus = TSTIFMGR__TSS__PRFRMING;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Marks test as completed.
 * @param:  vbPassed - Pass/Fail indicator.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void   					IndicateTestCompleted(bool vbPassed)
{
	mTest.bStepPassed = vbPassed; mTest.StepStatus = TSTIFMGR__TSS__COMPLETE;

    return;
}
//*********************************************************************************************************************************************************************************
