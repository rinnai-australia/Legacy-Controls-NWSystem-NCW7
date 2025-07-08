//*********************************************************************************************************************************************************************************
// uitftdsp.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: User Interface TFT Display - Main Access Point.
 *
 * 				Provides access to all TFT display related functionality.
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

#ifndef 		UITFTDSP_H_
#define 		UITFTDSP_H_

#include		<com_defn.h>
#include    	<uitftdsp_button.h>
#include		<uitftdsp_image.h>
#include		<uitftdsp_shape.h>
#include		<uitftdsp_color.h>
#include		<uitftdsp_number.h>
#include		<FT800_touchifc.h>
#include		<genprmdt.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	UI_TFTDISPLAY__FLASH_RATE {

	UITFTDSP__FR__1HZ = 0,
	UITFTDSP__FR__2HZ = 1,
	UITFTDSP__FR__4HZ = 2

} UI_TFTDISPLAY__FLASH_RATE;

#define		UI_TFTDISPLAY__MAX_FLASH_INTERVALS		3

typedef		FT800_TOUCHIFC__FT800TouchTagValue		UI_TFTDISPLAY__FT800TouchTagValue;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern const struct	GENPRMDT__ParameterValue	UITFTDSP__kTFTControlBacklightOnFullTimeoutInterval;
extern const struct	GENPRMDT__ParameterValue	UITFTDSP__kTFTControlBacklightDimmedTimeoutInterval;
extern const struct	GENPRMDT__ParameterValue	UITFTDSP__kTFTControlBacklightOnFullLevel;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern const struct	GENTIMDT__Timing			UITFTDSP__kTFTControlBacklightOnTimeoutTiming;
extern const struct	GENTIMDT__Timing			UITFTDSP__kTFTControlBacklightRampUpDownTiming;
extern const struct	GENTIMDT__Timing			UITFTDSP__kTFTControlFlash1HzTiming;
extern const struct	GENTIMDT__Timing			UITFTDSP__kTFTControlFlash2HzTiming;
extern const struct	GENTIMDT__Timing			UITFTDSP__kTFTControlFlash4HzTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void							UITFTDSP__Initialise(void);

extern void							UITFTDSP__BacklightOperation_Wakeup(void);
extern bool							UITFTDSP__bDisplayShutdownIsActive(void);

extern void							UITFTDSP__FlashControl_Update(void);
extern void							UITFTDSP__TouchControl_Update(void);
extern void							UITFTDSP__BacklightControl_Update(bool vbLoPowerMode, bool vbTestMode, uint8_t vucTestModeLevel);
extern uint8_t						UITFTDSP__Backlight_ucOnFullTimeoutInterval(void);
extern void							UITFTDSP__Backlight_KeepAtFullBrightness(bool vbKeepAtFullBrightness);

extern uint8_t						UITFTDSP__TouchControl_ucTagMaskValue(void);
extern void							UITFTDSP__TouchControl_RapidUpdateDataInit(void);
extern void							UITFTDSP__TouchControl_RapidUpdateDataDefine(FT800_TOUCHIFC__FT800TouchTagValue* pTouchTagValue);
extern uint8_t						UITFTDSP__TouchControl_ucActiveTag(void);

extern void							UITFTDSP__StartDisplayRefresh(void);
extern void							UITFTDSP__StopDisplayRefresh(void);

extern bool							UITFTDSP__ScreenShot_bEnabled(void);
extern bool							UITFTDSP__ScreenShot_bDisabled(void);
extern bool							UITFTDSP__ScreenShot_bStarted(uint16_t vuiYLineCount);
extern bool							UITFTDSP__ScreenShot_bBusyStatusCheckValid(bool* pbIsBusy);
extern bool							UITFTDSP__ScreenShot_bRGBDataRetrieved(uint8_t* pucRGBLineBuffer);

extern void							UITFTDSP__FlashImage(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);
extern void							UITFTDSP__ShowImage(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);
extern void							UITFTDSP__ShowImageWithAssociatedTag(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);

extern FT800_DEVICEIF__XFER_STATUS	UITFTDSP__UpdateFT800GRAM_CommonImages(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS	UITFTDSP__UpdateFT800GRAM_HeatingImages(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS	UITFTDSP__UpdateFT800GRAM_EvpCoolImages(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS	UITFTDSP__UpdateFT800GRAM_CoolingImages(FUNC_ACTION vAction);

extern void							UITFTDSP__FlashButton(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UITFTDSP_BUTTON__ButtonDefinition* pButton);
extern void 					   	UITFTDSP__ShowButton(UITFTDSP_BUTTON__ButtonDefinition* pButton);
extern void    						UITFTDSP__ShowButtonWithColor(UITFTDSP_BUTTON__ButtonDefinition* pButton);
extern void    						UITFTDSP__ShowButtonWithTwoTexts(UITFTDSP_BUTTON__ButtonWithTwoTextsDefinition* pButtonWithTwoTexts);

extern void							UITFTDSP__ShowRectangleWithFlashingText(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UI_TFTSHAPE__RectangleWithTextDetails* pRectangle);
extern void							UITFTDSP__ShowRectangleWithText(UI_TFTSHAPE__RectangleWithTextDetails* pRectangle);
extern void							UITFTDSP__FlashRectangle(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UI_TFTSHAPE__Rectangle* pRectangle);
extern void							UITFTDSP__ShowRectangle(UI_TFTSHAPE__Rectangle* pRectangle);

extern void   	 					UITFTDSP__ShowBarGraph(uint8_t vucLevel, UI_TFTSHAPE__BarGraphDetails* pBarGraph);
extern void   						UITFTDSP__ShowLevelButtons(UITFTDSP_BUTTON__LevelButtonDetails* pLevelButtons);

extern void							UITFTDSP__UpdateFanBladeAnimationAccordingToFanSpeedLevel(TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);

extern void							UITFTDSP__ShowTextAbsolute(UITFTDSP_TEXT__TextDetails* pText);
extern void							UITFTDSP__FlashTextAbsolute(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UITFTDSP_TEXT__TextDetails* pText);
extern void							UITFTDSP__AddFlashingCharacterToString(UI_TFTDISPLAY__FLASH_RATE vFlashRate, char vcChar, GENSTRDT__String* pString);
extern void							UITFTDSP__ShowTextAbsoluteWithCenteredY(UITFTDSP_TEXT__TextDetails* pText);
extern void							UITFTDSP__FlashTextAbsoluteWithCenteredY(UI_TFTDISPLAY__FLASH_RATE vFlashRate, UITFTDSP_TEXT__TextDetails* pText);

extern void							UITFTDSP__ShowNumber(UI_TFTNUMBER__NumberDetails* pNumber);

extern void							UITFTDSP__ShowDegreeSymbol(UI_TFTSHAPE__PointDetails* pOuterPoint, UI_TFTSHAPE__PointDetails* pInnerPoint);

extern void							UITFTDSP__SetCurrentColor(uint32_t vulColor);
extern void							UITFTDSP__RotateImage(int16_t viRotationAngle, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);
//*********************************************************************************************************************************************************************************

#endif /* UITFTDSP_H_ */
