//*********************************************************************************************************************************************************************************
// uitftdsp_shape.c
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
#include		<uitftdsp_shape.h>
#include        <uitftdsp_color.h>
#include		<uitftdsp_fonts.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	UITFTDSP_SHAPE__BAR_LEVEL {

	UITFTDSP_SHAPE__BL__BAR01	=  1,
	UITFTDSP_SHAPE__BL__BAR02	=  2,
	UITFTDSP_SHAPE__BL__BAR03	=  3,
	UITFTDSP_SHAPE__BL__BAR04	=  4,
	UITFTDSP_SHAPE__BL__BAR05	=  5,
	UITFTDSP_SHAPE__BL__BAR06	=  6,
	UITFTDSP_SHAPE__BL__BAR07	=  7,
	UITFTDSP_SHAPE__BL__BAR08	=  8,
	UITFTDSP_SHAPE__BL__BAR09	=  9,
	UITFTDSP_SHAPE__BL__BAR10	= 10,
	UITFTDSP_SHAPE__BL__BAR11	= 11,
	UITFTDSP_SHAPE__BL__BAR12	= 12,
	UITFTDSP_SHAPE__BL__BAR13	= 13,
	UITFTDSP_SHAPE__BL__BAR14	= 14,
	UITFTDSP_SHAPE__BL__BAR15	= 15,
	UITFTDSP_SHAPE__BL__BAR16	= 16

} UITFTDSP_SHAPE__BAR_LEVEL;

