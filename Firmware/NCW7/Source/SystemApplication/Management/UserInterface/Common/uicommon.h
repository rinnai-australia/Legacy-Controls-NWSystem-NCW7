//*********************************************************************************************************************************************************************************
// uicommon.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: User Interface Common Functionality.
 *
 * 				Container for user interface functionalities that are common to the application.
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

#ifndef 		UICOMMON_H_
#define 		UICOMMON_H_

#include		<com_defn.h>
#include		<uitftdsp.h>
#include		<nwsutils.h>
#include		<rtmclkif.h>
#include		<genstrdt.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	UI_COMMON__TIME_AMPM {					// Time AM/PM details
														//
	UI_COMMON_TF_NONE = 0,								// - None : 24hr mode
	UI_COMMON_TF_AM   = 1,								// - AM
	UI_COMMON_TF_PM   = 2								// - PM

} UI_COMMON__TIME_AMPM;
// Common bottom status number
typedef enum 	UI_COMMON__BOTTOMSTATUS_NUMBER {		// Bottom status image number
														//
	UI_COMMON__BSN_SPANNER	=	0,						// - Spanner
	UI_COMMON__BSN_LOCK		=	1,						// - Pad lock
	UI_COMMON__BSN_HEART	=	2,						// - Heart
	UI_COMMON__BSN_MASTER	=	3						// - Master networker

} UI_COMMON__BOTTOMSTATUS_NUMBER;

#define			UI_COMMON__BOTTOMSTATUS_MAXIMAGES		4
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct 	UICOMMON__TopStatusDetails {											// Top status structure
																						//
	RTMCLKIF__ClockDetails*					pClock;										// - Pointer to clock info
	bool									bClockIn12HourFormat;						// - Flags whether or not the clock is in 12hr format
																						//
	char*									pstrTextToDisplay;							// - Pointer to text to display
	UI_TFTDISPLAY__FLASH_RATE				TextFlashRate;								// - If text to be flashed the flash rate
	bool									bFlashText;									// - Flags whether to flash text
																						//
	uint8_t*								pucOutdoorTemperature;						// - Pointer to outdoor temperature
																						//
	UI_TFTIMAGE__ImageAppSpecificDetails*	pImage1;									// - Image 1 pointer
	UI_TFTIMAGE__ImageAppSpecificDetails*	pImage2;									// - Image 2 pointer
	UI_TFTIMAGE__ImageAppSpecificDetails*	pImage3;									// - Image 3 pointer
	UI_TFTIMAGE__ImageAppSpecificDetails*	pImage4;									// - Image 4 pointer, NC7090_DFDISP
	UI_TFTIMAGE__ImageAppSpecificDetails*	pImage5;									// - Image 5 pointer, NC7090_DFDISP

} UICOMMON__TopStatusDetails;

typedef struct	UICOMMON__ImageDisplayDetails {											// Image status structure for bottom status inclusion
																						//
	UI_TFTIMAGE__ImageAppSpecificDetails*	pDetails;									// - Pointer to image
	UI_TFTDISPLAY__FLASH_RATE				FlashRate;									// - Flash rate when flashing image
	bool									bFlashImage;								// - Flash image flag

} UICOMMON__ImageDisplayDetails;

typedef struct 	UICOMMON__BottomStatusDetails {											// Bottom status structure
																						//
	UITFTDSP_TEXT__TextDetails*				pText;										//
	UICOMMON__ImageDisplayDetails			aImage[UI_COMMON__BOTTOMSTATUS_MAXIMAGES];	//

} UICOMMON__BottomStatusDetails;

typedef struct	UICOMMON__TemperatureDetails {											// Temperature display structure
																						//
	UI_TFTNUMBER__NumberDetails				Temperature;								// - Number to describe temperature degree
	UI_TFTSHAPE__PointDetails				DegSymbol;									// - Point to describe degree symbol
	uint32_t								ulBGColor;									// - Background color

} UICOMMON__TemperatureDetails;
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
extern const struct	GENTIMDT__Timing			UICOMMON__kModeSwitchDelayTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void								UICOMMON__TFTDisplayList_Init_NormalOperation(uint32_t vulBackgroundColor);
extern void								UICOMMON__TFTDisplayList_Init_DisplayShutdown(void);

extern FT800_DEVICEIF__XFER_STATUS		UICOMMON__TFTDisplayList_Execute(FUNC_ACTION vAction);

extern void								UICOMMON__TopStatus_InitStructure(UICOMMON__TopStatusDetails* pTopStatus);
extern void								UICOMMON__TopStatus_Update(UICOMMON__TopStatusDetails* pTopStatus);

extern void								UICOMMON__BottomStatus_InitStructure(void);
extern void								UICOMMON__BottomStatus_Update(void);
extern void								UICOMMON__ShowBottomStatus(bool vbShowStauts);
extern void								UICOMMON__BottomStatus_UpdateText(void);
extern void								UICOMMON__BottomStatus_UpdateImage(UI_COMMON__BOTTOMSTATUS_NUMBER vIndex, bool vbFlashImage, UI_TFTDISPLAY__FLASH_RATE vFlashRate);

extern void    							UICOMMON__ShowRoomTemperature(uint8_t vucRoomTemperature);
extern void    							UICOMMON__ShowLargeNumber(UI_TFTNUMBER__NumberDetails* pNumber);

extern char*							UICOMMON__pstrDayOfWeek(RTMCLKIF__DAY_OF_WEEK vDayOfWeek);
extern void								UICOMMON__AssignTimeToString(bool vbIn12HrFormat, uint8_t vucHour, uint8_t vucMinute, GENSTRDT__String* pTime);
extern uint8_t							UICOMMON__ucTimeHourAfterConversion(bool vbIn12HrFormat, uint8_t vucHour, UI_COMMON__TIME_AMPM* pTimeAmPm);

extern NWSUTILS__CLIMATE_CONTROL_MODE	UICOMMON__CurrentClimateControlMode(NWSUTILS__CLIMATE_CONTROL_MODE vMode);
extern void								UICOMMON__StartModeSwitchDelayTimer(void);
extern bool								UICOMMON__bMasterModeSwitch(void);
extern void								UICOMMON__SetMasterModeSwitch(void);
//*********************************************************************************************************************************************************************************

#endif /* UICOMMON_H_ */
