//*********************************************************************************************************************************************************************************
// uitftdsp_button.c
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
#include        <uitftdsp_button.h>
#include		<uitftdsp_image_evpcool.h>
#include		<uitftdsp_image_common.h>
#include        <uitftdsp_color.h>
#include		<uitftdsp_image.h>
#include        <uitftdsp_shape.h>
#include		<uitftdsp_fonts.h>
#include        <tftdspif.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	ActiveButtonDetails {							// Active Button Details
																//
	uint8_t						ucTag;							// - Tag that was last detected from pressed button
	GENTIMDT__TimingCounter		PersistenceTimingCounter;		// - Timer to maintain active indication that button pressed
																//
} ActiveButtonDetails;											//
																//
static ActiveButtonDetails		mActiveButton;					// Container
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	UITFTDSP_BUTTON__kActiveButtonPersistenceTiming = { &mActiveButton.PersistenceTimingCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void    					DrawButton(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonDefinition* pButton, bool vbShowUserDefinedColor);
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
 * @brief:	Shows a button on screen.
 * @param:	vbShowNormalButtonState	- Flag indicates normal button state: active or passive.
 * 			pButton 				- Pointer to button definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP_BUTTON__ShowButton(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonDefinition* pButton)
{
	DrawButton( vbShowNormalButtonState, pButton, false );
    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a button with user defined color on screen.
 * @param:	vbShowNormalButtonState - Flag indicates normal button state: active or passive.
 * 			pButton 				- Pointer to button definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void 				   			UITFTDSP_BUTTON__ShowButtonWithColor(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonDefinition* pButton)
{
	DrawButton( vbShowNormalButtonState, pButton, true );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows a button with two texts on screen.
 * @param:	vbShowNormalButtonState	- Flag indicates normal button state: active or passive.
 * 			pButtonWithTwoTexts		- Pointer to button with two texts definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP_BUTTON__ShowButtonWithTwoTexts(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition* pButtonWithTwoTexts)
{
	UI_TFTSHAPE__Rectangle						OuterRectangle;
	UI_TFTSHAPE__RectangleWithTwoTextsDetails	InnerContainer;
    //
    // Copy data
    // - Outer
    OuterRectangle.iX 	   		 	   = pButtonWithTwoTexts->Container.Rectangle.iX;
    OuterRectangle.iY 	   		 	   = pButtonWithTwoTexts->Container.Rectangle.iY;
    OuterRectangle.iW 	   		 	   = pButtonWithTwoTexts->Container.Rectangle.iW;
    OuterRectangle.iH 	   		 	   = pButtonWithTwoTexts->Container.Rectangle.iH;
    // - Inner
    InnerContainer.Rectangle.iX  	   = OuterRectangle.iX + 2;
    InnerContainer.Rectangle.iY  	   = OuterRectangle.iY + 2;
    InnerContainer.Rectangle.iW  	   = OuterRectangle.iW - 4;
    InnerContainer.Rectangle.iH  	   = OuterRectangle.iH - 4;
    InnerContainer.Text1.iX		 	   = pButtonWithTwoTexts->Container.Text1.iX;
    InnerContainer.Text1.iY		 	   = pButtonWithTwoTexts->Container.Text1.iY;
    InnerContainer.Text1.iXOffset 	   = pButtonWithTwoTexts->Container.Text1.iXOffset;
    InnerContainer.Text1.iYOffset 	   = pButtonWithTwoTexts->Container.Text1.iYOffset;
    InnerContainer.Text1.FontSize 	   = pButtonWithTwoTexts->Container.Text1.FontSize;
    InnerContainer.Text1.Justification = pButtonWithTwoTexts->Container.Text1.Justification;
    InnerContainer.Text1.pstrToDisplay = pButtonWithTwoTexts->Container.Text1.pstrToDisplay;
    InnerContainer.Text2.iX		 	   = pButtonWithTwoTexts->Container.Text2.iX;
    InnerContainer.Text2.iY		 	   = pButtonWithTwoTexts->Container.Text2.iY;
    InnerContainer.Text2.iXOffset 	   = pButtonWithTwoTexts->Container.Text2.iXOffset;
    InnerContainer.Text2.iYOffset 	   = pButtonWithTwoTexts->Container.Text2.iYOffset;
    InnerContainer.Text2.FontSize 	   = pButtonWithTwoTexts->Container.Text2.FontSize;
    InnerContainer.Text2.Justification = pButtonWithTwoTexts->Container.Text2.Justification;
    InnerContainer.Text2.pstrToDisplay = pButtonWithTwoTexts->Container.Text2.pstrToDisplay;
    //
    // Overrides
    // - Line widths
    if ( pButtonWithTwoTexts->Style == UITFTDSP_BUTTON__STYLE_ROUND )
    {
        OuterRectangle.uiLineWidth 			 = 8;
        InnerContainer.Rectangle.uiLineWidth = 6;
    }
    else
    {
        OuterRectangle.uiLineWidth 			 = 1;
        InnerContainer.Rectangle.uiLineWidth = 1;
    }
    // - Outer color
    OuterRectangle.ulColor = UITFTDSP_COLOR__BUTTON_OUTER;							// - Color is not app specific!
    // - Inner color
    if  ( pButtonWithTwoTexts->State != UITFTDSP_BUTTON__STATE_PASSIVE )
    {
    	if ( vbShowNormalButtonState )
    	{
        	// Active color scheme - Select by user
        	InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
        	InnerContainer.Text1.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
        	InnerContainer.Text2.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
    	}
    	else
    	{
       		InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_PASSIVE;
       		InnerContainer.Text1.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE;
       		InnerContainer.Text2.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE;
    	}
    }
    else
   	if ( ( pButtonWithTwoTexts->bIndicateActive ) && ( pButtonWithTwoTexts->ucTagValue == TFTDSPIF__ucTouchTagValue() ) )
   	{
   		// Active color scheme - Button being pressed
   		InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
   		InnerContainer.Text1.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
   		InnerContainer.Text2.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
   		// Remember this button to highlight press
   		mActiveButton.ucTag = pButtonWithTwoTexts->ucTagValue;

   		GENTIMDT__StartTimer_TB_UIBased( pButtonWithTwoTexts->ucPersistenceTime, &mActiveButton.PersistenceTimingCounter );
   	}
   	else
   	if ( ( mActiveButton.ucTag == pButtonWithTwoTexts->ucTagValue ) && ( mActiveButton.PersistenceTimingCounter.ulValue != 0 ) )
   	{
   		// Active color scheme - Button was being pressed
   		InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
   		InnerContainer.Text1.ulColor     = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
   		InnerContainer.Text2.ulColor     = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
   	}
   	else
   	{
    	if ( vbShowNormalButtonState )
    	{
    		// Passive color scheme - Button not being pressed
    		InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_PASSIVE;
    		InnerContainer.Text1.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE;
    		InnerContainer.Text2.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE;
    	}
    	else
    	{
        	InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
        	InnerContainer.Text1.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
        	InnerContainer.Text2.ulColor	 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
    	}
   		// Clear tag being remembered
       	if ( mActiveButton.ucTag == pButtonWithTwoTexts->ucTagValue ) { mActiveButton.ucTag = 0; }
   	}
    //
    // Draw Rectangles
    // - Tag value
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK( TFTDSPIF__ucTouchTagMask() ) ); // set mask to allow use of TagValue
    FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG(pButtonWithTwoTexts->ucTagValue) );
    // - Outer rectangle
    UITFTDSP_SHAPE__ShowRectangle( &OuterRectangle );
    // - Inner rectangle
    UITFTDSP_SHAPE__ShowRectangleWithTwoTexts( true, &InnerContainer );
    // - Tag mask clear
    FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK( 0 ) );

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows combined level buttons.
 * @param:	pLevelButtons - Pointer to level buttons definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void    						UITFTDSP_BUTTON__ShowLevelButtons(UITFTDSP_BUTTON__LevelButtonDetails* pLevelButtons)
{
	UI_TFTSHAPE__Rectangle						OuterRectangle;
	UI_TFTSHAPE__RectangleWithTwoTextsDetails	InnerContainer;
	UI_TFTIMAGE__ImageAppSpecificDetails		TopButton, BottomButton;
    //
    // Copy data
    // - Outer rectangle
    OuterRectangle.ulColor			   	 = pLevelButtons->Container.Rectangle.ulColor;
    OuterRectangle.iX 	   		 	   	 = pLevelButtons->Container.Rectangle.iX;
    OuterRectangle.iY 	   		 	   	 = pLevelButtons->Container.Rectangle.iY;
    OuterRectangle.iW 	   		 	   	 = pLevelButtons->Container.Rectangle.iW;
    OuterRectangle.iH 	   		 	   	 = pLevelButtons->Container.Rectangle.iH;
    OuterRectangle.uiLineWidth  	   	 = pLevelButtons->Container.Rectangle.uiLineWidth;
    // - Inner container
    InnerContainer.Rectangle.ulColor   	 = UITFTDSP_COLOR__BACKGROUND;						// - Inner color is not app specific!
    InnerContainer.Rectangle.iX  	     = OuterRectangle.iX + 2;
    InnerContainer.Rectangle.iY  	     = OuterRectangle.iY + 2;
    InnerContainer.Rectangle.iW  	     = OuterRectangle.iW - 4;
    InnerContainer.Rectangle.iH  	     = OuterRectangle.iH - 4;
    InnerContainer.Rectangle.uiLineWidth = OuterRectangle.uiLineWidth - 2;
    InnerContainer.Text1.iX		 	     = pLevelButtons->Container.Text1.iX;
    InnerContainer.Text1.iY		 	     = pLevelButtons->Container.Text1.iY;
    InnerContainer.Text1.iXOffset 	     = pLevelButtons->Container.Text1.iXOffset;
    InnerContainer.Text1.iYOffset 	     = pLevelButtons->Container.Text1.iYOffset;
    InnerContainer.Text1.ulColor	     = pLevelButtons->Container.Text1.ulColor;
    InnerContainer.Text1.FontSize 	     = pLevelButtons->Container.Text1.FontSize;
    InnerContainer.Text1.Justification   = pLevelButtons->Container.Text1.Justification;
    InnerContainer.Text1.pstrToDisplay   = pLevelButtons->Container.Text1.pstrToDisplay;
    // - Top button
    TopButton.Number					 = pLevelButtons->TopButton.Number;
    TopButton.Number_ActivePress		 = pLevelButtons->TopButton.Number_ActivePress;
    TopButton.iXPos				 	 	 = pLevelButtons->Container.Rectangle.iX + 6;

    if ( pLevelButtons->TopButton.iYPos < 0 )
    {
    	// Offset from left edge
		TopButton.iYPos				 	 = pLevelButtons->Container.Rectangle.iY + 6;;
    }
    else
    {
    	// Absolute posn
        TopButton.iYPos				 	 = pLevelButtons->TopButton.iYPos;
    }
    TopButton.ucPersistenceTime			 = pLevelButtons->TopButton.ucPersistenceTime;
    TopButton.iTagValue					 = pLevelButtons->TopButton.iTagValue;
    // - Bottom button
    BottomButton.Number					 = pLevelButtons->BotButton.Number;
    BottomButton.Number_ActivePress		 = pLevelButtons->BotButton.Number_ActivePress;
    BottomButton.iXPos					 = TopButton.iXPos;

    if ( pLevelButtons->BotButton.iYPos < 0 )
    {
    	// Offset from top button
    	BottomButton.iYPos				 = TopButton.iYPos + 73;
    }
    else
    {
    	// Absolute posn
    	BottomButton.iYPos				 = pLevelButtons->BotButton.iYPos;
    }
    BottomButton.ucPersistenceTime		 = pLevelButtons->BotButton.ucPersistenceTime;
    BottomButton.iTagValue				 = pLevelButtons->BotButton.iTagValue;
    //
    // Draw rectangles
    // - Outer rectangle
    UITFTDSP_SHAPE__ShowRectangle( &OuterRectangle );
    // - Text2 details
    if ( pLevelButtons->Container.Text2.pstrToDisplay != 0 )
    {
		InnerContainer.Text2.iX		 	   = pLevelButtons->Container.Text2.iX;
		InnerContainer.Text2.iY		 	   = pLevelButtons->Container.Text2.iY;
		InnerContainer.Text2.iXOffset 	   = pLevelButtons->Container.Text2.iXOffset;
		InnerContainer.Text2.iYOffset 	   = pLevelButtons->Container.Text2.iYOffset;
		InnerContainer.Text2.ulColor	   = InnerContainer.Text1.ulColor;
		InnerContainer.Text2.FontSize 	   = pLevelButtons->Container.Text2.FontSize;
		InnerContainer.Text2.Justification = pLevelButtons->Container.Text2.Justification;
		InnerContainer.Text2.pstrToDisplay = pLevelButtons->Container.Text2.pstrToDisplay;
    }
    else
    {
    	InnerContainer.Text2.pstrToDisplay = 0;
    }
	// - Inner rectangle
	UITFTDSP_SHAPE__ShowRectangleWithTwoTexts( true, &InnerContainer );
    //
    // Images : Associated with button presses
    // - Top button
	UITFTDSP_IMAGE__ShowImageWithAssociatedTag( &TopButton );
	// - Bottom button
	UITFTDSP_IMAGE__ShowImageWithAssociatedTag( &BottomButton );

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the current tag that is causing a button to be displayed as active.
 * @param:	None.
 * @retval:	uint8_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							UITFTDSP_BUTTON__ucActiveButtonTag(void)
{
	return mActiveButton.ucTag;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Draws a button on screen.
 * @param:	vbShowNormalButtonState	- Flag indicates normal button state: active or passive.
 * 			pButton 				- Pointer to button definition details.
 * 			vbShowUserDefinedColor	- Flag indicates whether to show defined color.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void    					DrawButton(bool vbShowNormalButtonState, UITFTDSP_BUTTON__ButtonDefinition* pButton, bool vbShowUserDefinedColor)
{
	UI_TFTSHAPE__Rectangle					OuterRectangle;
	UI_TFTSHAPE__RectangleWithTextDetails	InnerContainer;
    //
    // Copy data
    // - Outer
    OuterRectangle.iX 	   		 	  = pButton->Container.Rectangle.iX;
    OuterRectangle.iY 	   		 	  = pButton->Container.Rectangle.iY;
    OuterRectangle.iW 	   		 	  = pButton->Container.Rectangle.iW;
    OuterRectangle.iH 	   		 	  = pButton->Container.Rectangle.iH;
    // - Inner
    InnerContainer.Rectangle.iX  	  = OuterRectangle.iX + 2;
    InnerContainer.Rectangle.iY  	  = OuterRectangle.iY + 2;
    InnerContainer.Rectangle.iW  	  = OuterRectangle.iW - 4;
    InnerContainer.Rectangle.iH  	  = OuterRectangle.iH - 4;
    InnerContainer.Text.iX		 	  = pButton->Container.Text.iX;
    InnerContainer.Text.iY		 	  = pButton->Container.Text.iY;
    InnerContainer.Text.iXOffset 	  = pButton->Container.Text.iXOffset;
    InnerContainer.Text.iYOffset 	  = pButton->Container.Text.iYOffset;
    InnerContainer.Text.FontSize 	  = pButton->Container.Text.FontSize;
    InnerContainer.Text.Justification = pButton->Container.Text.Justification;
    InnerContainer.Text.pstrToDisplay = pButton->Container.Text.pstrToDisplay;
    //
    // Overrides
    // - Line widths
    if ( pButton->Style == UITFTDSP_BUTTON__STYLE_ROUND )
    {
        OuterRectangle.uiLineWidth 			 = 8;
        InnerContainer.Rectangle.uiLineWidth = 6;
    }
    else
    {
        OuterRectangle.uiLineWidth 			 = 1;
        InnerContainer.Rectangle.uiLineWidth = 1;
    }
    // - Outer color
    OuterRectangle.ulColor 		 	  = UITFTDSP_COLOR__BUTTON_OUTER;							// - Color is not app specific!
    // - Inner color
    if  ( pButton->State != UITFTDSP_BUTTON__STATE_PASSIVE )
    {
    	if ( vbShowNormalButtonState )
    	{
    		// Active color scheme
    		if ( vbShowUserDefinedColor )
    		{
            	// - Select by user
            	InnerContainer.Rectangle.ulColor = pButton->Container.Rectangle.ulColor;;
            	InnerContainer.Text.ulColor		 = pButton->Container.Text.ulColor;;
    		}
    		else
    		{
            	// - Use default colors
            	InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
            	InnerContainer.Text.ulColor		 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
    		}
    	}
    	else
    	{
    		// Passive color scheme
    		if ( vbShowUserDefinedColor )
    		{
    			// - Select by user
            	InnerContainer.Rectangle.ulColor = pButton->Container.Rectangle.ulColor;;
            	InnerContainer.Text.ulColor		 = pButton->Container.Text.ulColor;;
    		}
    		else
    		{
    			// - Use default colors
           		InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_PASSIVE;
           		InnerContainer.Text.ulColor		 = UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE;
    		}
    	}
    }
    else
   	if ( ( pButton->bIndicateActive ) && ( pButton->ucTagValue == TFTDSPIF__ucTouchTagValue() ) )
   	{
   		// Active color scheme - Button being pressed
		if ( vbShowUserDefinedColor )
		{
        	// - Select by user
        	InnerContainer.Rectangle.ulColor = pButton->Container.Rectangle.ulColor;;
        	InnerContainer.Text.ulColor		 = pButton->Container.Text.ulColor;;
		}
		else
		{
        	// - Use default colors
        	InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
        	InnerContainer.Text.ulColor		 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
		}
   		// Remember this button to highlight press
   		mActiveButton.ucTag = pButton->ucTagValue; GENTIMDT__StartTimer_TB_UIBased( pButton->ucPersistenceTime, &mActiveButton.PersistenceTimingCounter );
   	}
   	else
   	if ( ( mActiveButton.ucTag == pButton->ucTagValue ) && ( mActiveButton.PersistenceTimingCounter.ulValue != 0 ) )
   	{
   		// Active color scheme - Button was being pressed
		if ( vbShowUserDefinedColor )
		{
        	// - Select by user
        	InnerContainer.Rectangle.ulColor = pButton->Container.Rectangle.ulColor;;
        	InnerContainer.Text.ulColor		 = pButton->Container.Text.ulColor;;
		}
		else
		{
        	// - Use default colors
        	InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
        	InnerContainer.Text.ulColor		 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
		}
   	}
   	else
   	{
    	if ( vbShowNormalButtonState )
    	{
    		// Passive color scheme - Button not being pressed
    		if ( vbShowUserDefinedColor )
    		{
            	// - Select by user
            	InnerContainer.Rectangle.ulColor = pButton->Container.Rectangle.ulColor;;
            	InnerContainer.Text.ulColor		 = pButton->Container.Text.ulColor;;
    		}
    		else
    		{
            	// - Use default colors
            	InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_PASSIVE;
            	InnerContainer.Text.ulColor		 = UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE;
    		}
    	}
    	else
    	{
    		// Active color scheme
    		if ( vbShowUserDefinedColor )
    		{
            	// - Select by user
            	InnerContainer.Rectangle.ulColor = pButton->Container.Rectangle.ulColor;;
            	InnerContainer.Text.ulColor		 = pButton->Container.Text.ulColor;;
    		}
    		else
    		{
            	// - Use default colors
            	InnerContainer.Rectangle.ulColor = UITFTDSP_COLOR__BUTTON_INNER_ACTIVE;
            	InnerContainer.Text.ulColor		 = UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE;
    		}
    	}
   		// Clear tag being remembered
       	if ( mActiveButton.ucTag == pButton->ucTagValue ) { mActiveButton.ucTag = 0; }
   	}
    //
    // Draw Rectangles
    // - Tag value
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK( TFTDSPIF__ucTouchTagMask() ) ); 	// Set mask to allow use of TagValue
    FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG( pButton->ucTagValue ) );
    // - Outer rectangle
    UITFTDSP_SHAPE__ShowRectangle( &OuterRectangle );
    // - Inner rectangle
    UITFTDSP_SHAPE__ShowRectangleWithText( true, &InnerContainer );
    // - Tag mask clear
    FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK( 0 ) );

    return;
}
//*********************************************************************************************************************************************************************************
