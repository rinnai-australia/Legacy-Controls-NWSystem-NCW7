//*********************************************************************************************************************************************************************************
// nws_prompt_common.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker Information Prompt - Common.
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

#ifndef 		NWS_PROMPT_COMMON_H_
#define 		NWS_PROMPT_COMMON_H_

#include		<com_defn.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_PROMPT_COMMON__CONTROL_BUTTONS {					// Networker Info Prompt
																		//
	NWS_PROMPT_COMMON__CB__NONE_PRESSED  			 = 0,				// - No button
	NWS_PROMPT_COMMON__CB__BACK			  			 = 1,				// - Cancel prompt
	NWS_PROMPT_COMMON__CB__PROMPT_OK				 = 2,				// - Prompt Ok button
	NWS_PROMPT_COMMON__CB__PROMPT_YES	  			 = 3,				// - Prompt Yes button
	NWS_PROMPT_COMMON__CB__PROMPT_NO		 		 = 4				// - Prompt No button

} NWS_PROMPT_COMMON__CONTROL_BUTTONS;

typedef enum	NWS_PROMPT_COMMON__MESSAGE {							// Message Types
																		//
	NWS_PROMPT_COMMON__MSG__NONE   					=  0,				// - None
	NWS_PROMPT_COMMON__MSG__CLOCK_SET_MASTER   		=  1,				// - Clock can only be set at master
	NWS_PROMPT_COMMON__MSG__INSTALL_AXS_MASTER 		=  2,				// - Install params can only be accessed by master
	NWS_PROMPT_COMMON__MSG__SERVICE_AXS_MASTER 		=  3,				// - Service params can only be accessed by master
	NWS_PROMPT_COMMON__MSG__INSTALL_AXS_SWITCH_OFF  =  4,				// - Install params can only be accessed while system off
	NWS_PROMPT_COMMON__MSG__SERVICE_AXS_SWITCH_OFF  =  5,				// - Service params can only be accessed while system off
	NWS_PROMPT_COMMON__MSG__SYST_SETTINGS_MASTER    =  6,				// - System settings master only
	NWS_PROMPT_COMMON__MSG__ZONE_NAMES_MASTER       =  7,				// - Zone name setting master only
	NWS_PROMPT_COMMON__MSG__ZONE_NAMES_NO_ZONES  	=  8, 				// - No zones exist
	NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_EVPCOOL  	=  9,  				// - Schedule unavailable to evaporative cooling mode
	NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_NOCNTRL  	= 10,  				// - Schedule unavailable to no control mode
	NWS_PROMPT_COMMON__MSG__NO_SCHEDULE_FANONLY  	= 11,  				// - Schedule unavailable to fan only mode
	NWS_PROMPT_COMMON__MSG__RECAL_TOUCH_SCREEN		= 12,				// - Recalibrate touch screen
	NWS_PROMPT_COMMON__MSG__RESET_NETWORKER_ID		= 13,				// - Reset networker ID
	NWS_PROMPT_COMMON__MSG__RESTORE_DEFAULTS		= 14,				// - Restore networker defaults
	NWS_PROMPT_COMMON__MSG__SYST_LOKOUT_REGNWK		= 15,				// - System can only be locked out by a registered networker
	NWS_PROMPT_COMMON__MSG__SYST_UNLOCK_REGNWK		= 16				// - System can only be unlocked by a registered networker

} NWS_PROMPT_COMMON__MESSAGE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	NWS_PROMPT_COMMON__UserControlDetails {					// Networker Info Prompt - User Control Specific (UI)
																		//
	NWS_PROMPT_COMMON__CONTROL_BUTTONS		ButtonPressed;				// - Control button pressed

} NWS_PROMPT_COMMON__UserControlDetails;

typedef struct	NWS_PROMPT_COMMON__UserAccessDetails {					// Networker Info Prompt - User Access Control
																		//
	NWS_PROMPT_COMMON__UserControlDetails	Control;					// - User access UI control interface : Info Prompt

} NWS_PROMPT_COMMON__UserAccessDetails;

typedef struct	NWS_PROMPT_COMMON__ControlDetails {						// Networker Info Prompt - Main Control Structure
																		//
	NWS_PROMPT_COMMON__MESSAGE				Message;					// - Message to display
	GENTIMDT__TimingCounter					AutoExitDelayTimingCounter;	// - Timer to time how long prompt is displayed
																		//
	bool									bWasBeingDisplayed;			// - Flag indicating prompt was being displayed, emilio 21-12-15
																		//
	NWS_PROMPT_COMMON__UserAccessDetails	UserAccess;					// - User access

} NWS_PROMPT_COMMON__ControlDetails;
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

#endif /* NWS_PROMPT_COMMON_H_ */
