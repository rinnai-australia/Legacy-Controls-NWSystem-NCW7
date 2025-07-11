//*********************************************************************************************************************************************************************************
// uitftdsp_number.c
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
#include        <uitftdsp_number.h>
#include        <uitftdsp_color.h>
#include		<FT800_coprccmd.h>

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
 * @brief:	Shows number at absolute X posn specified with Y posn centered - Always defaults to left justification.
 * @param:	pNumber - Pointer to number definition details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITFTDSP_NUMBER__ShowNumber(UI_TFTNUMBER__NumberDetails* pNumber)
{
	// Set the specified color
	UITFTDSP_COLOR__SetCurrentColor( pNumber->ulColor );
	// Show text
	switch ( pNumber->Justification )
	{
		case UITFTDSP_TEXT__TJ__LEFT:
			FT800_COPRCCMD__CPCmdQueue__CMD_NUMBER( pNumber->iX,
													pNumber->iY,
													pNumber->FontSize,
													FT800_COPRCCMD__OPTION_MASK__OPT_DEFAULT | FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY,
													*pNumber->pucNumber );
			break;

		case UITFTDSP_TEXT__TJ__RIGHT:
			FT800_COPRCCMD__CPCmdQueue__CMD_NUMBER( pNumber->iX,
													pNumber->iY,
													pNumber->FontSize,
													FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX | FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY,
													*pNumber->pucNumber );
			break;

		default:
			FT800_COPRCCMD__CPCmdQueue__CMD_NUMBER( pNumber->iX,
													pNumber->iY,
													pNumber->FontSize,
													FT800_COPRCCMD__OPTION_MASK__OPT_CENTER,
													*pNumber->pucNumber );
			break;
	}

	return;
}
//*********************************************************************************************************************************************************************************
