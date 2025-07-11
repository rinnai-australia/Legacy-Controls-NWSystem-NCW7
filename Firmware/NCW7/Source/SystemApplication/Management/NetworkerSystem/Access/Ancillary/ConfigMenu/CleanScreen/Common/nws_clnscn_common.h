//*********************************************************************************************************************************************************************************
//
//	Module:			Networker System Ancillary State - Clean Screen Common Header.
//
//	File: 			nws_clnscn_common.h.
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Modifications List
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	Version:		N-C7 - v090
//  Date:			01/10/2020
//	Modifier:  		Emilio R. La Greca
//	Reason:         Release
//
//*********************************************************************************************************************************************************************************
//*********************************************************************************************************************************************************************************
// nws_clnscn_common.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Ancillary State - Clean Screen Common Header.
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

#ifndef 		NWS_CLNSCN_COMMON_H_
#define 		NWS_CLNSCN_COMMON_H_

#include		<com_defn.h>
#include		<gentimdt.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_CLNSCN_COMMON__CONTROL_BUTTONS {					// Networker System Access Clean Screen Control Buttons for UI
																		//
	NWS_CLNSCN_COMMON__CB__NONE_PRESSED  			 = 0				// - No button

} NWS_CLNSCN_COMMON__CONTROL_BUTTONS;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	NWS_CLNSCN_COMMON__UserControlDetails {					// Networker System Access Clean Screen Setting - User Control Specific (UI)
																		//
	NWS_CLNSCN_COMMON__CONTROL_BUTTONS		ButtonPressed;				// - Control button pressed

} NWS_CLNSCN_COMMON__UserControlDetails;

typedef struct	NWS_CLNSCN_COMMON__UserAccessDetails {					// Networker System Access Clean Screen Setting - User Access Control
																		//
	NWS_CLNSCN_COMMON__UserControlDetails	Control;					// - User access UI control interface : Config Menu

} NWS_CLNSCN_COMMON__UserAccessDetails;

typedef struct	NWS_CLNSCN_COMMON__ControlDetails {						// Networker System Access Clean Screen Setting - Main Control Structure
																		//
	GENTIMDT__TimingCounter					AutoExitDelayTimingCounter;	// - Timer to time how long prompt is displayed
	NWS_CLNSCN_COMMON__UserAccessDetails	UserAccess;					// - User access

} NWS_CLNSCN_COMMON__ControlDetails;
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

#endif /* NWS_CLNSCN_COMMON_H_ */
