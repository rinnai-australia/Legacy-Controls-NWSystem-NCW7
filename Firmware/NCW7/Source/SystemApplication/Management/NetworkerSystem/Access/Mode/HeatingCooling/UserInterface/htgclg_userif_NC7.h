//*********************************************************************************************************************************************************************************
// htgclg_userif_NC7.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Heating/Cooling Common Display/Control - N-C7 Specific.
 *
 * 				Networker common heating/cooling UI.
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

#ifndef 		HTGCLG_USERIF_NC7_H_
#define 		HTGCLG_USERIF_NC7_H_

#include		<com_defn.h>
#include		<htgclg_common.h>
#include		<nws_svcinf_common.h>
#include		<nwsutils.h>
#include		<uicommon.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	HTGCLG_USERIF_NC7__TAG_VALUE {

	HTGCLG_USERIF_NC7__TV__HELP_BTN	  					= 1,
	HTGCLG_USERIF_NC7__TV__MENU_BTN	  					= 2,
	HTGCLG_USERIF_NC7__TV__BACK_BTN	  					= 3,
	HTGCLG_USERIF_NC7__TV__FAN_MENU_BTN	  				= 4,
	HTGCLG_USERIF_NC7__TV__ONOFF_BTN	  				= 5,
	HTGCLG_USERIF_NC7__TV__MODE_BTN	  					= 6,
	HTGCLG_USERIF_NC7__TV__ZONES_BTN  					= 7,

	HTGCLG_USERIF_NC7__TV__LEVEL_UP_BTN 				= 11,
	HTGCLG_USERIF_NC7__TV__LEVEL_DN_BTN  				= 12,
	HTGCLG_USERIF_NC7__TV__SET_TEMP_UP_BTN 				= 13,
	HTGCLG_USERIF_NC7__TV__SET_TEMP_DN_BTN  			= 14,
	HTGCLG_USERIF_NC7__TV__MANUAL_BTN	  				= 15,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_BTN	  				= 16,
	HTGCLG_USERIF_NC7__TV__ADVANCE_PERIOD_BTN			= 17,
	HTGCLG_USERIF_NC7__TV__CANCEL_BTN	  				= 18,

	HTGCLG_USERIF_NC7__TV__SCHEDULE_MONDAY_BTN	  		= 19,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_TUESDAY_BTN	  		= 20,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_WEDNESDAY_BTN	  	= 21,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_THURSDAY_BTN	  	= 22,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_FRIDAY_BTN	  		= 23,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_SATURDAY_BTN	  	= 24,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_SUNDAY_BTN	  		= 25,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKDAYS_BTN	  	= 26,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_WEEKENDS_BTN	  	= 27,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_BTN	  		= 28,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_BTN	  		= 29,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_BTN	  		= 30,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_BTN	  	= 31,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_BTN	  		= 32,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TIME_BTN	  	= 33,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TIME_BTN	  	= 34,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TIME_BTN	  	= 35,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TIME_BTN	= 36,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TIME_BTN	  	= 37,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_WAKE_TEMP_BTN	  	= 38,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEAVE_TEMP_BTN	  	= 39,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_RETURN_TEMP_BTN	  	= 40,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_PRESLEEP_TEMP_BTN	= 41,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_SLEEP_TEMP_BTN	  	= 42,

	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEVEL_UP_BTN	  	= 43,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_LEVEL_DN_BTN	  	= 44,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_A_BTN	  		= 45,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_B_BTN	  		= 46,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_C_BTN	  		= 47,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_ZONE_D_BTN	  		= 48,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_CANCEL_BTN	  		= 49,

	HTGCLG_USERIF_NC7__TV__ALERT_BTN  					= 50,
	HTGCLG_USERIF_NC7__TV__SCHEDULE_RESTORE_DFLTS_BTN	= 51,

	HTGCLG_USERIF_NC7__TV__DF_ELEC_BTN					= 52,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__TV__DF_GAS_BTN					= 53,		// NC7090_DFDISP
	HTGCLG_USERIF_NC7__TV__DF_AUTO_BTN					= 54		// NC7090_DFDISP

} HTGCLG_USERIF_NC7__TAG_VALUE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	HTGCLG_USERIF_NC7__TemperatureControlSpecificDetails {

	bool											bShowSetTemperature;
	uint8_t											ucSetTemperature;
	UITFTDSP_BUTTON__LevelButtonDetails* 			pLevelButtons;
	bool											bShowRoomTemperature;
	TWBIFUTL__MSPControlZoneDescriptionDetails* 	pMSPControl;

} HTGCLG_USERIF_NC7__TemperatureControlSpecificDetails;
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
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void										HTGCLG_USERIF_NC7__Initialise(void);

extern NWS_SVCINF_COMMON__ServiceInfoDetails*	HTGCLG_USERIF_NC7__pServiceInfoDetails(void);
extern bool										HTGCLG_USERIF_NC7__bViewingServiceInfoPrompt(HTGCLG_COMMON__ControlDetails* pHeatingCooling);

extern void										HTGCLG_USERIF_NC7__UpdateDisplay_Mode_Programming(HTGCLG_COMMON__ControlDetails* pHeatingCooling);
extern void										HTGCLG_USERIF_NC7__UpdateControl_Mode_Programming(HTGCLG_COMMON__ControlDetails* pHeatingCooling);

extern void										HTGCLG_USERIF_NC7__UpdateDisplay_Mode_Off(HTGCLG_COMMON__ControlDetails* pHeatingCooling);

extern void										HTGCLG_USERIF_NC7__UpdateDisplay_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pHeatingCooling);
extern void										HTGCLG_USERIF_NC7__UpdateControl_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pHeatingCooling);

extern void										HTGCLG_USERIF_NC7__UpdateDisplay_Mode_On(HTGCLG_COMMON__ControlDetails* pHeatingCooling);
extern void										HTGCLG_USERIF_NC7__UpdateDisplay_Mode_NormalOperation(NWSUTILS__CLIMATE_CONTROL_MODE vMode, HTGCLG_COMMON__ControlDetails* pHeatingCooling);

extern void										HTGCLG_USERIF_NC7__RapidUpdateControl_None(void);
extern void										HTGCLG_USERIF_NC7__RapidUpdateControl_Time(void);
extern void										HTGCLG_USERIF_NC7__RapidUpdateControl_FanSpeedLevel(bool vbIncreasingValue, TWBIFUTL__FAN_SPEED_LEVEL vFanSpeedLevel);
extern void										HTGCLG_USERIF_NC7__RapidUpdateControl_SetTemperature(bool vbIncreasingValue, uint8_t vucSetTemperature);

extern void										HTGCLG_USERIF_NC7__UpdateTemperatureControlDetailsWhenInClimateControlState(HTGCLG_USERIF_NC7__TemperatureControlSpecificDetails* pTempCtrlSpecific, GENSTRDT__String* pSetTemp);

extern void										HTGCLG_USERIF_NC7__EnableScheduleAccessByThisNetworker(bool vbEnableAccess);
extern bool										HTGCLG_USERIF_NC7__bScheduleAccessEnabledByThisNetworker(void);
//*********************************************************************************************************************************************************************************

#endif 	/* HTGCLG_USERIF_NC7_H_ */
