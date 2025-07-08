//*********************************************************************************************************************************************************************************
// uitftdsp_image.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: User Interface TFT Based Images - Main Access Point.
 *
 * 				Provides access to all TFT image related functionality.
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

#ifndef 		UITFTDSP_IMAGE_H_
#define 		UITFTDSP_IMAGE_H_

#include		<com_defn.h>
#include		<uitftdsp_image_heating.h>
#include		<uitftdsp_image_evpcool.h>
#include		<uitftdsp_image_cooling.h>
#include		<uitftdsp_image_common.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<twbifutl.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			UITFTDSP_IMAGE__OFFSET_COMMON		0
#define			UITFTDSP_IMAGE__OFFSET_HEATING		1000
#define			UITFTDSP_IMAGE__OFFSET_EVPCOOL		2000
#define			UITFTDSP_IMAGE__OFFSET_COOLING		3000

typedef enum	UITFTDSP_IMAGE__IMAGE_NUMBER
{
	//
	// COMMON
	//
	UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_PASSIVE   				= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__MENU_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__MENU_BTN_ACTIVE   				= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__MENU_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE       			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__HELP_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE        			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__HELP_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__ONOFF_BTN_PASSIVE      			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__ONOFF_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__ONOFF_BTN_ACTIVE       			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__ONOFF_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__MODE_BTN_PASSIVE       			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__MODE_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__MODE_BTN_ACTIVE        			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__MODE_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__ZONE_BTN_PASSIVE       			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__ZONE_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__ZONE_BTN_ACTIVE       		 	= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__ZONE_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__BRIVIS_LOGO            			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__BRIVIS_LOGO,
	UITFTDSP_IMAGE__IN__COMMON__CLOCK_ICON             			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__CLOCK_ICON,
	UITFTDSP_IMAGE__IN__COMMON__HEART_ICON            	 		= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__HEART_ICON,
	UITFTDSP_IMAGE__IN__COMMON__M_ICON                 			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__M_ICON,
	UITFTDSP_IMAGE__IN__COMMON__LOCK_ICON              			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__LOCK_ICON,
	UITFTDSP_IMAGE__IN__COMMON__SPANNER_ICON           			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__SPANNER_ICON,
	UITFTDSP_IMAGE__IN__COMMON__FAN_ICON               			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__FAN_ICON,
	UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_PASSIVE       			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__LEVEL_UP_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__LEVEL_UP_ACTIVE        			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__LEVEL_UP_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_PASSIVE       			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__LEVEL_DN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__LEVEL_DN_ACTIVE        			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__LEVEL_DN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__FONT_METRIC            			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__FONT_METRIC,
	UITFTDSP_IMAGE__IN__COMMON__FONT_BMP               			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__FONT_BMP,
	UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE				= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__BACK_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE					= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__BACK_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_PASSIVE 			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__FAN_MENU_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__FAN_MENU_BTN_ACTIVE 			= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__FAN_MENU_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__SCHEDULE_LEVEL_UP_BTN_PASSIVE	= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__SCHEDULE_LEVEL_UP_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__COMMON__SCHEDULE_LEVEL_UP_BTN_ACTIVE 	= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__SCHEDULE_LEVEL_UP_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__COMMON__ALERT_ICON						= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__ALERT_ICON,

	UITFTDSP_IMAGE__IN__COMMON__DF_GAS_BTN_PASSIVE				= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__DF_GAS_BTN_PASSIVE,			// Duel fuel GAS button, NC7090_DFDISP
	UITFTDSP_IMAGE__IN__COMMON__DF_GAS_BTN_ACTIVE				= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__DF_GAS_BTN_ACTIVE,				//
	UITFTDSP_IMAGE__IN__COMMON__DF_ELEC_BTN_PASSIVE				= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__DF_ELEC_BTN_PASSIVE,			// Duel fuel ELEC button, NC7090_DFDISP
	UITFTDSP_IMAGE__IN__COMMON__DF_ELEC_BTN_ACTIVE				= UITFTDSP_IMAGE__OFFSET_COMMON + UITFTDSP_IMAGE_COMMON__IN__DF_ELEC_BTN_ACTIVE,			//
	//
	// HEATING
	//
	UITFTDSP_IMAGE__IN__HEATING__FLAME_ICON		 				= UITFTDSP_IMAGE__OFFSET_HEATING + UITFTDSP_IMAGE_HEATING__IN__FLAME_ICON,
	UITFTDSP_IMAGE__IN__HEATING__LIGHTNING_BOLT_ICON		    = UITFTDSP_IMAGE__OFFSET_HEATING + UITFTDSP_IMAGE_HEATING__IN__LIGHTNING_BOLT_ICON,			// NC7090_DFDISP
	//
	// EVPCOOL
	//
	UITFTDSP_IMAGE__IN__EVPCOOL__FAN_BTN_PASSIVE  				= UITFTDSP_IMAGE__OFFSET_EVPCOOL + UITFTDSP_IMAGE_EVPCOOL__IN__FAN_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__EVPCOOL__FAN_BTN_ACTIVE   				= UITFTDSP_IMAGE__OFFSET_EVPCOOL + UITFTDSP_IMAGE_EVPCOOL__IN__FAN_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__EVPCOOL__PUMP_BTN_PASSIVE 				= UITFTDSP_IMAGE__OFFSET_EVPCOOL + UITFTDSP_IMAGE_EVPCOOL__IN__PUMP_BTN_PASSIVE,
	UITFTDSP_IMAGE__IN__EVPCOOL__PUMP_BTN_ACTIVE  				= UITFTDSP_IMAGE__OFFSET_EVPCOOL + UITFTDSP_IMAGE_EVPCOOL__IN__PUMP_BTN_ACTIVE,
	UITFTDSP_IMAGE__IN__EVPCOOL__DROP_ICON        				= UITFTDSP_IMAGE__OFFSET_EVPCOOL + UITFTDSP_IMAGE_EVPCOOL__IN__DROP_ICON,
	UITFTDSP_IMAGE__IN__EVPCOOL__RIPPLE_ICON      				= UITFTDSP_IMAGE__OFFSET_EVPCOOL + UITFTDSP_IMAGE_EVPCOOL__IN__RIPPLE_ICON,
	//
	// COOLING
	//
	UITFTDSP_IMAGE__IN__COOLING__SNOWMAN_ICON 				   	= UITFTDSP_IMAGE__OFFSET_COOLING + UITFTDSP_IMAGE_COOLING__IN__SNOWMAN_ICON

} UITFTDSP_IMAGE__IMAGE_NUMBER;

