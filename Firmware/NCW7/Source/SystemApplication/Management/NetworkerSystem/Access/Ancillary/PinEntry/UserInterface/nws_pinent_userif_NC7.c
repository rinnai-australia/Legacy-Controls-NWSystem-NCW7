//*********************************************************************************************************************************************************************************
// nws_pinent_userif_NC7.c
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
#include		<nws_pinent_common.h>
#include		<uicommon.h>
#include		<uitftdsp.h>
#include		<uitftdsp_button.h>
#include		<uitftdsp_color.h>
#include		<uitftdsp_shape.h>
#include		<uitftdsp_fonts.h>
#include		<tftdspif.h>
#include		<genstrdt.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Keyboard related constants
// - Font size used for all buttons
#define			NWS_PINENT_USERIF_NC7__FONT_SIZE				UITFTDSP_FONTS__FS__ROUND_3
// - Y offset for first row
#define			NWS_PINENT_USERIF_NC7__YOFFSET					85
// - X offset
#define			NWS_PINENT_USERIF_NC7__XOFFSET					225
// - Button height
#define			NWS_PINENT_USERIF_NC7__BTN_H					40
// - Button width : Standard
#define			NWS_PINENT_USERIF_NC7__BTN_W					80
//
// Cursor flash time
#define			NWS_PINENT_USERIF_NC7__CURSOR_FLASH_TIME_UITB	(24 / GENTIMDT__kucUI_TIMEBASE_10MS)
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_PINENT_USERIF_NC7__TAG_VALUE {

	NWS_PINENT_USERIF_NC7__TV__HELP_BTN	  	= 1,
	NWS_PINENT_USERIF_NC7__TV__BACK_BTN	  	= 2,

	NWS_PINENT_USERIF_NC7__TV__BSP			= 29,
	NWS_PINENT_USERIF_NC7__TV__OK			= 31

} NWS_PINENT_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_PIENT_USERIF_NC7__IMAGE_NUMBER {

	NWS_PINENT_USERIF_NC7__IN__HELP_BTN =  0,
	NWS_PINENT_USERIF_NC7__IN__BACK_BTN =  1

} NWS_PINENT_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PINENT_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PINENT_USERIF_NC7__TV__BACK_BTN  }, // 1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PINENT_USERIF_NC7__TV__HELP_BTN  }, // 0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_PINENT_USERIF_NC7__TV__BACK_BTN  }, // 1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Alpha Keypad - Numbers
//
//			---------------
//			| 1  | 2 |  3  |
//			| 4  | 5 |  6  |
//			| 7  | 8 |  9  |
//  		| Ok | 0 | DEL |
//			---------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			NWS_PINENT_USERIF_NC7__BUTTONS_MAX_NUMBERS	12