static const uint32_t 	mkaulComfortLevelColors[] =
{
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_01,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_02,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_03,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_04,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_05,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_06,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_07,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_08,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_09,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_10,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_11,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_12,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_13,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_14,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_15,
	UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_16
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void    					ShowIsoscelesTrapezium(bool vbShowInvertedTrapezium, UI_TFTSHAPE__Rectangle* pRectangle);
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
 * @brief:	Shows rectangle on screen.
 * @param:	pRectangle - Pointer to rectangle definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP_SHAPE__ShowRectangle(UI_TFTSHAPE__Rectangle* pRectangle)
{
	int16_t		iX1, iY1;
	int16_t		iX2, iY2;
	//
	// Define coordinates from W/H
	iX1 = pRectangle->iX; iX2 = iX1 + pRectangle->iW;
	iY1 = pRectangle->iY; iY2 = iY1 + pRectangle->iH;
	//
	// Begin rectangle
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BEGIN( FT800_DISPLIST__BEGIN_PRIM__RECTS ) );
	// Define color
	UITFTDSP_COLOR__SetCurrentColor( pRectangle->ulColor );
	// Line width
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__LINE_WIDTH( pRectangle->uiLineWidth * 16 ) );
	// Draw vertices
	if ( pRectangle->uiLineWidth == 1 ) // no corner rounding = no offset required
	{
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( iX1 * 16, iY1 * 16 ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( iX2 * 16, iY2 * 16 ) );
	}
	else
	{
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( ( iX1 + pRectangle->uiLineWidth ) * 16, ( iY1 + pRectangle->uiLineWidth ) * 16 ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( ( iX2 - pRectangle->uiLineWidth ) * 16, ( iY2 - pRectangle->uiLineWidth ) * 16 ) );
	}
	// End rectangle
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__END() );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Show rectangle with accompanying text on screen.
 * @param:	vbShowText - Flag indicates whether or not to show text.
 * 			pRectangle - Pointer to rectangle application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP_SHAPE__ShowRectangleWithText(bool vbShowText, UI_TFTSHAPE__RectangleWithTextDetails* pRectangle)
{
	int16_t		iX, iY;
	//
	// Show rectangle first
	UITFTDSP_SHAPE__ShowRectangle( &pRectangle->Rectangle );
	//
	// Now show the text if flagged to do so
	if ( !vbShowText ) { return; }
	//
	// X Text Position
	switch ( pRectangle->Text.Justification )
	{
		case UITFTDSP_TEXT__TJ__LEFT:
			if ( pRectangle->Text.iX < 0 )
			{
				// Offset from left edge
				iX = pRectangle->Rectangle.iX + pRectangle->Text.iXOffset;
			}
			else
			{
				// Absolute posn
				iX = pRectangle->Text.iX;
			}
			break;

		case UITFTDSP_TEXT__TJ__RIGHT:
			if ( pRectangle->Text.iX < 0 )
			{
				// Offset from right edge
				iX = pRectangle->Rectangle.iX + pRectangle->Rectangle.iW - pRectangle->Text.iXOffset;
			}
			else
			{
				// Absolute posn
				iX = pRectangle->Text.iX;
			}
			break;

		default:
			// Centre text in box
    	    iX = ( pRectangle->Rectangle.iX + ( pRectangle->Rectangle.iW / 2 ) );
			break;
	}
	//
	// Y Text Position
	if ( ( pRectangle->Text.iY < 0 ) && ( pRectangle->Text.iYOffset < 0 ) )
	{
		// No absolute posn or offset : Centre text in rectangle
		iY = ( pRectangle->Rectangle.iY + ( pRectangle->Rectangle.iH / 2 ) );
		// Show text within this container
		UITFTDSP_TEXT__ShowTextWithinContainer( iX, iY, false, &pRectangle->Text );
	}
	else
	if ( pRectangle->Text.iY < 0 )
	{
		// Absolute posn with offset
		iY = pRectangle->Rectangle.iY + pRectangle->Text.iYOffset;
		// Show text within this container
		UITFTDSP_TEXT__ShowTextWithinContainer( iX, iY, true, &pRectangle->Text );
	}
	else
	{
		// Absolute posn
		iY = pRectangle->Text.iY;
		// Show text within this container
		UITFTDSP_TEXT__ShowTextWithinContainer( iX, iY, true, &pRectangle->Text );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Show rectangle with accompanying two texts on screen.
 * @param:	vbShowText - Flag indicates whether or not to show text.
 * 			pRectangle - Pointer to rectangle with two texts application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP_SHAPE__ShowRectangleWithTwoTexts(bool vbShowText, UI_TFTSHAPE__RectangleWithTwoTextsDetails* pRectangle)
{
	int16_t		iX1, iX2, iY1, iY2;
	//
	// Show rectangle first
	UITFTDSP_SHAPE__ShowRectangle( &pRectangle->Rectangle );
	//
	// Now show the text if flagged to do so
	if ( !vbShowText ) { return; }
	//
	// Text1 details
	// - X1 Text Position
	switch ( pRectangle->Text1.Justification )
	{
		case UITFTDSP_TEXT__TJ__LEFT:
			if ( pRectangle->Text1.iX < 0 )
			{
				// Offset from left edge
				iX1 = pRectangle->Rectangle.iX + pRectangle->Text1.iXOffset;
			}
			else
			{
				// Absolute posn
				iX1 = pRectangle->Text1.iX;
			}
			break;

		case UITFTDSP_TEXT__TJ__RIGHT:
			if (pRectangle->Text1.iX < 0)
			{
				// Offset from right edge
				iX1 = pRectangle->Rectangle.iX + pRectangle->Rectangle.iW - pRectangle->Text1.iXOffset;
			}
			else
			{
				// Absolute posn
				iX1 = pRectangle->Text1.iX;
			}
			break;

		default:
			// Centre text in box
    	    iX1 = ( pRectangle->Rectangle.iX + ( pRectangle->Rectangle.iW / 2 ) );
			break;
	}
	//
	// - Y1 Text Position
	if ( ( pRectangle->Text1.iY < 0 ) && ( pRectangle->Text1.iYOffset < 0 ) )
	{
		// No absolute posn or offset : Centre text in rectangle
		iY1 = ( pRectangle->Rectangle.iY + ( pRectangle->Rectangle.iH / 2 ) );
		// Show text within this container
		UITFTDSP_TEXT__ShowTextWithinContainer( iX1, iY1, false, &pRectangle->Text1 );
	}
	else
	if ( pRectangle->Text1.iY < 0 )
	{
		// Absolute posn with offset
		iY1 = pRectangle->Rectangle.iY + pRectangle->Text1.iYOffset;
		// Show text within this container
		UITFTDSP_TEXT__ShowTextWithinContainer( iX1, iY1, true, &pRectangle->Text1 );
	}
	else
	{
		// Absolute posn
		iY1 = pRectangle->Text1.iY;
		// Show text within this container
		UITFTDSP_TEXT__ShowTextWithinContainer( iX1, iY1, true, &pRectangle->Text1 );
	}
	//
	// Text2 details - Only show if Text2 presents
	if ( pRectangle->Text2.pstrToDisplay != 0 )
	{
		// X2 Text Position
		switch ( pRectangle->Text2.Justification )
		{
			case UITFTDSP_TEXT__TJ__LEFT:
				if ( pRectangle->Text2.iX < 0 )
				{
					// Offset from left edge
					iX2 = pRectangle->Rectangle.iX + pRectangle->Text2.iXOffset;
				}
				else
				{
					// Absolute posn
					iX2 = pRectangle->Text2.iX;
				}
				break;

			case UITFTDSP_TEXT__TJ__RIGHT:
				if ( pRectangle->Text2.iX < 0 )
				{
					// Offset from right edge
					iX2 = pRectangle->Rectangle.iX + pRectangle->Rectangle.iW - pRectangle->Text2.iXOffset;
				}
				else
				{
					// Absolute posn
					iX2 = pRectangle->Text2.iX;
				}
				break;

			default:
				// Centre text in box
				iX2 = ( pRectangle->Rectangle.iX + ( pRectangle->Rectangle.iW / 2 ) );
				break;
		}
		//
		// Y2 Text Position
		if ( ( pRectangle->Text2.iY < 0 ) && ( pRectangle->Text2.iYOffset < 0 ) )
		{
			// No absolute posn or offset : Centre text in rectangle
			iY2 = ( pRectangle->Rectangle.iY + ( pRectangle->Rectangle.iH / 2 ) );
			// Show text within this container
			UITFTDSP_TEXT__ShowTextWithinContainer( iX2, iY2, false, &pRectangle->Text2 );
		}
		else
		if ( pRectangle->Text2.iY < 0 )
		{
			// Absolute posn with offset
			iY2 = pRectangle->Rectangle.iY + pRectangle->Text2.iYOffset;
			// Show text within this container
			UITFTDSP_TEXT__ShowTextWithinContainer( iX2, iY2, true, &pRectangle->Text2 );
		}
		else
		{
			// Absolute posn
			iY2 = pRectangle->Text2.iY;
			// Show text within this container
			UITFTDSP_TEXT__ShowTextWithinContainer( iX2, iY2, true, &pRectangle->Text2 );
		}
	}
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows all the bars in the bar graph up to the level specified - this can be either fan speed or comfort level.
 *
 *  		DIMENSIONS
 *  		Outer rectangle: 		   	78 x 129
 *  		Outer rectangle line width: 18
 *  		Inner rectangle: 			74 x 125
 *  		Inner rectangle line width: 16
 *  		Bar02 to Bar15: 			60 x 5
 *  		Bar01 and Bar16: 			5 pixels high trapezium
 *  		Gap between bars: 			2
 *
 * @param:	vucLevel  - Actual bar level.
 * 			pBarGraph - Pointer to bar graph definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP_SHAPE__ShowBarGraph(uint8_t vucLevel, UI_TFTSHAPE__BarGraphDetails* pBarGraph)
{
	UI_TFTSHAPE__Rectangle		InnerRectangle, Bar, Bar16, Bar01;
	UITFTDSP_SHAPE__BAR_LEVEL	BarLevel, Index;
	//
	// Copy data
	// - Inner rectangle
	InnerRectangle.ulColor		= UITFTDSP_COLOR__BACKGROUND;						// - Inner color is not app specific!
	InnerRectangle.iX			= pBarGraph->OuterRectangle.iX + 2;
	InnerRectangle.iY			= pBarGraph->OuterRectangle.iY + 2;
	InnerRectangle.iW			= pBarGraph->OuterRectangle.iW - 4;
	InnerRectangle.iH			= pBarGraph->OuterRectangle.iH - 4;
	InnerRectangle.uiLineWidth 	= pBarGraph->OuterRectangle.uiLineWidth - 2;
	// - Bottom bar: Bar01
	Bar01.iX					= pBarGraph->OuterRectangle.iX + 9;
	Bar01.iY					= pBarGraph->OuterRectangle.iY + 115;
	Bar01.iW					= 60;												// - Bar width, height and line width are not app specific!
	Bar01.iH					= 5;
	Bar01.uiLineWidth			= 1;
	// - Top bar: Bar16
	Bar16.iX					= Bar01.iX;
	Bar16.iY					= pBarGraph->OuterRectangle.iY + 10;
	Bar16.iW					= Bar01.iW;
	Bar16.iH					= Bar01.iH;
	Bar16.uiLineWidth			= Bar01.uiLineWidth;
	// - Bar02 to Bar15 common properties
	Bar.iX						= Bar01.iX;
	Bar.iW						= Bar01.iW;
	Bar.iH						= Bar01.iH - 1;
	Bar.uiLineWidth				= Bar01.uiLineWidth;
	//
	// Show rectangles
	// - Outer
	UITFTDSP_SHAPE__ShowRectangle( &pBarGraph->OuterRectangle );
	// - Inner
	UITFTDSP_SHAPE__ShowRectangle( &InnerRectangle );
	//
	// Get current bar level
	if ( vucLevel > UITFTDSP_SHAPE__BL__BAR16 )
	{
		BarLevel = UITFTDSP_SHAPE__BL__BAR16;
	}
	else
	{
		BarLevel = vucLevel;
	}
	//
	// Convert comfort level
	if ( !pBarGraph->bIsFanSpeedType )
	{
		BarLevel = 16 - BarLevel;
	}
	//
	// Init index
	Index = 1;
	// Show all the bars up to the level specified
	while ( Index <= BarLevel )
	{
		if ( Index <= UITFTDSP_SHAPE__BL__BAR01 )
		{
			// Bottom bar color
			if ( pBarGraph->bIsFanSpeedType )
			{
				// Fan speed bar
				Bar01.ulColor = UITFTDSP_COLOR__WHITE;
			}
			else
			{
				// Comfort level bar
				Bar01.ulColor = UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_01;
			}
			ShowIsoscelesTrapezium(true, &Bar01);
		}
		else
		if ( Index >= UITFTDSP_SHAPE__BL__BAR16 )
		{
			// Top bar color
			if ( pBarGraph->bIsFanSpeedType )
			{
				// Fan speed bar
				Bar16.ulColor = UITFTDSP_COLOR__WHITE;
			}
			else
			{
				// Comfort level bar
				Bar16.ulColor = UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_16;
			}
			ShowIsoscelesTrapezium( false, &Bar16 );
		}
		else
		{
			// Show bar02 to bar15
			// - Bar color
			if ( pBarGraph->bIsFanSpeedType )
			{
				// Fan speed bar
				Bar.ulColor = UITFTDSP_COLOR__WHITE;
			}
			else
			{
				// Comfort level bar
				Bar.ulColor = mkaulComfortLevelColors[Index - 1];
			}
			// - Y posn
			Bar.iY = Bar01.iY - (Index - 1) * 7;
			// - Show bar level rectangle
			UITFTDSP_SHAPE__ShowRectangle( &Bar );
		}

		Index++;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows the point specified.
 * @param:	pPoint - Pointer to point definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP_SHAPE__ShowPoint(UI_TFTSHAPE__PointDetails* pPoint)
{
	if ( pPoint->ulPointSize != 0 )
	{
		// Set the specified color
		UITFTDSP_COLOR__SetCurrentColor(pPoint->ulColor);

		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__POINT_SIZE( pPoint->ulPointSize * 16 ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BEGIN( FT800_DISPLIST__BEGIN_PRIM__POINTS ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( pPoint->iX * 16, pPoint->iY * 16 ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__END() );
	}

	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief: 	Shows isosceles trapezium specified.
 * @param:	vbShowInvertedTrapezium	- Flag indicates whether to show inverted trapezium.
 * 			pRectangle				- Pointer to rectangle definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void    					ShowIsoscelesTrapezium(bool vbShowInvertedTrapezium, UI_TFTSHAPE__Rectangle* pRectangle)
{
	int16_t		iX1, iY1, iX2;
	uint8_t		ucLineIndex, ucHeight;
	//
	// Copy data
	iX1 	 = pRectangle->iX;
	iX2 	 = iX1 + pRectangle->iW;
	iY1 	 = pRectangle->iY;
	ucHeight = pRectangle->iH;
	//
	// Draw lines
	// - Define Color
	UITFTDSP_COLOR__SetCurrentColor( pRectangle->ulColor );
	// - Define line width
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__LINE_WIDTH( pRectangle->uiLineWidth * 16 ) );
	// - Begin line command
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BEGIN( FT800_DISPLIST__BEGIN_PRIM__LINES ) );

	for ( ucLineIndex = 1; ucLineIndex <= ucHeight; ucLineIndex++ )
	{
		// Check if to draw inverted trapezium
		if ( vbShowInvertedTrapezium )
		{
			// Bottom bar - Decrease line width
			FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( ( iX1 + ucLineIndex - 1 ) * 16, ( iY1 + ucLineIndex - 1 ) * 16) );
			FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( ( iX2 - ucLineIndex + 1 ) * 16, ( iY1 + ucLineIndex - 1 ) * 16) );
		}
		else
		{
			// Top bar - Increasing line width
			FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( ( iX1 - ucLineIndex + ucHeight ) * 16, ( iY1 + ucLineIndex - 1 ) * 16) );
			FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( ( iX2 + ucLineIndex - ucHeight ) * 16, ( iY1 + ucLineIndex - 1 ) * 16) );
		}
	}
	// - End line command
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__END() );

	return;
}
//*********************************************************************************************************************************************************************************