#define			UITFTDSP_IMAGE__NO_IMAGE		65535
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct 	UI_TFTIMAGE__ImageAppSpecificDetails {				// Image application specific details
																	//
	UITFTDSP_IMAGE__IMAGE_NUMBER	Number;							// - Passive image number
	UITFTDSP_IMAGE__IMAGE_NUMBER	Number_ActivePress;				// - Active image number
																	//
	int16_t							iXPos;							// - X posn
	int16_t							iYPos;							// - Y posn
    uint8_t							ucPersistenceTime;				// - Time to persist indication
	int								iTagValue;						// - Press tag value

} UI_TFTIMAGE__ImageAppSpecificDetails;
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
extern const struct	GENTIMDT__Timing	UITFTDSP_IMAGE__kFanBladeDelayIconDisplayTiming;
extern const struct	GENTIMDT__Timing	UITFTDSP_IMAGE__kActiveImagePersistenceTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void								UITFTDSP_IMAGE__Initialise(void);

extern void								UITFTDSP_IMAGE__ShowImage(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);
extern void								UITFTDSP_IMAGE__ShowImageWithAssociatedTag(UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);
extern uint8_t							UITFTDSP_IMAGE__ucActiveImageTag(void);

extern FT800_DEVICEIF__XFER_STATUS		UITFTDSP_IMAGE__UpdateFT800GRAM_CommonImages(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS		UITFTDSP_IMAGE__UpdateFT800GRAM_HeatingImages(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS		UITFTDSP_IMAGE__UpdateFT800GRAM_EvpCoolImages(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS		UITFTDSP_IMAGE__UpdateFT800GRAM_CoolingImages(FUNC_ACTION vAction);

extern void								UITFTDSP_IMAGE__UpdateFanBladeAnimationAccordingToFanSpeedLevel(TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);
extern void 							UITFTDSP_IMAGE__RotateImage(int16_t viRotationAngle, UI_TFTIMAGE__ImageAppSpecificDetails* pImageAppSpecific);

extern void								UITFTDSP_IMAGE__SingleImageUpdate_Init(void);
extern void								UITFTDSP_IMAGE__SingleImageUpdate_Stop(void);

extern bool								UITFTDSP_IMAGE__bSingleImageUpdate_None(void);
extern bool								UITFTDSP_IMAGE__bSingleImageUpdate_Common(void);
extern bool								UITFTDSP_IMAGE__bSingleImageUpdate_Specific(void);
//*********************************************************************************************************************************************************************************

#endif /* UITFTDSP_IMAGE_H_ */
