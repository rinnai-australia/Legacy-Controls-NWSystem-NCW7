//*********************************************************************************************************************************************************************************
// nws_zonenm_common.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Ancillary State : Zone Names Common Data Header.
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

#ifndef 		NWS_ZONENM_COMMON_H_
#define 		NWS_ZONENM_COMMON_H_

#include		<com_defn.h>
#include		<twbifutl.h>
#include		<genstrdt.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_ZONENM_COMMON__CONTROL_BUTTONS {					// Networker System Access Zone Names Buttons for UI
																		//
	NWS_ZONENM_COMMON__CB__NONE_PRESSED  			 = 0,				// - No button
	NWS_ZONENM_COMMON__CB__ZONE_A_SELECTED			 = 1,				// - Zone A selected for entry
	NWS_ZONENM_COMMON__CB__ZONE_B_SELECTED			 = 2,				// - Zone B selected for entry
	NWS_ZONENM_COMMON__CB__ZONE_C_SELECTED			 = 3,				// - Zone C selected for entry
	NWS_ZONENM_COMMON__CB__ZONE_D_SELECTED			 = 4,				// - Zone D selected for entry
	NWS_ZONENM_COMMON__CB__ZONE_NAME_DONE			 = 5,				// - Zone name entry done
	NWS_ZONENM_COMMON__CB__ZONE_NAME_CANCEL			 = 6,				// - Zone name entry cancelled
	NWS_ZONENM_COMMON__CB__BACK						 = 7,				// - Back button (Back to user preferences menu)

} NWS_ZONENM_COMMON__CONTROL_BUTTONS;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	NWS_ZONENM_COMMON__UserControlDetails {					// Networker System Access Zone Names - User Control Specific (UI)
																		//
	NWS_ZONENM_COMMON__CONTROL_BUTTONS		ButtonPressed;				// - Control button pressed

} NWS_ZONENM_COMMON__UserControlDetails;

typedef struct	NWS_ZONENM_COMMON__UserZoneInformation {				// Networker System Access Zone Names - User Zone Information (UI)
																		//
	bool									bInstalled;					// - Installed status
	GENSTRDT__String*						pDescription;				// - Description

} NWS_ZONENM_COMMON__UserZoneInformation;

typedef struct	NWS_ZONENM_COMMON__UserAccessDetails {					// Networker System Access Zone Names - User Access Control
																		//
	NWS_ZONENM_COMMON__UserZoneInformation	ZoneA;						// - ZoneA info
	NWS_ZONENM_COMMON__UserZoneInformation	ZoneB;						// - ZoneB info
	NWS_ZONENM_COMMON__UserZoneInformation	ZoneC;						// - ZoneC info
	NWS_ZONENM_COMMON__UserZoneInformation	ZoneD;						// - ZoneD info
																		//
	NWS_ZONENM_COMMON__UserControlDetails	Control;					// - User access UI control interface : Installer Setup
																		//
	bool									bZoneWasBeingSelected;		// - Keep track of change to entry
	bool									bZoneNameBeingEntered;		// - Flags when keyboard up to enter zone name
	TWBIFUTL__ZONE							ZoneBeingUpdated;			// - The zone selected to have its name updated
	GENSTRDT__String						NewZoneName;				// - Copy of new zone name

} NWS_ZONENM_COMMON__UserAccessDetails;

typedef struct	NWS_ZONENM_COMMON__ControlDetails {						// Networker System Access Zone Names - Main Control Structure
																		//
	NWS_ZONENM_COMMON__UserAccessDetails	UserAccess;					// - User access

} NWS_ZONENM_COMMON__ControlDetails;
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

#endif /* NWS_ZONENM_COMMON_H_ */
