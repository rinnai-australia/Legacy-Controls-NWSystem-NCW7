//*********************************************************************************************************************************************************************************
// uitftdsp_button.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: UI TFT Display - Button Details.
 *
 * 				All buttons defined here that are generic to all modes of operation.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Release Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * Version:		N-C7 	   - v090
 * Date:		01/10/2020
 * Modifier:  	Emilio R. La Greca
 *
 * Original release.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef 		UITFTDSP_BUTTON_H_
#define 		UITFTDSP_BUTTON_H_

#include		<com_defn.h>
#include		<uitftdsp_image_evpcool.h>
#include		<uitftdsp_image_common.h>
#include		<uitftdsp_image.h>
#include		<uitftdsp_shape.h>
#include		<uitftdsp_fonts.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constants define how long to persist the pressing of a button with active color
// - Slow indication (250ms)
#define			UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW		( 25/GENTIMDT__kucUI_TIMEBASE_10MS )
// - Fast indication (50ms)
#define			UITFTDSP_BUTTON__PRESS_INDICATE_TIME_FAST		( 5/GENTIMDT__kucUI_TIMEBASE_10MS )

typedef enum	UITFTDSP_BUTTON__BUTTON_STATE {					// Normal state of the button
																//
    UITFTDSP_BUTTON__STATE_PASSIVE	=  0,  						// - Passive
    UITFTDSP_BUTTON__STATE_ACTIVE   =  1						// - Active

} UITFTDSP_BUTTON__BUTTON_STATE;

typedef enum	UITFTDSP_BUTTON__BUTTON_STYLE {					// Button Style
																//
    UITFTDSP_BUTTON__STYLE_ROUND    =  0,  						// - Round corners
    UITFTDSP_BUTTON__STYLE_SQUARE   =  1 	 					// - Square corners

} UITFTDSP_BUTTON__BUTTON_STYLE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct UITFTDSP_BUTTON__ButtonDefinition {								// Button Definition Details
																				//
    UITFTDSP_BUTTON__BUTTON_STATE				State;							// - Normal state of the button
    bool										bIndicateActive;				// - When pressed highlight as active flag
    uint8_t										ucPersistenceTime;				// - Time to persist indication
    UITFTDSP_BUTTON__BUTTON_STYLE      	   		Style;          				// - Round or square corners
    uint8_t                          	  		ucTagValue;     				// - Tag value
    																			//
    UI_TFTSHAPE__RectangleWithTextDetails		Container;						// - Container used to describe button

} UITFTDSP_BUTTON__ButtonDefinition;

typedef struct UITFTDSP_BUTTON__TemperatureButtonDefinition {					// Temperature Button Definition Details
																				//
    UITFTDSP_BUTTON__ButtonDefinition			Button;							// - Button definition details
    UI_TFTSHAPE__PointDetails					DegSymbol;						// - Point used as degree symbol

} UITFTDSP_BUTTON__TemperatureButtonDefinition;

typedef struct UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition {					// Button With Two Texts Definition Details
																				//
    UITFTDSP_BUTTON__BUTTON_STATE				State;							// - Normal state of the button
    bool										bIndicateActive;				// - When pressed highlight as active flag
    uint8_t										ucPersistenceTime;				// - Time to persist indication
    UITFTDSP_BUTTON__BUTTON_STYLE      	   		Style;          				// - Round or square corners
    uint8_t                          	  		ucTagValue;     				// - Tag value
    																			//
    UI_TFTSHAPE__RectangleWithTwoTextsDetails	Container;						// - Container used to describe button with two texts

} UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition;

typedef struct 	UITFTDSP_BUTTON__LevelButtonDetails {  							// Level Buttons Definition Details
																				//
	UI_TFTSHAPE__RectangleWithTwoTextsDetails	Container;						// - Container used to describe button
	UI_TFTIMAGE__ImageAppSpecificDetails		TopButton;        				// - Top button bitmap
	UI_TFTIMAGE__ImageAppSpecificDetails		BotButton;        				// - Bottom button bitmap
	UI_TFTSHAPE__PointDetails					DegSymbol;						// - Point used as degree symbol

} UITFTDSP_BUTTON__LevelButtonDetails;

typedef struct 	UITFTDSP_BUTTON__LevelButtonAppSpecificDetails {				// Level Button Application Specific Details
																				//
	bool										bIncreasingValue;				// - Flag indicating whether or not to increase value
	bool										bSetToZeroIfMinimumExceeded;	// - Flag indicating whether or not to set value to zero if minimum value exceeded
	uint8_t										ucMinValue;						// - Minimum value
	uint8_t										ucMaxValue;						// - Maximum value

} UITFTDSP_BUTTON__LevelButtonAppSpecificDetails;
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
extern const struct	GENTIMDT__Timing	UITFTDSP_BUTTON__kActiveButtonPersistenceTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void    					UITFTDSP_BUTTON__ShowButton(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonDefinition* pButton);
extern void 		   			UITFTDSP_BUTTON__ShowButtonWithColor(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonDefinition* pButton);
extern void    					UITFTDSP_BUTTON__ShowButtonWithTwoTexts(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition* pButtonWithTwoTexts);
extern void    					UITFTDSP_BUTTON__ShowLevelButtons(UITFTDSP_BUTTON__LevelButtonDetails* pLevelButtons);

extern uint8_t					UITFTDSP_BUTTON__ucActiveButtonTag(void);
//*********************************************************************************************************************************************************************************

#endif /* UITFTDSP_BUTTON_H_ */
