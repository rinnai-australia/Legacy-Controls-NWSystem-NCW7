//*********************************************************************************************************************************************************************************
// uitftdsp_image_common.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: UI TFT Display - Images (Common).
 *
 * 				All images defined here that are common to the entire application.
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

#ifndef 		UITFTDSP_IMAGE_COMMON_H_
#define 		UITFTDSP_IMAGE_COMMON_H_

#include		<com_defn.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	UITFTDSP_IMAGE_COMMON__IMAGE_NUMBER {

	UITFTDSP_IMAGE_COMMON__IN__MENU_BTN_PASSIVE     			=  0,
	UITFTDSP_IMAGE_COMMON__IN__MENU_BTN_ACTIVE      			=  1,
	UITFTDSP_IMAGE_COMMON__IN__HELP_BTN_PASSIVE     			=  2,
	UITFTDSP_IMAGE_COMMON__IN__HELP_BTN_ACTIVE      			=  3,
	UITFTDSP_IMAGE_COMMON__IN__ONOFF_BTN_PASSIVE    			=  4,
	UITFTDSP_IMAGE_COMMON__IN__ONOFF_BTN_ACTIVE    				=  5,
	UITFTDSP_IMAGE_COMMON__IN__MODE_BTN_PASSIVE     			=  6,
	UITFTDSP_IMAGE_COMMON__IN__MODE_BTN_ACTIVE      			=  7,
	UITFTDSP_IMAGE_COMMON__IN__ZONE_BTN_PASSIVE    	 			=  8,
	UITFTDSP_IMAGE_COMMON__IN__ZONE_BTN_ACTIVE      			=  9,
	UITFTDSP_IMAGE_COMMON__IN__BRIVIS_LOGO          			= 10,
	UITFTDSP_IMAGE_COMMON__IN__CLOCK_ICON           			= 11,
	UITFTDSP_IMAGE_COMMON__IN__HEART_ICON           			= 12,
	UITFTDSP_IMAGE_COMMON__IN__M_ICON               			= 13,
	UITFTDSP_IMAGE_COMMON__IN__LOCK_ICON            			= 14,
	UITFTDSP_IMAGE_COMMON__IN__SPANNER_ICON         			= 15,
	UITFTDSP_IMAGE_COMMON__IN__FAN_ICON             			= 16,
	UITFTDSP_IMAGE_COMMON__IN__LEVEL_UP_PASSIVE     			= 17,
	UITFTDSP_IMAGE_COMMON__IN__LEVEL_UP_ACTIVE      			= 18,
	UITFTDSP_IMAGE_COMMON__IN__LEVEL_DN_PASSIVE     			= 19,
	UITFTDSP_IMAGE_COMMON__IN__LEVEL_DN_ACTIVE      			= 20,
	UITFTDSP_IMAGE_COMMON__IN__FONT_METRIC          			= 21,
	UITFTDSP_IMAGE_COMMON__IN__FONT_BMP             			= 22,
	UITFTDSP_IMAGE_COMMON__IN__BACK_BTN_PASSIVE	 				= 23,
	UITFTDSP_IMAGE_COMMON__IN__BACK_BTN_ACTIVE	 	 			= 24,
	UITFTDSP_IMAGE_COMMON__IN__FAN_MENU_BTN_PASSIVE  			= 25,
	UITFTDSP_IMAGE_COMMON__IN__FAN_MENU_BTN_ACTIVE   			= 26,
	UITFTDSP_IMAGE_COMMON__IN__SCHEDULE_LEVEL_UP_BTN_PASSIVE  	= 27,
	UITFTDSP_IMAGE_COMMON__IN__SCHEDULE_LEVEL_UP_BTN_ACTIVE  	= 28,
	UITFTDSP_IMAGE_COMMON__IN__ALERT_ICON					  	= 29,

	UITFTDSP_IMAGE_COMMON__IN__DF_GAS_BTN_PASSIVE				= 30,		// Duel fuel GAS button, NC7090_DFDISP
	UITFTDSP_IMAGE_COMMON__IN__DF_GAS_BTN_ACTIVE			  	= 31,		//
	UITFTDSP_IMAGE_COMMON__IN__DF_ELEC_BTN_PASSIVE				= 32,		// Dual fuel ELEC button, NC7090_DFDISP
	UITFTDSP_IMAGE_COMMON__IN__DF_ELEC_BTN_ACTIVE				= 33,		//

} UITFTDSP_IMAGE_COMMON__IMAGE_NUMBER; // Do not forget to update image number in uitftdsp_image.h!!
/*
#define			UITFTDSP_IMAGE_COMMON__MAX_IMAGES		30
*/ // Extra 4 images, NC7090_DFDISP
#define			UITFTDSP_IMAGE_COMMON__MAX_IMAGES		34
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	UITFTDSP_IMAGE_COMMON__ImageDefinition {				// Image Definition Details
																		//
	const uint8_t*									pucTable;			// - Pointer to the array holding the converted image
	uint32_t										ulDataSize;			// - Data size
	uint16_t										uiLineStride;		// - Line stride
	uint16_t										uiWidth;			// - Width
	uint16_t										uiHeight;			// - Height

} UITFTDSP_IMAGE_COMMON__ImageDefinition;

typedef struct UITFTDSP_IMAGE_COMMON__ImageDetails {					// Image Details
																		//
	uint32_t										ulRAMStartAddx;		// - Start addx in FT800 graphics RAM
	const UITFTDSP_IMAGE_COMMON__ImageDefinition*	pDefinition;		// - Image definition details

} UITFTDSP_IMAGE_COMMON__ImageDetails;
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
extern void									UITFTDSP_IMAGE_COMMON__Initialise(void);
extern UITFTDSP_IMAGE_COMMON__ImageDetails*	UITFTDSP_IMAGE_COMMON__pImage(UITFTDSP_IMAGE_COMMON__IMAGE_NUMBER vImageNumber);

extern void									UITFTDSP_IMAGE_COMMON__SetNextStartAddx(uint32_t vulStartAddx);
extern void									UITFTDSP_IMAGE_COMMON__GetNextStartAddx(uint32_t* pulStartAddx);
//*********************************************************************************************************************************************************************************

#endif /* UITFTDSP_IMAGE_COMMON_H_ */
