//*********************************************************************************************************************************************************************************
// uitftdsp_image_heating.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: UI TFT Display - Images (Heating).
 *
 * 				All images defined here that are specific to the heating mode of operation.
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

#ifndef	 		UITFTDSP_IMAGE_HEATING_H_
#define 		UITFTDSP_IMAGE_HEATING_H_

#include		<com_defn.h>
#include		<uitftdsp_image_common.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	UITFTDSP_IMAGE_HEATING__IMAGE_NUMBER {

	UITFTDSP_IMAGE_HEATING__IN__FLAME_ICON     		=  0,
	UITFTDSP_IMAGE_HEATING__IN__LIGHTNING_BOLT_ICON =  1,		// Extra heating image, NC7090_DFDISP

} UITFTDSP_IMAGE_HEATING__IMAGE_NUMBER;
/*
#define			UITFTDSP_IMAGE_HEATING__MAX_IMAGES		1
*/ // Extra heating image, NC7090_DFDISP
#define			UITFTDSP_IMAGE_HEATING__MAX_IMAGES		2
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	STRUCTURES
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
extern void									UITFTDSP_IMAGE_HEATING__Initialise(void);
extern UITFTDSP_IMAGE_COMMON__ImageDetails*	UITFTDSP_IMAGE_HEATING__pImage(UITFTDSP_IMAGE_HEATING__IMAGE_NUMBER vImageNumber);
//*********************************************************************************************************************************************************************************

#endif /* UITFTDSP_IMAGE_HEATING_H_ */
