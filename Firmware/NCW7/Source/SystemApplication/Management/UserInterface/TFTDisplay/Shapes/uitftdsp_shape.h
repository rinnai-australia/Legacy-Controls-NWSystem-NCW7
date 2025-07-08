//*********************************************************************************************************************************************************************************
// uitftdsp_shape.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: UI TFT Display - Shape Details.
 *
 * 				Shape related functions.
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

#ifndef 		UITFTDSP_SHAPE_H_
#define 		UITFTDSP_SHAPE_H_

#include		<com_defn.h>
#include		<uitftdsp_text.h>
#include		<uitftdsp_image.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct 	UI_TFTSHAPE__Rectangle {					// Rectangle Details
															//
	uint32_t					ulColor;					// - Color
	int16_t						iX;							// - X posn
	int16_t						iY;							// - Y posn
	int16_t						iW;							// - Width
	int16_t						iH;							// - Height
	uint16_t					uiLineWidth;				// - Line width

} UI_TFTSHAPE__Rectangle;

typedef struct 	UI_TFTSHAPE__RectangleWithTextDetails {		// Rectangle With Text Details
															//
	UI_TFTSHAPE__Rectangle		Rectangle;					// - Container for text
	UITFTDSP_TEXT__TextDetails	Text;						// - Text to place in rectangle

} UI_TFTSHAPE__RectangleWithTextDetails;

typedef struct 	UI_TFTSHAPE__RectangleWithTwoTextsDetails {	// Rectangle With Two Text Details
															//
	UI_TFTSHAPE__Rectangle		Rectangle;					// - Container for texts
	UITFTDSP_TEXT__TextDetails	Text1;						// - First text to place in rectangle
	UITFTDSP_TEXT__TextDetails	Text2;						// - Second text to place in rectangle

} UI_TFTSHAPE__RectangleWithTwoTextsDetails;

typedef struct 	UI_TFTSHAPE__BarGraphDetails { 				// Bar Graph Definition Details
															//
	bool						bIsFanSpeedType; 			// - Flag indicates the type of bar graph: fan speed or comfort level
	UI_TFTSHAPE__Rectangle		OuterRectangle;				// - Outer rectangle

} UI_TFTSHAPE__BarGraphDetails;

typedef struct 	UI_TFTSHAPE__PointDetails {					// Point Details
															//
	uint32_t					ulColor;					// - Color
	int16_t						iX;							// - X posn
	int16_t						iY;							// - Y posn
	int16_t						iXOffset1;					// - X offset to show degree symbol for single-digit temperature display
	int16_t						iYOffset1;					// - Y offset to show degree symbol for single-digit temperature display
	int16_t						iXOffset2;					// - X offset to show degree symbol for two-digit temperature display
	int16_t						iYOffset2;					// - Y offset to show degree symbol for two-digit temperature display
	uint32_t					ulPointSize;				// - Point size

} UI_TFTSHAPE__PointDetails;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void 	UITFTDSP_SHAPE__ShowRectangle(UI_TFTSHAPE__Rectangle* pRectangle);
extern void		UITFTDSP_SHAPE__ShowRectangleWithText(bool vbShowText, UI_TFTSHAPE__RectangleWithTextDetails* pRectangle);
extern void		UITFTDSP_SHAPE__ShowRectangleWithTwoTexts(bool vbShowText, UI_TFTSHAPE__RectangleWithTwoTextsDetails* pRectangle);

extern void    	UITFTDSP_SHAPE__ShowBarGraph(uint8_t vucLevel, UI_TFTSHAPE__BarGraphDetails* pBarGraph);
extern void		UITFTDSP_SHAPE__ShowPoint(UI_TFTSHAPE__PointDetails* pPoint);
//*********************************************************************************************************************************************************************************

#endif /* UITFTDSP_SHAPE_H_ */
