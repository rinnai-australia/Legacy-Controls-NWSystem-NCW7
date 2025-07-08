//*********************************************************************************************************************************************************************************
// nws_csetup_common.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Ancillary State : Custom Setup Menu Common Data Header.
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

#ifndef 		NWS_CSETUP_COMMON_H_
#define 		NWS_CSETUP_COMMON_H_

#include		<com_defn.h>
#include		<nws_prmaxs_common.h>
#include		<twbcomif.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_CSETUP_COMMON__CONTROL_BUTTONS {					// Networker System Access Custom Setup Buttons for UI
																		//
	NWS_CSETUP_COMMON__CB__NONE_PRESSED  			 =  0,				// - No button
	NWS_CSETUP_COMMON__CB__NETWORKER_PARAMS			 =  1,				// - Networker installer parameters
	NWS_CSETUP_COMMON__CB__COOLER_PARAMS			 =  2,				// - Cooler appliance installer parameters
	NWS_CSETUP_COMMON__CB__HEATER_PARAMS			 =  3,				// - Heater appliance installer parameters
	NWS_CSETUP_COMMON__CB__UNIT_SELECTED			 =  4,				// - Unit selected button
	NWS_CSETUP_COMMON__CB__RESTORE_DEFAULTS			 =  5,				// - Restore defaults
	NWS_CSETUP_COMMON__CB__BACK						 =  6			    // - Back button (Back to configuration menu)

} NWS_CSETUP_COMMON__CONTROL_BUTTONS;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	NWS_CSETUP_COMMON__UserControlDetails {					// Networker System Access Custom Setup - User Control Specific (UI)
																		//
	NWS_CSETUP_COMMON__CONTROL_BUTTONS		ButtonPressed;				// - Control button pressed

} NWS_CSETUP_COMMON__UserControlDetails;

typedef struct	NWS_CSETUP_COMMON__UserAccessDetails {					// Networker System Access Custom Setup - User Access Control
																		//
	bool									bRestoringDefaults;			// - Restoring defaults flag
	bool									bSelectUnit;				// - Unit to access to be selected
	uint8_t									ucUnitIDSelected;			// - UnitID selected from list
	NWS_CSETUP_COMMON__UserControlDetails	Control;					// - User access UI control interface : Custom Setup
	NWS_PRMAXS_COMMON__ACCESS_PARAM_TYPE	ParameterType;				// - Type of access to this screen

} NWS_CSETUP_COMMON__UserAccessDetails;

typedef struct	NWS_CSETUP_COMMON__ControlDetails {						// Networker System Access Custom Setup - Main Control Structure
																		//
	NWS_PRMAXS_COMMON__DEVICE_TYPE			DeviceTypeSelected;			// - Device type selected
	NWS_PRMAXS_COMMON__02_DEVICE_TYPE		DeviceType02Selected;		// - Type02 device selected
	TWBCOMIF__TWBDeviceAddress				DeviceAddress;				// - Address of device selected
	NWS_CSETUP_COMMON__UserAccessDetails	UserAccess;					// - User access

} NWS_CSETUP_COMMON__ControlDetails;
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

#endif /* NWS_CSETUP_COMMON_H_ */
