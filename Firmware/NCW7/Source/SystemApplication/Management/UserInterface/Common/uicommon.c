//*********************************************************************************************************************************************************************************
// uicommon.c
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Release Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * Version:		N-C7  	   - v090
 * Date:		01/10/2020
 * Modifier:  	Emilio R. La Greca
 *
 * Original release.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include		<com_defn.h>
#include		<uicommon.h>
#include		<uitftdsp.h>
#include		<rtmclkif.h>
#include		<tempmsif.h>
#include		<genstrdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// String definition details - ** Array variables as text to display will vary so define here for pointer inclusion in text table **
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Large number text
// - Max length allowed
#define			UICOMMON__LARGE_NUMBER_LENGTH				2
// - Actual container for string
static char				mstrLargeNumberText[UICOMMON__LARGE_NUMBER_LENGTH+1];
// - String definition
static GENSTRDT__String	mLargeNumberString;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Day of week text
// - Max length allowed
#define			UICOMMON__DAY_OF_WEEK_TEXT_LENGTH			10
// - Actual container for string
static char 			mstrDayOfWeekText[UICOMMON__DAY_OF_WEEK_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mDayOfWeekString;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Time display text
// - Max length allowed
#define			UICOMMON__TIME_TEXT_LENGTH					10
// - Actual container for string
static char 			mstrTimeText[UICOMMON__TIME_TEXT_LENGTH+1];
// - String definition
static GENSTRDT__String	mTimeString;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Text to display in top status area
// - Max length allowed
#define			UICOMMON__TEXT_TO_DISPLAY_LENGTH			100
// - Actual container for string
static char				mstrTextToDisplay[UICOMMON__TEXT_TO_DISPLAY_LENGTH+1];
// - String definition
static GENSTRDT__String	mTextToDisplayString;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Maximum days of week
#define			UICOMMON__MAX_DAYSOFWEEK		RTMCLKIF__MAX_DAYS_IN_WEEK
// Pointer array holds the memory locations where the text strings of all days of week are kept
static const char*		mkastrDayOfWeek[UICOMMON__MAX_DAYSOFWEEK] = {

	"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	UICOMMON__LARGE_FONT_NUMBER {

	UICOMMON__LFN__CROSS_MARK 		= 10

} UICOMMON__LARGE_FONT_NUMBER;
// Maximum number of large custom fonts
#define			UICOMMON__MAX_LARGE_FONTS		11
// Pointer array holds the memory locations where the large custom font 0-9 characters are kept - ** To be used with string functions characters must be in hex instead of as ascii characters **
static const char*		mkastrLargeFont[UICOMMON__MAX_LARGE_FONTS]	= {

	// '0',	   '1',	   '2',	   '3',	   '4',	   '5',	   '6',	   '7',	   '8',	   '9',	'X'
	"\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07", "\x08", "\x09", "\x0A", "\x0B"

};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[UI_COMMON__BOTTOMSTATUS_MAXIMAGES] = {

	// 				  			   ImageNumber,    ImageNumber_ActivePress,   X,   Y, Persistence time, TAG
	{ UITFTDSP_IMAGE__IN__COMMON__SPANNER_ICON,   UITFTDSP_IMAGE__NO_IMAGE, 345, 249, 				 0,  -1 },
	{ UITFTDSP_IMAGE__IN__COMMON__LOCK_ICON,      UITFTDSP_IMAGE__NO_IMAGE, 366, 249, 				 0,  -1 },
	{ UITFTDSP_IMAGE__IN__COMMON__HEART_ICON,     UITFTDSP_IMAGE__NO_IMAGE, 382, 249, 				 0,  -1 },
	{ UITFTDSP_IMAGE__IN__COMMON__M_ICON,         UITFTDSP_IMAGE__NO_IMAGE, 403, 249, 				 0,  -1 }

};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaText[]
typedef enum	UICOMMON__TEXT_NUMBER {

	UICOMMON__TN__TIME_12HR_FORMAT		=  0,
	UICOMMON__TN__TIME_24HR_FORMAT		=  1,
	UICOMMON__TN__CLOCK_AM_PASSIVE		=  2,
	UICOMMON__TN__CLOCK_AM_ACTIVE		=  3,
	UICOMMON__TN__CLOCK_PM_PASSIVE		=  4,
	UICOMMON__TN__CLOCK_PM_ACTIVE		=  5,
	UICOMMON__TN__DAY_OF_WEEK 			=  6,
	UICOMMON__TN__OUTDOOR_TEXT 			=  7,
	UICOMMON__TN__TEXT_TO_DISPLAY 		=  8,
	UICOMMON__TN__TEMPORARY_OVERRIDE	=  9

} UICOMMON__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails				mkaText[] = {

	// 	X, 	 Y,  XO,  YO, 			   			 Color, 		 	  	  Font Size, 	   	  	Justification,  String Label
	{  65,  23,  -1,  -1, 		 UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3,  UITFTDSP_TEXT__TJ__RIGHT, (char*)&mstrTimeText[0]			},		// Time 12 hour format
	{  50,  23,  -1,  -1, 		 UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__CENTER, (char*)&mstrTimeText[0]			},		// Time 24 hour format
	{  90,  23,  -1,  -1, UITFTDSP_COLOR__PASSIVE_TEXT, UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "AM"							},		// AM passive
	{  90,  26,  -1,  -1, 		 UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "AM"							},		// AM active
	{  90,  23,  -1,  -1, UITFTDSP_COLOR__PASSIVE_TEXT, UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "PM"							},		// PM passive
	{  90,  26,  -1,  -1, 		 UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_1,  UITFTDSP_TEXT__TJ__RIGHT, "PM"							},		// PM active
	{ 150,  23,  -1,  -1, 		 UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2, UITFTDSP_TEXT__TJ__CENTER, (char*)&mstrDayOfWeekText[0]	},		// Day of week
	{ 210,  23,  -1,  -1, 		 UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, "OD"							},		// Outdoor temperature
	{  15,  23,  -1,  -1, 		 UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrTextToDisplay[0]	},		// Text to display
	{ 270, 256,  -1,  -1, UITFTDSP_COLOR__PASSIVE_TEXT, UITFTDSP_FONTS__FS__ROUND_1,   UITFTDSP_TEXT__TJ__LEFT, "Temporary"						}		// Temporary

};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Rectangle Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaRectangle[]
typedef enum	UICOMMON__RECTANGLE_NUMBER {

	UICOMMON__RN__TOPSTATUS_BACKGROUND	= 	0,
	UICOMMON__RN__TOPSTATUS_DIVIDER_1	= 	1,
	UICOMMON__RN__TOPSTATUS_DIVIDER_2	= 	2

} UICOMMON__RECTANGLE_NUMBER;
//
// Array of rectangles
static const UI_TFTSHAPE__Rectangle					mkaRectangle[] = {

	// 				   					Color, 	  X,   Y,   W,   H,  Line Width
	{ 		  UITFTDSP_COLOR__TOP_STATUS_LINE,    0,   0, 480,  46,  1 			},		// Top status background
	{ UITFTDSP_COLOR__TOP_STATUS_MENU_DIVIDER,  100,   7,   0,  32,  1			},		// Menu divider 1
	{ UITFTDSP_COLOR__TOP_STATUS_MENU_DIVIDER,  200,   7,   0,  32,  1			}		// Menu divider 2

};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Temperature details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	UICOMMON__TEMPERATURE_NUMBER {		// Temperature Number - Use as index to : mkaTemperature[]

	UICOMMON__TN__OUTDOOR_TEMPERATURE		=  0,
	UICOMMON__TN__ROOM_TEMPERATURE			=  1

} UICOMMON__TEMPERATURE_NUMBER;
//
// Temperature containers - Defined here to include in table
static uint8_t		mucOutdoorTemperature;
static uint8_t		mucRoomTemperature;

static const UICOMMON__TemperatureDetails	  		mkaTemperature[] = {

	// NUMBER																									    	  	 	// POINT													   	   // COLOR
	//    X,   Y,			      Color,		 		 	 FontSize,		   	   Justification, 	Pointer to number		    //		   	 	   Color,   X,   Y, XO1, YO1, XO2, YO2, PointSize  Background color
	{ { 240,  23, UITFTDSP_COLOR__WHITE,  UITFTDSP_FONTS__FS__ROUND_2, 	 UITFTDSP_TEXT__TJ__LEFT, 	&mucOutdoorTemperature 	  }, { UITFTDSP_COLOR__WHITE,  -1,  -1,  14,  -4,  23,  -4,  3 		}, UITFTDSP_COLOR__TOP_STATUS_LINE  },
	{ { 160,  85, UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__CUSTOM_0, UITFTDSP_TEXT__TJ__CENTER, 	&mucRoomTemperature 	  }, { UITFTDSP_COLOR__WHITE,  -1,  -1,  49,  13,  78,  13, 15 		}, UITFTDSP_COLOR__BACKGROUND 		}

};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static UICOMMON__BottomStatusDetails 	mBottomStatus;
static bool								mbShowBottomStatus;

static bool								mbMasterModeSwitch;
static GENTIMDT__TimingCounter			mModeSwitchDelayTimeCounter;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	UICOMMON__kModeSwitchDelayTiming = { &mModeSwitchDelayTimeCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void						TopStatus_ShowClock(bool vbIn12HrFormat,  uint8_t vucHour, uint8_t vucMinute, GENSTRDT__String* pTime);
static void						TopStatus_ShowOutdoorTemperature(bool vbShowTemperature, UICOMMON__TemperatureDetails* pODTemperature);
static void    					ShowTemperature(UICOMMON__TemperatureDetails* pTemperature);
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
 * @brief:	Prepares the display list the initial commands queued to clear the display to the background color.
 * @param:	vulBackgroundColor - Background color to display (used for test).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__TFTDisplayList_Init_NormalOperation(uint32_t vulBackgroundColor)
{
	uint8_t		ucColorR, ucColorG, ucColorB;
	//
	// Define color components
	ucColorR = (uint8_t)( vulBackgroundColor >> 16 );
	ucColorG = (uint8_t)( vulBackgroundColor >> 8  );
	ucColorB = (uint8_t)( vulBackgroundColor >> 0  );
	//
	// Define mode : Queue then transfer mode
	FT800_COPRCCMD__CPCmd__DefineAccessType( FT800_COPRCCMD__BAT_QUEUE_XFER );
	//
	// Queue display list
	FT800_COPRCCMD__CPCmdQueue__CMD_DLSTART();
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB( ucColorR, ucColorG, ucColorB ) );
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR( 1, 1, 1 ) );
	//
    // Clear tag mask so as to set tag buffer to the default value 0
    FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK( 0 ) );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Prepares the display list the initial commands queued to clear the display to the background color.
 * @param:	vulBackgroundColor - Background color to display (used for test).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__TFTDisplayList_Init_DisplayShutdown(void)
{
	uint32_t	ulColor = UITFTDSP_COLOR__BLACK;
	//
	// Set the background color
	uint8_t	ucColorR = (uint8_t)( ulColor >> 16 );
	uint8_t	ucColorG = (uint8_t)( ulColor >> 8 );
	uint8_t	ucColorB = (uint8_t)( ulColor >> 0  );
	//
	// Define mode : Queue then transfer mode
	FT800_COPRCCMD__CPCmd__DefineAccessType( FT800_COPRCCMD__BAT_QUEUE_XFER );
	//
	// Queue display list
	FT800_COPRCCMD__CPCmdQueue__CMD_DLSTART();
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB( ucColorR, ucColorG, ucColorB ) );
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__CLEAR( 1, 1, 1 ) );

    FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK(0) );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Completes writing display list and begins transfer of the queue commands to the FT800.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		UICOMMON__TFTDisplayList_Execute(FUNC_ACTION vAction)
{
	static bool	sbInit = true;

	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	if ( sbInit )
	{
		// Queue remaining commands to complete display list
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__DISPLAY() );
		FT800_COPRCCMD__CPCmdQueue__CMD_SWAP( FT800_DISPLIST__DL_SWAP_TYPE_FRAME );
		// Only do this once until xfer completed
		sbInit = false;
	}

	// Issue the display list
	if ( ( XferStatus = FT800_COPRCCMD__CPCmd__IssueQueue( vAction ) ) != FT800_DEVICEIF__XS_ACTV )
	{
		sbInit = true;
	}

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises top status structure.
 * @param:	pTopStatus - Pointer to top status application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__TopStatus_InitStructure(UICOMMON__TopStatusDetails* pTopStatus)
{
	pTopStatus->pClock					= 0;
	pTopStatus->pstrTextToDisplay		= 0;
	pTopStatus->pucOutdoorTemperature   = 0;
	pTopStatus->bFlashText		 		= false;
	pTopStatus->pImage1         		= 0;
	pTopStatus->pImage2         		= 0;
	pTopStatus->pImage3         		= 0;
	pTopStatus->pImage4         		= 0;		// Image 4 pointer init, NC7090_DFDISP
	pTopStatus->pImage5         		= 0;		// Image 5 pointer init, NC7090_DFDISP

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates top status display area.
 * @param:	pTopStatus - Pointer to top status application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__TopStatus_Update(UICOMMON__TopStatusDetails* pTopStatus)
{
	// Show top status background
	UITFTDSP_SHAPE__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[UICOMMON__RN__TOPSTATUS_BACKGROUND] );

	if ( pTopStatus->pClock != 0 )
	{
	    // Time details
	    // - Init string
	    GENSTRDT__InitString( UICOMMON__TIME_TEXT_LENGTH, &mstrTimeText[0], &mTimeString );
	    // - Show the current time
	    TopStatus_ShowClock( pTopStatus->bClockIn12HourFormat, pTopStatus->pClock->ucCurrentHour, pTopStatus->pClock->ucCurrentMinute, &mTimeString );
		//
		// Day of week details
		// - Initialise string used for text display
		GENSTRDT__InitString( UICOMMON__DAY_OF_WEEK_TEXT_LENGTH , &mstrDayOfWeekText[0], &mDayOfWeekString );
		// - Update string
		GENSTRDT__AssignTextToString( (char*)mkastrDayOfWeek[pTopStatus->pClock->CurrentDay], &mDayOfWeekString );
		// - Show the current day
		UITFTDSP__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__DAY_OF_WEEK] );
		//
		// Outdoor temperature details
		// - Update container
		mucOutdoorTemperature = *( pTopStatus->pucOutdoorTemperature );
		// - Show outdoor temperature
		TopStatus_ShowOutdoorTemperature( ( pTopStatus->pucOutdoorTemperature != 0 ), (UICOMMON__TemperatureDetails*)&mkaTemperature[UICOMMON__TN__OUTDOOR_TEMPERATURE] );
		//
		// Show dividers
		UITFTDSP_SHAPE__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[UICOMMON__RN__TOPSTATUS_DIVIDER_1] );
		UITFTDSP_SHAPE__ShowRectangle( (UI_TFTSHAPE__Rectangle*)&mkaRectangle[UICOMMON__RN__TOPSTATUS_DIVIDER_2] );
	}
	else
	if ( pTopStatus->pstrTextToDisplay != 0 )
	{
	    // Text to display details
		// - Initialise string used for text display
		GENSTRDT__InitString( UICOMMON__TEXT_TO_DISPLAY_LENGTH , &mstrTextToDisplay[0], &mTextToDisplayString );
		// - Update string
		GENSTRDT__AssignTextToString( pTopStatus->pstrTextToDisplay, &mTextToDisplayString );
		// - Update display
		if ( pTopStatus->bFlashText )
		{
			// Flash the text
			UITFTDSP__FlashTextAbsoluteWithCenteredY( pTopStatus->TextFlashRate, (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__TEXT_TO_DISPLAY] );
		}
		else
		{
			// Show the text
			UITFTDSP__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__TEXT_TO_DISPLAY] );
		}
	}

	if ( pTopStatus->pImage1 != 0 )
	{
		// Show image 1 with associated tag
		UITFTDSP_IMAGE__ShowImageWithAssociatedTag( pTopStatus->pImage1 );
	}

	if ( pTopStatus->pImage2 != 0 )
	{
		// Show image 2 with associated tag
		UITFTDSP_IMAGE__ShowImageWithAssociatedTag( pTopStatus->pImage2 );
	}

	if ( pTopStatus->pImage3 != 0 )
	{
		// Show image 3 with associated tag
		UITFTDSP_IMAGE__ShowImageWithAssociatedTag( pTopStatus->pImage3 );
	}
	//
	// Image 4 update, NC7090_DFDISP
	if ( pTopStatus->pImage4 != 0 )
	{
		// Show image 4 with associated tag
		UITFTDSP_IMAGE__ShowImageWithAssociatedTag( pTopStatus->pImage4 );
	}
	//
	// Image 5 update, NC7090_DFDISP
	if ( pTopStatus->pImage5 != 0 )
	{
		// Show image 5 with associated tag
		UITFTDSP_IMAGE__ShowImageWithAssociatedTag( pTopStatus->pImage5 );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises bottom status structure.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__BottomStatus_InitStructure(void)
{
	uint8_t	ucIndex = 0;
	// Init text
	mBottomStatus.pText = 0;
	// Init images
	for ( ucIndex = 0; ucIndex < UI_COMMON__BOTTOMSTATUS_MAXIMAGES; ucIndex++ )
	{
		mBottomStatus.aImage[ucIndex].pDetails = 0;
		mBottomStatus.aImage[ucIndex].bFlashImage = false;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates bottom status display area.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__BottomStatus_Update(void)
{
	uint8_t	ucIndex = 0;
	//
	// Check if it needs to show bottom status
	if ( mbShowBottomStatus )
	{
		//
		// Update text
		if ( mBottomStatus.pText != 0 )
		{
			// Flash text
			UITFTDSP__FlashTextAbsoluteWithCenteredY( UITFTDSP__FR__1HZ, mBottomStatus.pText );
		}
		// Update images
		for ( ucIndex = 0; ucIndex < UI_COMMON__BOTTOMSTATUS_MAXIMAGES; ucIndex++ )
		{
			if ( mBottomStatus.aImage[ucIndex].pDetails != 0 )
			{
				if ( mBottomStatus.aImage[ucIndex].bFlashImage )
				{
					// Flash image
					UITFTDSP__FlashImage( mBottomStatus.aImage[ucIndex].FlashRate, mBottomStatus.aImage[ucIndex].pDetails );
				}
				else
				{
					// Show image
					UITFTDSP__ShowImage( mBottomStatus.aImage[ucIndex].pDetails );
				}
			}
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the bottom status display flag.
 * @param:	vbShowStauts - Flag indicates whether or not to show bottom status.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__ShowBottomStatus(bool vbShowStatus)
{
	mbShowBottomStatus = vbShowStatus;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates text at bottom status display area.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__BottomStatus_UpdateText(void)
{
	// Update bottom status text - temporary override
	mBottomStatus.pText = (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__TEMPORARY_OVERRIDE];
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates images at bottom status display area.
 * @param:	vIndex       - Index to image to update.
 * 			vbFlashImage - Flash image flag.
 * 			vFlashRate   - Flash rate.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__BottomStatus_UpdateImage(UI_COMMON__BOTTOMSTATUS_NUMBER vIndex, bool vbFlashImage, UI_TFTDISPLAY__FLASH_RATE vFlashRate)
{
	mBottomStatus.aImage[vIndex].pDetails  	 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[vIndex];
	mBottomStatus.aImage[vIndex].bFlashImage = vbFlashImage;
	mBottomStatus.aImage[vIndex].FlashRate	 = vFlashRate;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows room temperature on screen.
 * @param:	vucRoomTemperature - Room temperature to display.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UICOMMON__ShowRoomTemperature(uint8_t vucRoomTemperature)
{
	// Update room temperature container
	mucRoomTemperature = vucRoomTemperature;
	// Show room temperature
	ShowTemperature( (UICOMMON__TemperatureDetails*)&mkaTemperature[UICOMMON__TN__ROOM_TEMPERATURE] );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows number in large font on screen.
 * @param:	pNumber	- Pointer to number definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void 		   					UICOMMON__ShowLargeNumber(UI_TFTNUMBER__NumberDetails* pNumber)
{
	UITFTDSP_TEXT__TextDetails			LargeText;
	UITFTDSP_IMAGE_COMMON__ImageDetails	*pImageMetric, *pImageFont;
	//
	// Copy data
	LargeText.iX			= pNumber->iX;
	LargeText.iY			= pNumber->iY;
	LargeText.FontSize		= pNumber->FontSize;
	LargeText.ulColor		= pNumber->ulColor;
	LargeText.Justification	= pNumber->Justification;
	//
	// Image details
	// - Custom font metric block
	pImageMetric = UITFTDSP_IMAGE_COMMON__pImage( UITFTDSP_IMAGE_COMMON__IN__FONT_METRIC );
	// - Custom font bitmap raw data
	pImageFont   = UITFTDSP_IMAGE_COMMON__pImage( UITFTDSP_IMAGE_COMMON__IN__FONT_BMP );
	//
	// Begin bitmap command
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BEGIN( FT800_DISPLIST__BEGIN_PRIM__BITMAPS ) );
	// Set current color to white
	UITFTDSP__SetCurrentColor( UITFTDSP_COLOR__WHITE );
	// Assign parameter values
	// - Bitmap handle
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BITMAP_HANDLE(LargeText.FontSize) );
	// - Bitmap source address
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BITMAP_SOURCE( pImageFont->ulRAMStartAddx - pImageFont->pDefinition->uiLineStride * pImageFont->pDefinition->uiHeight ) );
	// - Bitmap layout
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BITMAP_LAYOUT( FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__L8, pImageFont->pDefinition->uiLineStride, pImageFont->pDefinition->uiHeight ) );
	// - Bitmap size
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BITMAP_SIZE( FT800_DISPLIST__BITMAP_SIZE_FILTER__NEAREST, FT800_DISPLIST__BITMAP_SIZE_WRAP__BORDER, FT800_DISPLIST__BITMAP_SIZE_WRAP__BORDER, pImageFont->pDefinition->uiWidth, pImageFont->pDefinition->uiHeight ) );
	// - Custom defined font setup
	FT800_COPRCCMD__CPCmdQueue__CMD_SETFONT( LargeText.FontSize, pImageMetric->ulRAMStartAddx );
	//
    // Large number details
	// - Initialise string used for text displays
	GENSTRDT__InitString( UICOMMON__LARGE_NUMBER_LENGTH , &mstrLargeNumberText[0], &mLargeNumberString );
	// - Update string
	if ( *(pNumber->pucNumber) == 255 )
	{
		// Number equal to 0xFF - show "XX"
		GENSTRDT__AddTextToString( (char*)mkastrLargeFont[UICOMMON__LFN__CROSS_MARK], &mLargeNumberString );
		GENSTRDT__AddTextToString( (char*)mkastrLargeFont[UICOMMON__LFN__CROSS_MARK], &mLargeNumberString );
	}
	else
	if ( *(pNumber->pucNumber) < 10 )
	{
		// Single digit
		GENSTRDT__AddTextToString( (char*)mkastrLargeFont[ *(pNumber->pucNumber) ], &mLargeNumberString );
	}
	else
	if ( *(pNumber->pucNumber) < 100 )
	{
		// Two digits number less than 100
		// - Update tens
		GENSTRDT__AddTextToString( (char*)mkastrLargeFont[ *(pNumber->pucNumber) / 10 ], &mLargeNumberString );
		// - Update units
		GENSTRDT__AddTextToString( (char*)mkastrLargeFont[ *(pNumber->pucNumber) % 10 ], &mLargeNumberString );
	}
	else
	{
		// Number greater than 99 - show "99"
		// - Update tens
		GENSTRDT__AddTextToString( (char*)mkastrLargeFont[9], &mLargeNumberString );
		// - Update units
		GENSTRDT__AddTextToString( (char*)mkastrLargeFont[9], &mLargeNumberString );
	}
	//
	// Update text structure string
	LargeText.pstrToDisplay = &mstrLargeNumberText[0];
	//
	// Show number
	UITFTDSP__ShowTextAbsolute(&LargeText);
	//
	// End bitmap command
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__END() );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the day of week string.
 * @param:	vDayOfWeek - Day of week numeric value.
 * @retval:	char*.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
char*						UICOMMON__pstrDayOfWeek(RTMCLKIF__DAY_OF_WEEK vDayOfWeek)
{
	if ( vDayOfWeek < UICOMMON__MAX_DAYSOFWEEK )
	{
		return (char*)mkastrDayOfWeek[vDayOfWeek];
	}
	else
	{
		return COM_DEFN__NULL;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Assigns the time passed to a string.
 * @param:	vbIn12HrFormat - Indicates whether or not time is in 12/24 hr format.
 * 			vucHour		   - The time hour.
 * 			vucMinute	   - The time minute.
 * 			pTime		   - Pointer to time string.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void						UICOMMON__AssignTimeToString(bool vbIn12HrFormat, uint8_t vucHour, uint8_t vucMinute, GENSTRDT__String* pTime)
{
	UI_COMMON__TIME_AMPM	TimeAmPm;
	//
	// Check format
	vucHour = UICOMMON__ucTimeHourAfterConversion( vbIn12HrFormat, vucHour, &TimeAmPm );
	//
	// Add the hour
	GENSTRDT__Add2DigitNumberToString( vucHour, false, pTime );
	// Add colon
	GENSTRDT__AddCharToString( ':', pTime );
	// Add the minutes
	GENSTRDT__Add2DigitNumberToString( vucMinute, true, pTime );
	//
	// AM/PM?
	switch ( TimeAmPm )
	{
		case UI_COMMON_TF_AM:
			GENSTRDT__AddCharToString( ' ', pTime ); GENSTRDT__AddTextToString( "AM", pTime );
			break;

		case UI_COMMON_TF_PM:
			GENSTRDT__AddCharToString( ' ', pTime ); GENSTRDT__AddTextToString( "PM", pTime );
			break;

		default:
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Converts the hours in time according to 12 hr format flag.
 * @param:	vbIn12HrFormat - Indicates whether or not time is in 12/24 hr format.
 * 			vucHour		   - The value to convert.
 * 			pTimeAmPm	   - Pointer to variable holding AM/PM result (None if 24 hr).
 * @retval:	The converted hours.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							UICOMMON__ucTimeHourAfterConversion(bool vbIn12HrFormat, uint8_t vucHour, UI_COMMON__TIME_AMPM* pTimeAmPm)
{
	if ( vbIn12HrFormat )
	{
		// Define if AM/PM
		if ( vucHour > 11 )
		{
			*pTimeAmPm = UI_COMMON_TF_PM;
		}
		else
		{
			*pTimeAmPm = UI_COMMON_TF_AM;
		}
		// Do the conversion
		if ( vucHour == 0 )
		{
			vucHour = 12;
		}
		else
		if ( vucHour > 12 )
		{
			vucHour -= 12;
		}
	}
	else
	{
		*pTimeAmPm = UI_COMMON_TF_NONE;
	}

	return vucHour;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the current climate control mode being accessed in the climate control operational state on expiry of the mode switch delay timer. This ensures that mode
 * 			switching message is displayed prior to downloading mode specific images therefore, it resolves the problem of image corruption when switching modes via a master
 * 			networker.
 * @param:	vMode - Current climate control mode.
 * @retval:	Current climate control mode.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
NWSUTILS__CLIMATE_CONTROL_MODE	UICOMMON__CurrentClimateControlMode(NWSUTILS__CLIMATE_CONTROL_MODE vMode)
{
	if ( mModeSwitchDelayTimeCounter.ulValue == 0 )
	{
		// Timer has expired - mode switched over
		// - Clear master mode switch flag
		mbMasterModeSwitch = false;
		// - Update current climate control mode
		return NWSUTILS__CurrentClimateControlMode();
	}
	else
	{
		// Mode being switched - no update
		return vMode;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Starts master networker mode switch delay timer.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__StartModeSwitchDelayTimer(void)
{
	GENTIMDT__StartTimer_TB_UIBased_1SEC( 2, &mModeSwitchDelayTimeCounter );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns master networker mode switch status.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UICOMMON__bMasterModeSwitch(void)
{
	return mbMasterModeSwitch;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Sets master networker mode switch.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UICOMMON__SetMasterModeSwitch(void)
{
	mbMasterModeSwitch = true;
	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the clock at top status display area.
 * @param:	vbIn12HrFormat - 12 hr format flag.
 * 			vucHour		   - The time hour.
 * 			vucMinute	   - The time minute.
 * 			pTime		   - Pointer to time string.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TopStatus_ShowClock(bool vbIn12HrFormat,  uint8_t vucHour, uint8_t vucMinute, GENSTRDT__String* pTime)
{
	UI_COMMON__TIME_AMPM	TimeAmPm;
	//
	// Do any required conversion on the hour
	vucHour = UICOMMON__ucTimeHourAfterConversion( vbIn12HrFormat, vucHour, &TimeAmPm );
	//
	// Add the hour
	GENSTRDT__Add2DigitNumberToString( vucHour, false, pTime );
	// Add flashing colon
	UITFTDSP__AddFlashingCharacterToString( UITFTDSP__FR__2HZ, ':', pTime );
	// Add the minutes
	GENSTRDT__Add2DigitNumberToString( vucMinute, true, pTime );

	switch ( TimeAmPm )
	{
		case UI_COMMON_TF_AM:
			// AM - AM symbol active
			UITFTDSP__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__CLOCK_AM_ACTIVE] );
			break;

		case UI_COMMON_TF_PM:
			// PM - PM symbol active
			UITFTDSP__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__CLOCK_PM_ACTIVE] );
			break;

		default:
			break;
	}
	//
	// Show current time
	if ( vbIn12HrFormat )
	{
		// 12 hour format
		UITFTDSP__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__TIME_12HR_FORMAT] );
	}
	else
	{
		// 24 hour format
		UITFTDSP__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__TIME_24HR_FORMAT] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates outdoor temperature at top status display area.
 * @param:	vbShowTemperature - Flag indicates whether or not to show outdoor temperature.
 * 			pODTemperature	  - Pointer to outdoor temperature application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						TopStatus_ShowOutdoorTemperature(bool vbShowTemperature, UICOMMON__TemperatureDetails* pODTemperature)
{
	uint8_t 	ucTemperature;
	//
	// If allowed show outdoor temperature
	if ( vbShowTemperature )
	{
		// Get the current temperature to update
		ucTemperature = *pODTemperature->Temperature.pucNumber;
		//
		// Show OD text
		UITFTDSP__ShowTextAbsoluteWithCenteredY( (UITFTDSP_TEXT__TextDetails*)&mkaText[UICOMMON__TN__OUTDOOR_TEXT] );
		//
		// Check if is in celsius or fahrenheit
		if ( !TWBIFUTL__bTemperatureDisplayFormatInCelsius() )
		{
			// Update temperature after conversion
			*pODTemperature->Temperature.pucNumber = TEMPMSIF__ucTemperatureInFahrenheit( ucTemperature );
		}
		//
		// Show outdoor temperature with degree symbol
		ShowTemperature( pODTemperature );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows temperature on screen.
 * @param:	pTemperature - Pointer to temperature definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void    					ShowTemperature(UICOMMON__TemperatureDetails* pTemperature)
{
	UI_TFTSHAPE__PointDetails		OuterPoint, InnerPoint;
	//
	// Copy data
	// - Colors
	OuterPoint.ulColor 	   = pTemperature->DegSymbol.ulColor;
	InnerPoint.ulColor	   = pTemperature->ulBGColor;
	// - Sizes
	OuterPoint.ulPointSize = pTemperature->DegSymbol.ulPointSize;
	InnerPoint.ulPointSize = OuterPoint.ulPointSize / 2 + 1;
	// - Outer point posn
	if ( *(pTemperature->Temperature.pucNumber) < 10 )
	{
		OuterPoint.iX = pTemperature->Temperature.iX + pTemperature->DegSymbol.iXOffset1;
		OuterPoint.iY = pTemperature->Temperature.iY + pTemperature->DegSymbol.iYOffset1;
	}
	else
	{
		OuterPoint.iX = pTemperature->Temperature.iX + pTemperature->DegSymbol.iXOffset2;
		OuterPoint.iY = pTemperature->Temperature.iY + pTemperature->DegSymbol.iYOffset2;
	}
	// - Inner point posn
	InnerPoint.iX = OuterPoint.iX;
	InnerPoint.iY = OuterPoint.iY;
	//
	// Show temperature
	if ( pTemperature->Temperature.FontSize < UITFTDSP_FONTS__FS__BOLD_1 )
	{
		// Custom large font
		UICOMMON__ShowLargeNumber( &pTemperature->Temperature );
	}
	else
	{
		// System font
		UITFTDSP__ShowNumber( &pTemperature->Temperature );
	}
	//
	// Show degree symbol
	UITFTDSP__ShowDegreeSymbol( &OuterPoint, &InnerPoint );

	return;
}
//*********************************************************************************************************************************************************************************
