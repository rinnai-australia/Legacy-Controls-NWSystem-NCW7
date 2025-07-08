//*********************************************************************************************************************************************************************************
// uikeybrd.c
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
#include		<uitftdsp.h>
#include		<uitftdsp_button.h>
#include		<uitftdsp_color.h>
#include		<uitftdsp_shape.h>
#include		<uitftdsp_fonts.h>
#include		<genstrdt.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Type of keyboard to display
typedef enum	UIKEYBRD__KEYBOARD_DISPLAY_TYPE {

	UIKEYBRD__KDT__LOWER_CASE = 0,
	UIKEYBRD__KDT__UPPER_CASE = 1,
	UIKEYBRD__KDT__SYMBOLS    = 2

} UIKEYBRD__KEYBOARD_DISPLAY_TYPE;
//
// Keyboard related constants
// - Font size used for all buttons
#define			UIKEYBRD__FONT_SIZE					UITFTDSP_FONTS__FS__ROUND_2
// - Y offset for first row
#define			UIKEYBRD__YOFFSET					90
// - X offset
#define			UIKEYBRD__XOFFSET					6
// - Button height
#define			UIKEYBRD__BUTTON_H					33
// - Button width : Standard
#define			UIKEYBRD__BUTTON_W					45
// - Button width : Symbol/letter selection
#define			UIKEYBRD__SYMBOL_W					65
// - Button width : Space button
#define			UIKEYBRD__SPACE_W					334
// - Button width : Done button
#define			UIKEYBRD__DONE_W					65
// - Button posn  : Done button
#define			UIKEYBRD__DONE_X					(UIKEYBRD__SYMBOL_W + UIKEYBRD__SPACE_W)
//
// Tag values (Note that character tag values are actually character values)
// - Shift button
#define			UIKEYBRD__TAG_SHF					28
// - Backspace button
#define			UIKEYBRD__TAG_BSP					29
// - Symbol button
#define			UIKEYBRD__TAG_SYM					30
// - Done button
#define			UIKEYBRD__TAG_DNE					31
//
// Text cursor flash time
#define			UIKEYBRD__CURSOR_FLASH_TIME_UITB	(14/GENTIMDT__kucUI_TIMEBASE_10MS)
//
// Keypad buttons touch times
#define			UIKEYBRD__KEY_PRESS_FAST_TIME		(5 / GENTIMDT__kucUI_TIMEBASE_10MS)
#define			UIKEYBRD__KEY_PRESS_SLOW_TIME		(25 / GENTIMDT__kucUI_TIMEBASE_10MS)
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Alpha Keypad - Numbers
//
//			-----------------------------------------
//			| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 |
//			-----------------------------------------
//
#define			UIKEYBRD__BUTTONS_MAX_NUMBERS		10

