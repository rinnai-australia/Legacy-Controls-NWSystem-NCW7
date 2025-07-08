//*********************************************************************************************************************************************************************************
// uitftdsp_image.c
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
#include		<uitftdsp_image.h>
#include		<uitftdsp_image_evpcool.h>
#include		<uitftdsp_image_common.h>
#include		<uitftdsp_color.h>
#include		<tftdspif.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<gentimdt.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	UI_TFTIMAGE__GRAM_UPDATE_STATE {			// Graphic RAM update state
															//
	UI_TFTIMAGE__GUS__INIT = 0,								// - Initialise start addx and image number
	UI_TFTIMAGE__GUS__DEFN = 1,								// - Define image pointer
	UI_TFTIMAGE__GUS__WRTE = 2								// - Write the image to RAM

} UI_TFTIMAGE__GRAM_UPDATE_STATE;

typedef enum	UI_TFTIMAGE__GRAM_UPDATE_TYPE {

	UI_TFTIMAGE__GUT__COMMON  = 0,
	UI_TFTIMAGE__GUT__HEATING = 1,
	UI_TFTIMAGE__GUT__EVPCOOL = 2,
	UI_TFTIMAGE__GUT__COOLING = 3

} UI_TFTIMAGE__GRAM_UPDATE_TYPE;

