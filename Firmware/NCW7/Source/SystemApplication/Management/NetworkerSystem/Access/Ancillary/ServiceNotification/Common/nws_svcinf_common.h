//*********************************************************************************************************************************************************************************
// nws_svcinf_common.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker Service Information - Common.
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

#ifndef 		NWS_SVCINF_COMMON_H_
#define 		NWS_SVCINF_COMMON_H_

#include		<com_defn.h>
#include		<gentimdt.h>
#include		<twbcomif.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_SVCINF_COMMON__CONTROL_BUTTONS {					// Networker Service Info
																		//
	NWS_SVCINF_COMMON__CB__NONE_PRESSED  		= 0,					// - No button
	NWS_SVCINF_COMMON__CB__BACK			  		= 1,					// - Cancel prompt
	NWS_SVCINF_COMMON__CB__OK					= 2,					// - Prompt Ok button
	NWS_SVCINF_COMMON__CB__RESET				= 3,					// - Prompt Reset button
	NWS_SVCINF_COMMON__CB__CLEAR				= 4						// - Prompt Clear button

} NWS_SVCINF_COMMON__CONTROL_BUTTONS;

typedef enum 	NWS_SVCINF_COMMON__MESSAGE_TYPE {						// Networker Service Info - Message Type
																		//
	NWS_SVCINF_COMMON__MT__NONE  			 	= 0,					// - None
	NWS_SVCINF_COMMON__MT__FAULT_TRANSIENT		= 1,					// - Fault : Transient
	NWS_SVCINF_COMMON__MT__FAULT_LOCKOUT		= 2,					// - Fault : Lockout
	NWS_SVCINF_COMMON__MT__SERVICE_NOTIFY		= 3						// - Service Notification

} NWS_SVCINF_COMMON__MESSAGE_TYPE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct 	NWS_SVCINF_COMMON__ServiceInfoDetails {					// Networker Service Info - Application Access Details
																		//
	NWS_SVCINF_COMMON__MESSAGE_TYPE			MessageType;				// - Type of message
																		//
	TWBCOMIF__GROUPID						GroupID;					// - Device group
	TWBCOMIF__UNITID						UnitID;						// - Device unit
																		//
	uint8_t									ucFaultCode;				// - Fault code
	uint8_t									ucMaxUnits;					// - Maximum number of group units
	uint16_t								uiFanHoursLogged;			// - Fan hours logged so far

} NWS_SVCINF_COMMON__ServiceInfoDetails;


typedef struct	NWS_SVCINF_COMMON__UserControlDetails {					// Networker Service Info - User Control Specific (UI)
																		//
	NWS_SVCINF_COMMON__CONTROL_BUTTONS		ButtonPressed;				// - Control button pressed

} NWS_SVCINF_COMMON__UserControlDetails;

typedef struct	NWS_SVCINF_COMMON__UserAccessDetails {					// Networker Service Info - User Access Control
																		//
	bool									bResetPressed;				// - Reset pressed
	bool									bClearPressed;				// - Clear pressed
																		//
	NWS_SVCINF_COMMON__UserControlDetails	Control;					// - User access UI control interface : Service Info

} NWS_SVCINF_COMMON__UserAccessDetails;

typedef struct	NWS_SVCINF_COMMON__ControlDetails {						// Networker Service Info - Main Control Structure
																		//
	NWS_SVCINF_COMMON__ServiceInfoDetails	Details;					// - Details of service info
	GENTIMDT__TimingCounter					AutoExitDelayTimingCounter;	// - Timer to time how long prompt is displayed
																		//
	NWS_SVCINF_COMMON__UserAccessDetails	UserAccess;					// - User access

} NWS_SVCINF_COMMON__ControlDetails;
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

#endif /* NWS_SVCINF_COMMON_H_ */