static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton_PinEntryKeypad[NWS_PINENT_USERIF_NC7__BUTTONS_MAX_NUMBERS] =
{		// BUTTON																										   							    RECTANGLE						   																																										                								TEXT
		//                Normal State,   HL,         	  		           Persist time, 	   			        Style, 							  Tag,		       Color,   																               X,   															              Y,                            W,   			               Y, LW  	     X,   Y,  XO,  YO, Color,          		          FontSize,		     Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '1', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (0 * 2) + (0 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (0 * 2) + (0 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "1" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '2', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (1 * 2) + (1 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (0 * 2) + (0 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "2" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '3', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (2 * 2) + (2 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (0 * 2) + (0 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "3" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '4', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (0 * 2) + (0 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (1 * 2) + (1 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "4" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '5', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (1 * 2) + (1 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (1 * 2) + (1 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "5" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '6', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (2 * 2) + (2 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (1 * 2) + (1 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "6" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						  	  '7', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (0 * 2) + (0 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (2 * 2) + (2 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "7" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '8', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (1 * 2) + (1 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (2 * 2) + (2 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "8" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '9', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (2 * 2) + (2 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (2 * 2) + (2 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "9" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,  NWS_PINENT_USERIF_NC7__TV__OK,  {  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (0 * 2) + (0 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (3 * 2) + (3 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "OK" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, 						 	  '0', 	{  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (1 * 2) + (1 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (3 * 2) + (3 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "0" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST, UITFTDSP_BUTTON__STYLE_ROUND, NWS_PINENT_USERIF_NC7__TV__BSP,  {  {           0, NWS_PINENT_USERIF_NC7__XOFFSET +  0 + (2 * 2) + (2 * NWS_PINENT_USERIF_NC7__BTN_W), NWS_PINENT_USERIF_NC7__YOFFSET + (3 * 2) + (3 * NWS_PINENT_USERIF_NC7__BTN_H), NWS_PINENT_USERIF_NC7__BTN_W,  NWS_PINENT_USERIF_NC7__BTN_H, 0  },  {  -1,  -1,  -1,  -1,     0, NWS_PINENT_USERIF_NC7__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "DEL" 	 } } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text in rectangle details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define text entry array here for inclusion in table!
// - +2 to allow for flashing cursor (This is text to be displayed)
static char		mstrPinTextEntry[NWS_PINENT_COMMON__MAX_PIN_LENGTH+2];
//
// Number - Use as index to mkaTextEntry[]
typedef enum	NWS_PINENT_USERIF_NC7__TEXTRECT_NUMBER {

	NWS_PINENT_USERIF_NC7__TRN__KBD_ENTRY = 0,

} NWS_PINENT_USERIF_NC7__TEXTRECT_NUMBER;
//
// Array of text entries
static const UI_TFTSHAPE__RectangleWithTextDetails	mkaTextEntry[] = {

	// Rectangle														         							            Text
	// 			         Color,   X,                                 Y,    W,                               H,   LW        X,   Y,  XO,  YO,                  Color,          	        FontSize,            Justification,   Text to display
	{  { UITFTDSP_COLOR__WHITE,  12,  NWS_PINENT_USERIF_NC7__YOFFSET+2,  200,  NWS_PINENT_USERIF_NC7__BTN_H-4,   6 },   { -1,  -1,  10,  -1,  UITFTDSP_COLOR__BLACK, UITFTDSP_FONTS__FS__ROUND_3,  UITFTDSP_TEXT__TJ__LEFT,   (char*)&mstrPinTextEntry[0] } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Containers
// - Max length allowed
#define			NWS_PINENT_USERIF_NC7__MAX_PIN_DESC_LENGTH		32
// - Actual container for string
static char				mstrPinDescription[NWS_PINENT_USERIF_NC7__MAX_PIN_DESC_LENGTH+1];
// - String definition
static GENSTRDT__String	mPinDescription;
//
// Number - Use as index to mkaText[]
typedef enum	NWS_PINENT_USERIF_NC7__TEXT_NUMBER {

	NWS_PINENT_USERIF_NC7__TN_PIN_DESC		= 0,
	NWS_PINENT_USERIF_NC7__TN_PINERROR      = 1

} NWS_PINENT_USERIF_NC7__TEXT_NUMBER;
//
// Array of texts
static const UITFTDSP_TEXT__TextDetails		mkaText[] = {

	//  X,                                  Y,  XO,  YO,                  Color,              		    Font,             Justification, String Label
	{  12,  NWS_PINENT_USERIF_NC7__YOFFSET-30,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, (char*)&mstrPinDescription[0]  }, // 0
	{  12,  NWS_PINENT_USERIF_NC7__YOFFSET+55,  -1,  -1,  UITFTDSP_COLOR__WHITE, UITFTDSP_FONTS__FS__ROUND_2,   UITFTDSP_TEXT__TJ__LEFT, "Invalid PIN - Try again" 		}, // 1
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	NumericKeyboardDetails {						// Numeric Keyboard Details
																//
	GENSTRDT__String			NumberDisplay;					// - String container for text to be displayed
	GENSTRDT__String			NumberEntered;					// - String container for number being entered
																//
	bool						bShowCursor;					// - Show cursor flag
	GENTIMDT__TimingCounter		FlashCursorTimingCounter;		// - Cursor flash timer
																//
} NumericKeyboardDetails;										//
																//
static NumericKeyboardDetails	mNumericKeyboard;				// Container
//
// Placeholder for actual numbers entered
static char						mstrPinEntered[NWS_PINENT_COMMON__MAX_PIN_LENGTH+1];
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	NWS_PINENT_USERIF_NC7__kNumericKeyboardFlashCursorTiming = { &mNumericKeyboard.FlashCursorTimingCounter };
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
 * @param:	pPinEntry - Pointer to pin entry access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PINENT_USERIF_NC7__InitBeforeStateEntry(NWS_PINENT_COMMON__ControlDetails* pPinEntry)
{
	// Init String
	GENSTRDT__InitString( NWS_PINENT_COMMON__MAX_PIN_LENGTH+1, (char*)&mstrPinTextEntry[0], &mNumericKeyboard.NumberDisplay );
	//
	// Copy details from string passed
	// - Max length
	if ( pPinEntry->UserAccess.NumberEntered.uiMaxLength <= mNumericKeyboard.NumberDisplay.uiMaxLength )
	{
		mNumericKeyboard.NumberDisplay.uiMaxLength = pPinEntry->UserAccess.NumberEntered.uiMaxLength;
	}
	mNumericKeyboard.NumberDisplay.uiMaxLength++;	// +1 allows for cursor!
	// - Copy over
	GENSTRDT__CopyGENString( &pPinEntry->UserAccess.NumberEntered, &mNumericKeyboard.NumberDisplay );
	//
	// Init String
	GENSTRDT__InitString( NWS_PINENT_COMMON__MAX_PIN_LENGTH, (char*)&mstrPinEntered[0], &mNumericKeyboard.NumberEntered );
	//
	// Copy details from string passed
	// - Max length
	if ( pPinEntry->UserAccess.NumberEntered.uiMaxLength <= mNumericKeyboard.NumberEntered.uiMaxLength )
	{
		mNumericKeyboard.NumberEntered.uiMaxLength = pPinEntry->UserAccess.NumberEntered.uiMaxLength;
	}
	// - Copy over
	GENSTRDT__CopyGENString( &pPinEntry->UserAccess.NumberEntered, &mNumericKeyboard.NumberEntered );
	//
	// Pin Description
	GENSTRDT__InitString( NWS_PINENT_USERIF_NC7__MAX_PIN_DESC_LENGTH, (char*)&mstrPinDescription[0], &mPinDescription );
	//
	// Reset cursor flash time
	mNumericKeyboard.bShowCursor = false; GENTIMDT__StartTimer_TB_UIBased( 0, &mNumericKeyboard.FlashCursorTimingCounter );

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Pin entry display update - NC7 specific.
 * @param:	pPinEntry - Pointer to pin entry access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PINENT_USERIF_NC7__UpdateDisplay(NWS_PINENT_COMMON__ControlDetails* pPinEntry)
{
	UICOMMON__TopStatusDetails	TopStatus;
	uint8_t						ucIndex;
    //
    // Top status line update
    // - Init structure
	UICOMMON__TopStatus_InitStructure( &TopStatus );
	// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PINENT_USERIF_NC7__IN__HELP_BTN];
#endif
    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_PINENT_USERIF_NC7__IN__BACK_BTN];
	TopStatus.pstrTextToDisplay = pPinEntry->pAppInterface->pstrTopStatusText;
    // - Perform update
	UICOMMON__TopStatus_Update( &TopStatus );
	//
	// Pin Description
	// - Assign it
	GENSTRDT__AssignTextToString( pPinEntry->pAppInterface->pstrPinDescription, &mPinDescription );
	// - Display it
	UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PINENT_USERIF_NC7__TN_PIN_DESC] );
	//
	// Invalid pin prompt
	if (pPinEntry->UserAccess.bInvalidPin)
	{
		// Show error string
		UITFTDSP__ShowTextAbsolute( (UITFTDSP_TEXT__TextDetails*)&mkaText[NWS_PINENT_USERIF_NC7__TN_PINERROR] );
		// Clear entries
		mNumericKeyboard.NumberEntered.uiLength = 0;
		mNumericKeyboard.NumberDisplay.uiLength = 0;
	}
	//
	// Keypad for number entry
	// - Check if cursor to be added to end of string
	if ( mNumericKeyboard.FlashCursorTimingCounter.ulValue == 0 )
	{
		mNumericKeyboard.bShowCursor = !mNumericKeyboard.bShowCursor;

		GENTIMDT__StartTimer_TB_UIBased( NWS_PINENT_USERIF_NC7__CURSOR_FLASH_TIME_UITB, &mNumericKeyboard.FlashCursorTimingCounter );
	}
	// - Replace with password char?
	for ( ucIndex = 0; ucIndex < mNumericKeyboard.NumberDisplay.uiLength; ucIndex++ )
	{
		mNumericKeyboard.NumberDisplay.pstrText[ucIndex] = '*';
	}
	// - Add cursor
	if ( mNumericKeyboard.bShowCursor )
	{
		GENSTRDT__AddCharToString( '|', &mNumericKeyboard.NumberDisplay );
	}
	// - Show numbers
	UITFTDSP__ShowRectangleWithText( (UI_TFTSHAPE__RectangleWithTextDetails*)&mkaTextEntry[NWS_PINENT_USERIF_NC7__TRN__KBD_ENTRY] );
	// - Remove cursor
	if ( mNumericKeyboard.bShowCursor )
	{
		GENSTRDT__RemoveCharFromString( &mNumericKeyboard.NumberDisplay );
	}
	//
	// - Always show numbers
	for ( ucIndex = 0; ucIndex < NWS_PINENT_USERIF_NC7__BUTTONS_MAX_NUMBERS; ucIndex++ )
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_PinEntryKeypad[ucIndex] );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Pin entry control update - NC7 specific.
 * @param:	pPinEntry - Pointer to pin entry access info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PINENT_USERIF_NC7__UpdateControl(NWS_PINENT_COMMON__ControlDetails* pPinEntry)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_PINENT_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_PINENT_USERIF_NC7__TV__BACK_BTN:
			// Back button
			sucActiveTag = ucTagValue;
			// No rapid update
			RapidUpdateControl_None();
			break;

   		case NWS_PINENT_USERIF_NC7__TV__HELP_BTN:
   			// TODO - Help button
   			RapidUpdateControl_None();
    		break;

		case NWS_PINENT_USERIF_NC7__TV__BSP:
			// Button - Backspace
			// - Remove char from entry
			GENSTRDT__RemoveCharFromString( &mNumericKeyboard.NumberDisplay );
			GENSTRDT__RemoveCharFromString( &mNumericKeyboard.NumberEntered );
			// - Cancel invalid flag
			pPinEntry->UserAccess.bInvalidPin = false;
			// - No rapid update
			RapidUpdateControl_None();
			break;

		case NWS_PINENT_USERIF_NC7__TV__OK:
			// Button Pressed - Done
			// - Save it to persist
			sucActiveTag = ucTagValue;
			// - No rapid update
			RapidUpdateControl_None();
			break;

		default:
			if ( ucTagValue != 0 )
			{
				// Must be one of the character buttons
				// - Cancel invalid flag
				pPinEntry->UserAccess.bInvalidPin = false;
				// - Add it to string
				if ( mNumericKeyboard.NumberDisplay.uiLength < ( mNumericKeyboard.NumberDisplay.uiMaxLength-1 ) )
				{
					GENSTRDT__AddCharToString( (char)ucTagValue, &mNumericKeyboard.NumberDisplay );
				}
				if ( mNumericKeyboard.NumberEntered.uiLength < mNumericKeyboard.NumberEntered.uiMaxLength )
				{
					GENSTRDT__AddCharToString( (char)ucTagValue, &mNumericKeyboard.NumberEntered );
				}
				// - No rapid update
				RapidUpdateControl_None();
			}
			break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )				// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (NWS_PINENT_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_PINENT_USERIF_NC7__TV__BACK_BTN:
					pPinEntry->UserAccess.Control.ButtonPressed = NWS_PINENT_COMMON__CB__CANCEL;
					break;

				case NWS_PINENT_USERIF_NC7__TV__OK:
					GENSTRDT__CopyGENString( &mNumericKeyboard.NumberEntered, &pPinEntry->UserAccess.NumberEntered );

					pPinEntry->UserAccess.Control.ButtonPressed = NWS_PINENT_COMMON__CB__OK;
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
