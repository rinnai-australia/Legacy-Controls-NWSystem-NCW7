//*********************************************************************************************************************************************************************************
// uitftdsp_text.c
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
#include        <uitftdsp_text.h>
#include        <uitftdsp_color.h>
#include        <uitftdsp_fonts.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
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
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
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
 * @brief:	Shows text at absolute posn specified - Always defaults to left justification.
 * @param:	pText - Pointer to text application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP_TEXT__ShowTextAbsolute(UITFTDSP_TEXT__TextDetails* pText)
{
	// Set the specified color
	UITFTDSP_COLOR__SetCurrentColor( pText->ulColor );
	// Show text
	switch (pText->Justification)
	{
		case UITFTDSP_TEXT__TJ__LEFT:
			FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( pText->iX,
												  pText->iY,
												  pText->FontSize,
												  FT800_COPRCCMD__OPTION_MASK__OPT_DEFAULT,
												  pText->pstrToDisplay );
			break;

		case UITFTDSP_TEXT__TJ__RIGHT:
			FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( pText->iX,
												  pText->iY,
												  pText->FontSize,
												  FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX,
												  pText->pstrToDisplay );
			break;

		default:
			FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( pText->iX,
												  pText->iY,
												  pText->FontSize,
												  FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX,
												  pText->pstrToDisplay );
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows text at absolute X posn specified with Y posn centered- Always defaults to left justification.
 * @param:	pText - Pointer to text application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP_TEXT__ShowTextAbsoluteWithCenteredY(UITFTDSP_TEXT__TextDetails* pText)
{
	// Set the specified color
	UITFTDSP_COLOR__SetCurrentColor( pText->ulColor );
	// Show text
	switch ( pText->Justification )
	{
		case UITFTDSP_TEXT__TJ__LEFT:
			FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( pText->iX,
												  pText->iY,
												  pText->FontSize,
												  FT800_COPRCCMD__OPTION_MASK__OPT_DEFAULT | FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY,
												  pText->pstrToDisplay );
			break;

		case UITFTDSP_TEXT__TJ__RIGHT:
			FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( pText->iX,
												  pText->iY,
												  pText->FontSize,
												  FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX | FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY,
												  pText->pstrToDisplay );
			break;

		default:
			FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( pText->iX,
												  pText->iY,
												  pText->FontSize,
												  FT800_COPRCCMD__OPTION_MASK__OPT_CENTER,
												  pText->pstrToDisplay );
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Shows text within a container where X/Y need to be specified.
 * @param:	viX, viY			    - X/Y coordinates where text to be displayed within container.
 * 			vbYIsAbsoluteCoordinate	- Use why coordinate as absolute coordinate when set.  If not use as centre.
 * 			pText* 					- Pointer to text application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP_TEXT__ShowTextWithinContainer(int16_t viX, int16_t viY, bool vbYIsAbsoluteCoordinate, UITFTDSP_TEXT__TextDetails* pText)
{
	// Set the specified color
	UITFTDSP_COLOR__SetCurrentColor( pText->ulColor );
	// Show text
	switch ( pText->Justification )
	{
		case UITFTDSP_TEXT__TJ__LEFT:
			if ( vbYIsAbsoluteCoordinate )
			{
				FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( viX,
													  viY,
													  pText->FontSize,
													  FT800_COPRCCMD__OPTION_MASK__OPT_DEFAULT,
													  pText->pstrToDisplay );
			}
			else
			{
				FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( viX,
													  viY,
													  pText->FontSize,
													  FT800_COPRCCMD__OPTION_MASK__OPT_DEFAULT | FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY,
													  pText->pstrToDisplay );
			}
			break;

		case UITFTDSP_TEXT__TJ__RIGHT:
			if ( vbYIsAbsoluteCoordinate )
			{
				FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( viX,
													  viY,
													  pText->FontSize,
													  FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX,
													  pText->pstrToDisplay );
			}
			else
			{
				FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( viX,
													  viY,
													  pText->FontSize,
													  FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX | FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY,
													  pText->pstrToDisplay );
			}
			break;

		default:
			if ( vbYIsAbsoluteCoordinate )
			{
				FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( viX,
													  viY,
													  pText->FontSize,
													  FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX,
													  pText->pstrToDisplay );
			}
			else
			{
				FT800_COPRCCMD__CPCmdQueue__CMD_TEXT( viX,
													  viY,
													  pText->FontSize,
													  FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX | FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY,
													  pText->pstrToDisplay );
			}
			break;
	}

	return;
}
//*********************************************************************************************************************************************************************************