typedef enum	UI_TFTIMAGE__GRAM_SINGLE_IMAGE_UPDATE {

	UI_TFTIMAGE__GSIU__NONE     = 0,
	UI_TFTIMAGE__GSIU__COMMON   = 1,
	UI_TFTIMAGE__GSIU__SPECIFIC = 2

} UI_TFTIMAGE__GRAM_SINGLE_IMAGE_UPDATE;
//
// Fan rotation angle in 10ms x GENTIMDT__kucUI_TIMEBASE_10MS interval - index is the actual fan speed.
static const uint8_t    mkaucFanBladeRotationAngle[TWBIFUTL__MAX_FANSPEED_LEVEL+1] = {

	0,
	4, 	4, 	5, 	5, 	6,  6,  7, 	7,
	8, 	8, 	9, 	9, 10, 10, 11, 11
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	GRAMUpdateDetails {

	UI_TFTIMAGE__GRAM_UPDATE_TYPE			Type;
	uint16_t								uiImageOffset;
	uint16_t								uiMaxImages;

} GRAMUpdateDetails;

typedef struct	FanBladeDetails {														// Fan blade animation control
																						//
    uint8_t         						ucFanRotationNumber;						// - Holds the current rotation number which is used for calculating rotation angle
    GENTIMDT__TimingCounter					DelayBeforeNextIconDisplayTimingCounter;    // - The counter used to time the intervals between updates
    																					//
} FanBladeDetails;																		//
																						//
static FanBladeDetails		mFanBladeAnimation;		     	  							// Container

typedef struct	ActiveImageDetails {													// Active Button Details
																						//
	uint8_t									ucTag;										// - Tag that was last detected from pressed button
	UITFTDSP_IMAGE_COMMON__ImageDetails*	pActiveImage;								// - Image that is being pressed
	GENTIMDT__TimingCounter					PersistenceTimingCounter;					// - Timer to maintin active indication that button pressed
																						//
} ActiveImageDetails;																	//
																						//
static ActiveImageDetails	mActiveImage;												// Container

static UI_TFTIMAGE__GRAM_SINGLE_IMAGE_UPDATE	mSingleImageUpdate = UI_TFTIMAGE__GSIU__NONE;
static UI_TFTIMAGE__GRAM_UPDATE_STATE			mGRAMUpdateState   = UI_TFTIMAGE__GUS__INIT;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	UITFTDSP_IMAGE__kFanBladeDelayIconDisplayTiming  = { &mFanBladeAnimation.DelayBeforeNextIconDisplayTimingCounter };
const struct	GENTIMDT__Timing	UITFTDSP_IMAGE__kActiveImagePersistenceTiming 	 = { &mActiveImage.PersistenceTimingCounter						 };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void									DrawImage(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific, UITFTDSP_IMAGE_COMMON__ImageDetails* pImage);
static UITFTDSP_IMAGE_COMMON__ImageDetails*	pSpecificImage(UITFTDSP_IMAGE__IMAGE_NUMBER vImageNumber);
FT800_DEVICEIF__XFER_STATUS					UpdateFT800GRAM_Images(FUNC_ACTION vAction, GRAMUpdateDetails* pGRAMUpdate);
static FT800_DEVICEIF__XFER_STATUS			WrImageToFT800GRAM(FUNC_ACTION vAction, uint32_t vulAddx, UITFTDSP_IMAGE_COMMON__ImageDetails* pImage);
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
 * @brief:	Image modules initialisation.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								UITFTDSP_IMAGE__Initialise(void)
{
	// Image init - Common
	UITFTDSP_IMAGE_COMMON__Initialise();
	// Image init - EvpCool
	UITFTDSP_IMAGE_EVPCOOL__Initialise();
	// Image init - Heating
	UITFTDSP_IMAGE_HEATING__Initialise();
	// Image init - Cooling
	UITFTDSP_IMAGE_COOLING__Initialise();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the image specified never assigning any tag.
 * @param:	pImageAppSpecific - Points to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								UITFTDSP_IMAGE__ShowImage(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	UITFTDSP_IMAGE_COMMON__ImageDetails*	pImage = pSpecificImage( pImageAppSpecific->Number );

	if ( pImage != 0 ) { DrawImage( pImageAppSpecific, pImage ); }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Displays the image specified assigning tag if one is defined.
 * @param:	pImageAppSpecific - Points to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								UITFTDSP_IMAGE__ShowImageWithAssociatedTag(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	UITFTDSP_IMAGE_COMMON__ImageDetails*	pImage 		 = pSpecificImage( pImageAppSpecific->Number );
	UITFTDSP_IMAGE_COMMON__ImageDetails*	pImageActive = pSpecificImage( pImageAppSpecific->Number_ActivePress );

	if ( pImage == COM_DEFN__NULL ) { return; }
	//
	// Set tag mask
	if ( pImageAppSpecific->iTagValue > 0 )
	{
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK( TFTDSPIF__ucTouchTagMask() ) );
		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG( (uint8_t)pImageAppSpecific->iTagValue ) );

		if ( ( pImageActive != 0 ) && ( (uint8_t)pImageAppSpecific->iTagValue == TFTDSPIF__ucTouchTagValue() ) )
		{
			// This image is being pressed and it has an active image counterpart
			// - Draw the active image
			DrawImage( pImageAppSpecific, pImageActive );
			// - Save the tag and start the persistence timer
			mActiveImage.ucTag = (uint8_t)pImageAppSpecific->iTagValue; GENTIMDT__StartTimer_TB_UIBased( pImageAppSpecific->ucPersistenceTime, &mActiveImage.PersistenceTimingCounter );
		}
		else
		if ( ( mActiveImage.ucTag == (uint8_t)pImageAppSpecific->iTagValue ) && ( mActiveImage.PersistenceTimingCounter.ulValue != 0 ) )
		{
			// Persistence timer for this tag is still running
			// - Draw the active image
			DrawImage( pImageAppSpecific, pImageActive );
		}
		else
		{
			// Draw the passive image
			DrawImage( pImageAppSpecific, pImage );
			// Reset active tag
			if ( mActiveImage.ucTag == (uint8_t)pImageAppSpecific->iTagValue ) { mActiveImage.ucTag = 0; }
		}

		FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__TAG_MASK( 0 ) );
	}
	else
	{
		// Draw the image
		DrawImage( pImageAppSpecific, pImage );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the current tag thatis causing an image to be displayed as active.
 * @param:	None.
 * @retval:	uint8_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t								UITFTDSP_IMAGE__ucActiveImageTag(void)
{
	return mActiveImage.ucTag;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all common images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS			UITFTDSP_IMAGE__UpdateFT800GRAM_CommonImages(FUNC_ACTION vAction)
{
	GRAMUpdateDetails	GRAMUpdate;

	GRAMUpdate.Type          = UI_TFTIMAGE__GUT__COMMON;
	GRAMUpdate.uiImageOffset = UITFTDSP_IMAGE__OFFSET_COMMON;
	GRAMUpdate.uiMaxImages   = UITFTDSP_IMAGE_COMMON__MAX_IMAGES;

	return UpdateFT800GRAM_Images( vAction, &GRAMUpdate );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all heating specific images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS			UITFTDSP_IMAGE__UpdateFT800GRAM_HeatingImages(FUNC_ACTION vAction)
{
	GRAMUpdateDetails	GRAMUpdate;

	GRAMUpdate.Type          = UI_TFTIMAGE__GUT__HEATING;
	GRAMUpdate.uiImageOffset = UITFTDSP_IMAGE__OFFSET_HEATING;
	GRAMUpdate.uiMaxImages   = UITFTDSP_IMAGE_HEATING__MAX_IMAGES;

	return UpdateFT800GRAM_Images( vAction, &GRAMUpdate );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all evaporative cooling specific images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction 	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS			UITFTDSP_IMAGE__UpdateFT800GRAM_EvpCoolImages(FUNC_ACTION vAction)
{
	GRAMUpdateDetails	GRAMUpdate;

	GRAMUpdate.Type          = UI_TFTIMAGE__GUT__EVPCOOL;
	GRAMUpdate.uiImageOffset = UITFTDSP_IMAGE__OFFSET_EVPCOOL;
	GRAMUpdate.uiMaxImages   = UITFTDSP_IMAGE_EVPCOOL__MAX_IMAGES;

	return UpdateFT800GRAM_Images( vAction, &GRAMUpdate );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes all cooling specific images to the FT800 graphic RAM (FT800__RAM_G)
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS			UITFTDSP_IMAGE__UpdateFT800GRAM_CoolingImages(FUNC_ACTION vAction)
{
	GRAMUpdateDetails	GRAMUpdate;

	GRAMUpdate.Type          = UI_TFTIMAGE__GUT__COOLING;
	GRAMUpdate.uiImageOffset = UITFTDSP_IMAGE__OFFSET_COOLING;
	GRAMUpdate.uiMaxImages   = UITFTDSP_IMAGE_COOLING__MAX_IMAGES;

	return UpdateFT800GRAM_Images( vAction, &GRAMUpdate );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the fan blade animation according to fan speed level specified.
 *
 * 			Fan icon 35 x 35 pixels - rotate around the centre of image.
 *
 * @param:	vFanSpeedLevel    - Actual fan speed.
 * 			pImageAppSpecific - Pointer to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								UITFTDSP_IMAGE__UpdateFanBladeAnimationAccordingToFanSpeedLevel(TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	uint8_t		ucMaxRotationNumber;
	//
	// Calculate the max number of rotations required (within 90 degree angle) according to the selected fan speed level.
	ucMaxRotationNumber = 90 / mkaucFanBladeRotationAngle[vFanSpeedLevel];

	if ( mFanBladeAnimation.DelayBeforeNextIconDisplayTimingCounter.ulValue == 0 )
	{
		// Start UI based timer
		GENTIMDT__StartTimer_TB_UIBased( 1, &mFanBladeAnimation.DelayBeforeNextIconDisplayTimingCounter );

   	    if ( mFanBladeAnimation.ucFanRotationNumber < ucMaxRotationNumber )
        {
   	    	// Increase the rotation number
   	    	mFanBladeAnimation.ucFanRotationNumber++;
        }
        else
        {
        	// Clear the rotation number
        	mFanBladeAnimation.ucFanRotationNumber = 0;
        }
	}
    //
	// Rotate fan icon
	UITFTDSP_IMAGE__RotateImage( mFanBladeAnimation.ucFanRotationNumber * mkaucFanBladeRotationAngle[vFanSpeedLevel], pImageAppSpecific );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rotates image by any angle specified assigning tag if a valid one is defined.
 * @param:	viRotationAngle   - Variable to pass rotation angle.
 * 			pImageAppSpecific - Pointer to image application specific defined info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void 								UITFTDSP_IMAGE__RotateImage(int16_t viRotationAngle, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific)
{
	UITFTDSP_IMAGE_COMMON__ImageDetails*	pImage = pSpecificImage(pImageAppSpecific->Number);
	float 									fTxFactor, fTyFactor;
	//
	// Calculate translate factor
	fTxFactor = (float)pImage->pDefinition->uiWidth / 2;
	fTyFactor = (float)pImage->pDefinition->uiHeight / 2;
	//
	// Rotate image
	// - Set current color to white
	UITFTDSP_COLOR__SetCurrentColor( UITFTDSP_COLOR__WHITE );
	// - Begin bitmap command
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BEGIN( FT800_DISPLIST__BEGIN_PRIM__BITMAPS ) );
	// - Set the current matrix to the identity matrix
    FT800_COPRCCMD__CPCmdQueue__CMD_LOADIDENTITY();
    // - Translate the current matrix around the centre:
    FT800_COPRCCMD__CPCmdQueue__CMD_TRANSLATE( fTxFactor * 65536, fTyFactor * 65536 );
    // - Rotate the current matrix
    FT800_COPRCCMD__CPCmdQueue__CMD_ROTATE( viRotationAngle * 65536 / 360 );
    // - Translate the current matrix back to top left
    FT800_COPRCCMD__CPCmdQueue__CMD_TRANSLATE( -fTxFactor * 65536, -fTyFactor * 65536 );
    // - Write the current matrix to display list
    FT800_COPRCCMD__CPCmdQueue__CMD_SETMATRIX();
    //
    // Check if the image has a valid tag value defined
    if ( pImageAppSpecific->iTagValue < 0 )
    {
    	// Image only
    	UITFTDSP_IMAGE__ShowImage(pImageAppSpecific);
    }
    else
    {
    	// Image with associated tag
    	UITFTDSP_IMAGE__ShowImageWithAssociatedTag(pImageAppSpecific);
    }
    //
    // End bitmap rotation command
	FT800_COPRCCMD__CPCmdQueue__CMD_LOADIDENTITY();
	FT800_COPRCCMD__CPCmdQueue__CMD_SETMATRIX();
	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__END() );

    return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Single image update start and stop functions.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void								UITFTDSP_IMAGE__SingleImageUpdate_Init(void)
{
	mSingleImageUpdate = UI_TFTIMAGE__GSIU__COMMON;
	mGRAMUpdateState   = UI_TFTIMAGE__GUS__INIT;

	return;
}
void								UITFTDSP_IMAGE__SingleImageUpdate_Stop(void)
{
	mSingleImageUpdate = UI_TFTIMAGE__GSIU__NONE;
	mGRAMUpdateState   = UI_TFTIMAGE__GUS__INIT;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns state of single image update - None, Common or Specific.
 * @param:	None.
 * @retval: bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool								UITFTDSP_IMAGE__bSingleImageUpdate_None(void)
{
	return (bool)( mSingleImageUpdate == UI_TFTIMAGE__GSIU__NONE );
}
bool								UITFTDSP_IMAGE__bSingleImageUpdate_Common(void)
{
	return (bool)( mSingleImageUpdate == UI_TFTIMAGE__GSIU__COMMON );
}
bool								UITFTDSP_IMAGE__bSingleImageUpdate_Specific(void)
{
	return (bool)( mSingleImageUpdate == UI_TFTIMAGE__GSIU__SPECIFIC );
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
//	MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Draws the bitmap image on the TFT display.
 * @param:	pImageAppSpecific - Pointer to application specific info for the image such as where it is located.
 * 			pImage			  - Pointer to image details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void									DrawImage(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific, UITFTDSP_IMAGE_COMMON__ImageDetails* pImage)
{
	// Set current color to white
	UITFTDSP_COLOR__SetCurrentColor(UITFTDSP_COLOR__WHITE);

	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BEGIN( FT800_DISPLIST__BEGIN_PRIM__BITMAPS ) );

	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BITMAP_SOURCE( pImage->ulRAMStartAddx ) );

	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BITMAP_LAYOUT( FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__ARGB1555, pImage->pDefinition->uiLineStride,	pImage->pDefinition->uiHeight ) );

	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__BITMAP_SIZE( FT800_DISPLIST__BITMAP_SIZE_FILTER__NEAREST, FT800_DISPLIST__BITMAP_SIZE_WRAP__BORDER, FT800_DISPLIST__BITMAP_SIZE_WRAP__BORDER, pImage->pDefinition->uiWidth, pImage->pDefinition->uiHeight ) );

	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__VERTEX2F( pImageAppSpecific->iXPos * 16, pImageAppSpecific->iYPos * 16 ) );

	FT800_COPRCCMD__CPDatQueue__Value( FT800_DISPLIST__DLCmd__END() );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns a pointer to the image using the image number as the identifier.
 * @param:	vImageNumber - Image number used to identify the image required.
 * @retval:	Pointer to UITFTDSP_IMAGE_COMMON__ImageDetails.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static UITFTDSP_IMAGE_COMMON__ImageDetails*	pSpecificImage(UITFTDSP_IMAGE__IMAGE_NUMBER vImageNumber)
{
	UITFTDSP_IMAGE_COMMON__ImageDetails*	pImage = 0;
	
	if ( vImageNumber < UITFTDSP_IMAGE__OFFSET_HEATING )
	{
		// Image details - Common
		pImage = UITFTDSP_IMAGE_COMMON__pImage( (UITFTDSP_IMAGE_COMMON__IMAGE_NUMBER)( vImageNumber - UITFTDSP_IMAGE__OFFSET_COMMON ) );
	}
	else
	if ( vImageNumber < UITFTDSP_IMAGE__OFFSET_EVPCOOL )
	{
		// Image details - Heating specific
		pImage = UITFTDSP_IMAGE_HEATING__pImage( (UITFTDSP_IMAGE_HEATING__IMAGE_NUMBER)( vImageNumber - UITFTDSP_IMAGE__OFFSET_HEATING ) );
	}
	else
	if ( vImageNumber < UITFTDSP_IMAGE__OFFSET_COOLING )
	{
		// Image details - Evap cooling specific
		pImage = UITFTDSP_IMAGE_EVPCOOL__pImage( (UITFTDSP_IMAGE_EVPCOOL__IMAGE_NUMBER)( vImageNumber - UITFTDSP_IMAGE__OFFSET_EVPCOOL ) );
	}
	else
	{
		// Image details - Cooling specific
		pImage = UITFTDSP_IMAGE_COOLING__pImage( (UITFTDSP_IMAGE_COOLING__IMAGE_NUMBER)( vImageNumber - UITFTDSP_IMAGE__OFFSET_COOLING ) );
	}
	
	return pImage;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Common function to update graphics RAM of FT800 with images.
 * @param:	vAction				- Action to take within function (Loop until complete or break out).
 * 			pGRAMUpdate			- Pointer to structure holding info specific to the type of images to be updated in the graphics RAM.
 * 			vbUpdateSingleImage	- Flag indicates whether or not to refresh image one at a time.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS					UpdateFT800GRAM_Images(FUNC_ACTION vAction, GRAMUpdateDetails* pGRAMUpdate)
{
	static uint16_t								suiImageCount = 0;
	static uint32_t								sulStartAddx;
	static UITFTDSP_IMAGE_COMMON__ImageDetails*	spImage;
	static bool									sbSpecificImageAddxSet = false;

	uint16_t									uiImageNumber;
	FT800_DEVICEIF__XFER_STATUS					XferStatus;

	MCUAXESS__FT800_SPIPrioritySetLo();			// Go lo priority on SPI, emilio 03-07-17

	if ( mGRAMUpdateState == UI_TFTIMAGE__GUS__INIT )
	{
		if ( mSingleImageUpdate == UI_TFTIMAGE__GSIU__NONE ) { suiImageCount = 0; }

		if ( suiImageCount == 0 )
		{
			if ( pGRAMUpdate->Type == UI_TFTIMAGE__GUT__COMMON )
			{
				// Common type  - Start of graphics RAM
				sulStartAddx = FT800__RAM_G;

				if ( !sbSpecificImageAddxSet ) { UITFTDSP_IMAGE_COMMON__SetNextStartAddx( sulStartAddx ); }
			}
			else
			{
				// Get next start addx
				UITFTDSP_IMAGE_COMMON__GetNextStartAddx( &sulStartAddx );
			}
		}
		// Define image pointer
		mGRAMUpdateState = UI_TFTIMAGE__GUS__DEFN;
	}

	XferStatus = FT800_DEVICEIF__XS_ACTV;

	do
	{
		if ( mGRAMUpdateState == UI_TFTIMAGE__GUS__DEFN )
		{
			uiImageNumber = pGRAMUpdate->uiImageOffset + suiImageCount;

			if ( ( spImage = pSpecificImage( (UITFTDSP_IMAGE__IMAGE_NUMBER)uiImageNumber ) ) == 0 )
			{
				suiImageCount++;
			}
			else
			{
				mGRAMUpdateState = UI_TFTIMAGE__GUS__WRTE;
			}
		}

		if ( mGRAMUpdateState == UI_TFTIMAGE__GUS__WRTE )
		{
			if ( ( XferStatus = WrImageToFT800GRAM( vAction, sulStartAddx, spImage ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				// Next addx is last addx + size of last data block
				sulStartAddx += spImage->pDefinition->ulDataSize;
				// Update only if common type
				if ( pGRAMUpdate->Type == UI_TFTIMAGE__GUT__COMMON )
				{
					if ( !sbSpecificImageAddxSet ) { UITFTDSP_IMAGE_COMMON__SetNextStartAddx( sulStartAddx ); }
				}
				// Next, define pointer
				mGRAMUpdateState = UI_TFTIMAGE__GUS__DEFN;
				// Next image
				suiImageCount++;
			}
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) && ( suiImageCount < pGRAMUpdate->uiMaxImages ) && ( mSingleImageUpdate == UI_TFTIMAGE__GSIU__NONE ) );
	//
	// Check the transfer status in the event not FUNC_LOOP
	if ( XferStatus == FT800_DEVICEIF__XS_ACTV )
	{
		if ( suiImageCount >= pGRAMUpdate->uiMaxImages )
		{
			// No images were ever transferred
			XferStatus       = FT800_DEVICEIF__XS_DONE_ERROR;
			// Revert update state to init
			mGRAMUpdateState = UI_TFTIMAGE__GUS__INIT;
		}
	}
	else
	if ( ( mSingleImageUpdate != UI_TFTIMAGE__GSIU__NONE ) && ( suiImageCount >= pGRAMUpdate->uiMaxImages ) )
	{
		suiImageCount = 0;		// Reset image count value

		if ( mSingleImageUpdate == UI_TFTIMAGE__GSIU__COMMON )
		{
			mSingleImageUpdate = UI_TFTIMAGE__GSIU__SPECIFIC;
		}
		else
		{
			mSingleImageUpdate = UI_TFTIMAGE__GSIU__COMMON;
		}
	}
	//
	// Reset update state once transfer no longer active
	if ( XferStatus != FT800_DEVICEIF__XS_ACTV )
	{
		mGRAMUpdateState = UI_TFTIMAGE__GUS__INIT;

		sbSpecificImageAddxSet = true;			// Added, emilio 30-10-15
	}

	MCUAXESS__FT800_SPIPrioritySetHi();			// Go hi priority on SPI, emilio 03-07-17

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes the image passed to FT800 graphics RAM at the addx specifed.  The start addx in the image structure is updated
 * 			with the image passed.
 * @param:	vAction:
 * 				- Action to take within function (Loop until complete or break out).
 * 			vulAddx:
 * 				- Address in FT800 to write image to.
 * 			pImage:
 * 				- Pointer to the image info.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static FT800_DEVICEIF__XFER_STATUS			WrImageToFT800GRAM(FUNC_ACTION vAction, uint32_t vulAddx, UITFTDSP_IMAGE_COMMON__ImageDetails* pImage)
{
	FT800_DEVICEIF__Buffer 	DataBuffer;
	//
	// Assign the start addx
	pImage->ulRAMStartAddx = vulAddx;
	//
	// Data buffer init
	DataBuffer.pucData 	   = (uint8_t*)pImage->pDefinition->pucTable;
	DataBuffer.ulMaxLength = pImage->pDefinition->ulDataSize;
	DataBuffer.ulLength    = pImage->pDefinition->ulDataSize;
	//
	// Now write the buffer
	return FT800_DEVICEIF__WrN8( vAction, pImage->ulRAMStartAddx, &DataBuffer );
}
//*********************************************************************************************************************************************************************************