static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton_KeyboardNumbers[UIKEYBRD__BUTTONS_MAX_NUMBERS] =
{
		// BUTTON																											// RECTANGLE						   																																			   // TEXT
		//                Normal State,   HL,         	  		ersist time, 	   			        Style, Tag,			    // Color,   														X,   												    Y,                  W,                   Y, LW  	   X,   Y,  XO,  YO, Color,            FontSize,		     Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '1',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "1" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '2',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "2" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '3',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "3" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '4',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "4" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '5',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "5" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '6',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "6" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '7',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "7" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '8',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "8" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '9',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "9" 		 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, '0',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (9 * 2) + (9 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (0 * 2) + (0 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "0" 		 } } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Alpha Keypad - Letters (uppercase)
//
//			-----------------------------------------
//			| Q | W | E | R | T | Y | U | I | O | P |
//			-----------------------------------------
//			  -------------------------------------
//			  | A | S | D | F | G | H | J | K | L |
//			  -------------------------------------
//		   --------------------------------------------
//		   | SHF | Z | X | C | V | B | N | M | / | <- |
//		   --------------------------------------------
//		   --------------------------------------------
//		   | ?#+ |            Space            | Done |
//		   --------------------------------------------
//
#define			UIKEYBRD__BUTTONS_MAX_LETTERS		32

static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton_KeyboardUppercaseLetters[UIKEYBRD__BUTTONS_MAX_LETTERS] =
{
		// BUTTON																															    // RECTANGLE						   																																			   // TEXT
		//                Normal State,   HL,         	  	   Persist time, 	   			        Style,					 Tag,			    // Color,   													    X,  												    Y,                  W,   			     Y, LW  	   X,   Y,  XO,  YO, Color,            FontSize,		     Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'Q',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Q" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'W',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "W" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'E',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "E" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'R',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "R" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'T',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "T" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'Y',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Y" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'U',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "U" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'I',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "I" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'O',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "O" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'P',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (9 * 2) + (9 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "P" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'A',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "A" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'S',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "S" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'D',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "D" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'F',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "F" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'G',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "G" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'H',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "H" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'J',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "J" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'K',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "K" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'L',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "L" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND,     UIKEYBRD__TAG_SHF,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Shift"  } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'Z',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Z" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'X',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "X" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'C',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "C" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'V',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "V" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'B',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "B" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'N',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "N" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'M',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "M" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '/',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "/" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND,     UIKEYBRD__TAG_BSP,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (9 * 2) + (9 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "DEL" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND,     UIKEYBRD__TAG_SYM,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H), UIKEYBRD__SYMBOL_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "?#+"	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 ' ',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__SYMBOL_W), UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H),  UIKEYBRD__SPACE_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Space"  } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_SLOW_TIME, UITFTDSP_BUTTON__STYLE_ROUND,     UIKEYBRD__TAG_DNE,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (UIKEYBRD__DONE_X)      , UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H),   UIKEYBRD__DONE_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Done" 	 } } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Alpha Keypad - Letters (lowercase)
//
//			-----------------------------------------
//			| q | w | e | r | t | y | u | i | o | p |
//			-----------------------------------------
//			  -------------------------------------
//			  | a | s | d | f | g | h | j | k | l |
//			  -------------------------------------
//		   --------------------------------------------
//		   | SHF | z | x | c | v | b | n | m | / | <- |
//		   --------------------------------------------
//		   --------------------------------------------
//		   | ?#+ |            Space            | Done |
//		   --------------------------------------------
//
static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton_KeyboardLowercaseLetters[UIKEYBRD__BUTTONS_MAX_LETTERS] =
{
		// BUTTON																																// RECTANGLE						   																																			   // TEXT
		//                Normal State,   HL,     		       Persist time, 	   			        Style,					 Tag,			    // Color,   													    X,   									  			    Y,                  W,   			     Y, LW  	   X,   Y,  XO,  YO, Color,            FontSize,		     Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'q',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "q" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'w',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "w" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'e',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "e" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'r',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "r" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 't',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "t" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'y',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "y" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'u',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "u" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'i',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "i" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'o',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "o" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'p',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (9 * 2) + (9 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "p" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'a',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "a" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 's',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "s" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'd',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "d" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'f',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "f" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'g',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "g" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'h',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "h" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'j',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "j" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'k',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "k" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'l',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "l" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 	   UIKEYBRD__TAG_SHF,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Shift"  } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'z',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "z" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'x',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "x" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'c',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "c" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'v',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "v" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     'b',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "b" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'n',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "n" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 'm',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "m" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '/',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "/" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 	   UIKEYBRD__TAG_BSP,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (9 * 2) + (9 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "DEL" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 	   UIKEYBRD__TAG_SYM,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H), UIKEYBRD__SYMBOL_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "?#+"	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 ' ',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__SYMBOL_W), UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H),  UIKEYBRD__SPACE_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Space"  } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_SLOW_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 	   UIKEYBRD__TAG_DNE,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (UIKEYBRD__DONE_X)      , UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H),   UIKEYBRD__DONE_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Done" 	 } } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Alpha Keypad - Symbols
//
//			-----------------------------------------
//			| ! | @ | # | $ | % | ^ | & | * | ( | ) |
//			-----------------------------------------
//			  -------------------------------------
//			  | - | _ | + | = | [ | ] | { | } | \ |
//			  -------------------------------------
//		   --------------------------------------------
//		   | SHF | | | ~ | < | > | , | . | / | ? | <- |
//		   --------------------------------------------
//		   --------------------------------------------
//		   | ABC |            Space            | Done |
//		   --------------------------------------------
//
#define			UIKEYBRD__BUTTONS_MAX_SYMBOLS		31

static const UITFTDSP_BUTTON__ButtonDefinition	mkaButton_KeyboardSymbols[UIKEYBRD__BUTTONS_MAX_SYMBOLS] =
{
		// BUTTON																																// RECTANGLE						   																																			   // TEXT
		//                Normal State,   HL,      		       Persist time, 	   			        Style,					 Tag,			    // Color,   													    X,   												    Y,                  W,   			     Y, LW  	   X,   Y,  XO,  YO, Color,            FontSize,		     Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '!',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "!" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '@',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "@" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '#',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "#" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '$',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "$" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '%',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "%" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '^',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "^" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '&',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "&" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '*',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "*" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '(',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "(" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 ')',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (9 * 2) + (9 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (1 * 2) + (1 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, ")" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '-',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "-" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '_',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "_" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '+',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "+" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '=',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "=" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '[',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "[" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 ']',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "]" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '{',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "{" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '}',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "}" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '\\',   	{  {           0, UIKEYBRD__XOFFSET + 22 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (2 * 2) + (2 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "\\" 	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '|',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "|" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     '~',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (1 * 2) + (1 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "~" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     '<',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (2 * 2) + (2 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "<" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     '>',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (3 * 2) + (3 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, ">" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 				     ',',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (4 * 2) + (4 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "," 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '.',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (5 * 2) + (5 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "." 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '/',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (6 * 2) + (6 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "/" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 '?',   	{  {           0, UIKEYBRD__XOFFSET + 45 + (7 * 2) + (7 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "?" 	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 	   UIKEYBRD__TAG_BSP,   	{  {           0, UIKEYBRD__XOFFSET + 45 + (8 * 2) + (8 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (3 * 2) + (3 * UIKEYBRD__BUTTON_H), UIKEYBRD__BUTTON_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "DEL"	 } } },

	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 	   UIKEYBRD__TAG_SYM,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (0 * 2) + (0 * UIKEYBRD__BUTTON_W), UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H), UIKEYBRD__SYMBOL_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "ABC"	 } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_FAST_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 					 ' ',   	{  {           0, UIKEYBRD__XOFFSET +  0 + (1 * 2) + (1 * UIKEYBRD__SYMBOL_W), UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H),  UIKEYBRD__SPACE_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Space"  } } },
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UIKEYBRD__KEY_PRESS_SLOW_TIME, UITFTDSP_BUTTON__STYLE_ROUND, 	   UIKEYBRD__TAG_DNE,   	{  {           0, UIKEYBRD__XOFFSET +  0 + (2 * 2) + (UIKEYBRD__DONE_X)      , UIKEYBRD__YOFFSET + (4 * 2) + (4 * UIKEYBRD__BUTTON_H),   UIKEYBRD__DONE_W,  UIKEYBRD__BUTTON_H, 0  },  {  -1,  -1,  -1,  -1,     0, UIKEYBRD__FONT_SIZE, UITFTDSP_TEXT__TJ__CENTER, "Done" 	 } } },
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Text details
// - Max characters that can be entered
#define			UIKEYBRD__TEXT_ENTRY_MAX_LENGTH		50
// - +2 to allow for flashing cursor
static char		mstrTextEntry[UIKEYBRD__TEXT_ENTRY_MAX_LENGTH+2];

typedef enum	UIKEYBRD__TEXT_NUMBER {

	UIKEYBRD__TN__KBD_ENTRY = 0,

} UIKEYBRD__TEXT_NUMBER;

static const UI_TFTSHAPE__RectangleWithTextDetails	mkaTextEntry[] = {

	// Rectangle														        Text
	// 			         Color,                  X,   Y,    W,   Y,   LW        X,   Y,  XO,  YO,                  Color,          	         FontSize,            Justification,   Text to display
	{  { UITFTDSP_COLOR__WHITE,  UIKEYBRD__XOFFSET,  50,  468,  36,   6 },   { -1,  -1,  10,  -1,  UITFTDSP_COLOR__BLACK, UITFTDSP_FONTS__FS__ROUND_2,  UITFTDSP_TEXT__TJ__LEFT,   (char*)&mstrTextEntry[0] } },
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	AlphaKeyboardDetails {									// Alpha Numeric Keyboard Details
																		//
	UIKEYBRD__KEYBOARD_DISPLAY_TYPE		DisplayType;					// - Current display type
	UIKEYBRD__KEYBOARD_DISPLAY_TYPE		LastDisplayType;				// - Last display being shown
																		//
	GENSTRDT__String					TextEntryString;				// - String container for text
																		//
	bool								bShowCursor;					// - Show cursor flag
	GENTIMDT__TimingCounter				FlashCursorTimingCounter;		// - Cursor flash timer
																		//
	bool								bLastKeyPressed_SHF;			// - Track when shift key pressed
																		//
} AlphaKeyboardDetails;													//
																		//
static AlphaKeyboardDetails	mAlphaKeyboard;								// Container
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	UIKEYBRD__kAlphaKeyboardFlashCursorTiming = { &mAlphaKeyboard.FlashCursorTimingCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void						RapidUpdateControl_Backspace(void);
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
 * @brief:	Initialises UI before first display of keyboard.
 * @param:	pString	- Pointer to string holding text to display in text entry box.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UIKEYBRD__InitBeforeDisplay(GENSTRDT__String* pString)
{
	// Init String
	GENSTRDT__InitString( UIKEYBRD__TEXT_ENTRY_MAX_LENGTH+1, (char*)&mstrTextEntry[0], &mAlphaKeyboard.TextEntryString );
	//
	// Copy details from string passed
	// - Max length
	if ( pString->uiMaxLength <= mAlphaKeyboard.TextEntryString.uiMaxLength )
	{
		mAlphaKeyboard.TextEntryString.uiMaxLength = pString->uiMaxLength;
	}
	mAlphaKeyboard.TextEntryString.uiMaxLength++;	// +1 allows for cursor!
	// - Copy over
	GENSTRDT__CopyGENString( pString, &mAlphaKeyboard.TextEntryString );
	//
	// Reset cursor flash time
	mAlphaKeyboard.bShowCursor = false; GENTIMDT__StartTimer_TB_UIBased( 0, &mAlphaKeyboard.FlashCursorTimingCounter );
	//
	// Start with lower case always
	mAlphaKeyboard.DisplayType     = UIKEYBRD__KDT__LOWER_CASE;
	mAlphaKeyboard.LastDisplayType = UIKEYBRD__KDT__LOWER_CASE;
	// No button pressed
	mAlphaKeyboard.bLastKeyPressed_SHF = false;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns a pointer to the keynoard text entry string.  Use this to gain access to the text entered by the user.
 * @param:	None.
 * @retval:	Pointer to GENSTRDT__String.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
GENSTRDT__String*				UIKEYBRD__pTextEntryString(void)
{
	return &mAlphaKeyboard.TextEntryString;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Alpha keyboard - Display update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UIKEYBRD__UpdateDisplay(void)
{
	uint8_t		ucIndex;
	//
	// Show text in entry box
	// - Check if cursor to be added to end of string
	if ( mAlphaKeyboard.FlashCursorTimingCounter.ulValue == 0 )
	{
		mAlphaKeyboard.bShowCursor = !mAlphaKeyboard.bShowCursor;

		GENTIMDT__StartTimer_TB_UIBased( UIKEYBRD__CURSOR_FLASH_TIME_UITB, &mAlphaKeyboard.FlashCursorTimingCounter );
	}
	// - Add cursor
	if ( mAlphaKeyboard.bShowCursor )
	{
		GENSTRDT__AddCharToString( '_', &mAlphaKeyboard.TextEntryString );
	}
	// - Show text
	UITFTDSP__ShowRectangleWithText( (UI_TFTSHAPE__RectangleWithTextDetails*)&mkaTextEntry[UIKEYBRD__TN__KBD_ENTRY] );
	// - Remove cursor
	if ( mAlphaKeyboard.bShowCursor )
	{
		GENSTRDT__RemoveCharFromString( &mAlphaKeyboard.TextEntryString );
	}
	//
	// Always show numbers
	for ( ucIndex = 0; ucIndex < UIKEYBRD__BUTTONS_MAX_NUMBERS; ucIndex++ )
	{
		UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_KeyboardNumbers[ucIndex] );
	}
	//
	// Uppercase, Lowercase or symbols?
	switch ( mAlphaKeyboard.DisplayType )
	{
		case UIKEYBRD__KDT__LOWER_CASE:
			for ( ucIndex = 0; ucIndex < UIKEYBRD__BUTTONS_MAX_LETTERS; ucIndex++ )
			{
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_KeyboardLowercaseLetters[ucIndex] );
			}
			break;

		case UIKEYBRD__KDT__UPPER_CASE:
			for ( ucIndex = 0; ucIndex < UIKEYBRD__BUTTONS_MAX_LETTERS; ucIndex++ )
			{
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_KeyboardUppercaseLetters[ucIndex] );
			}
			break;

		default:
			for ( ucIndex = 0; ucIndex < UIKEYBRD__BUTTONS_MAX_SYMBOLS; ucIndex++ )
			{
				UITFTDSP__ShowButton( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton_KeyboardSymbols[ucIndex] );
			}
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Alpha keyboard - Control update.
 * 			The tag value must be read at the application level as there may be other buttons on the display.  This tag value is
 * 			passed for processing here.
 * @param:	vucTagValue   - Tag value retrieved.
 * 			pbDonePressed - Pointer to flag indicating "Done" button pressed.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UIKEYBRD__UpdateControl(uint8_t vucTagValue, bool* pbDonePressed)
{
	static uint8_t	sucActiveTag = 0;

	switch ( vucTagValue )
	{
		case UIKEYBRD__TAG_SHF:
			// Button - Shift
			// - Toggle uppercase/lowercase display
			if ( mAlphaKeyboard.DisplayType == UIKEYBRD__KDT__LOWER_CASE )
			{
				mAlphaKeyboard.DisplayType = UIKEYBRD__KDT__UPPER_CASE;
				mAlphaKeyboard.bLastKeyPressed_SHF = true;
			}
			else
			{
				mAlphaKeyboard.DisplayType = UIKEYBRD__KDT__LOWER_CASE;
			}
			// - No rapid update
			RapidUpdateControl_None();
			break;

		case UIKEYBRD__TAG_BSP:
			// Button - Backspace
			// - Remove char from entry
			GENSTRDT__RemoveCharFromString( &mAlphaKeyboard.TextEntryString );
			// - No rapid update
			RapidUpdateControl_Backspace();
			break;

		case UIKEYBRD__TAG_SYM:
			// Button Pressed - Symbol
			// - Toggle symbols/letters
			if ( mAlphaKeyboard.DisplayType == UIKEYBRD__KDT__SYMBOLS )
			{
				mAlphaKeyboard.DisplayType = mAlphaKeyboard.LastDisplayType;
			}
			else
			{
				mAlphaKeyboard.DisplayType = UIKEYBRD__KDT__SYMBOLS;
			}
			// - No rapid update
			RapidUpdateControl_None();
			break;

		case UIKEYBRD__TAG_DNE:
			// Button Pressed - Done
			// - Save it to persist
			sucActiveTag = vucTagValue;
			// - No rapid update
			RapidUpdateControl_None();
			break;

		default:
			if ( vucTagValue != 0 )
			{
				// Must be one of the character buttons
				// - Add it to string
				if ( mAlphaKeyboard.TextEntryString.uiLength < ( mAlphaKeyboard.TextEntryString.uiMaxLength-1 ) )
				{
					GENSTRDT__AddCharToString( (char)vucTagValue, &mAlphaKeyboard.TextEntryString );
				}
				// - No rapid update
				RapidUpdateControl_None();
				// - Check if last key pressed to toggle back to lower/upper case keyboard
				if ( mAlphaKeyboard.bLastKeyPressed_SHF )
				{
					mAlphaKeyboard.DisplayType = UIKEYBRD__KDT__LOWER_CASE;
				}
				// - Clear last key pressed flag
				mAlphaKeyboard.bLastKeyPressed_SHF = false;
			}
			break;
	}

	if ( sucActiveTag != 0 )
	{
		vucTagValue = sucActiveTag;												// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )		// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( vucTagValue )
			{
				case UIKEYBRD__TAG_DNE:
					*pbDonePressed = true;
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
 * @brief:	Rapid update control - Backspace.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_Backspace(void)
{
	UI_TFTDISPLAY__FT800TouchTagValue	TouchTagValue;

	TouchTagValue.bWrapAroundEnabled = true;
	TouchTagValue.ucMinimum 		 = 0;
	TouchTagValue.ucMaximum 		 = 16;

	FT800_TOUCHIFC__RapidUpdateDataDefine( &TouchTagValue );

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
