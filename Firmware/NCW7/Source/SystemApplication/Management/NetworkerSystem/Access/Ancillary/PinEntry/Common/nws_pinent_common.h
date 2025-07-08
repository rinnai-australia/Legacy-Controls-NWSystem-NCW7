//*********************************************************************************************************************************************************************************
// nws_pinent_common.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Ancillary State - Pin Entry Common Header.
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

#ifndef 		NWS_PINENT_COMMON_H_
#define 		NWS_PINENT_COMMON_H_

#include		<com_defn.h>
#include		<genstrdt.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_PINENT_COMMON__CONTROL_BUTTONS {					// Networker System Access Pin Number Entry Buttons for UI
																		//
	NWS_PINENT_COMMON__CB__NONE_PRESSED	 = 0,							// - No button
	NWS_PINENT_COMMON__CB__OK			 = 1,							// - Valid pin entered
	NWS_PINENT_COMMON__CB__CANCEL		 = 2							// - Cancel setting pin

} NWS_PINENT_COMMON__CONTROL_BUTTONS;

#define			NWS_PINENT_COMMON__MAX_PIN_LENGTH		4				// Max numbers in pin
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	NWS_PINENT_COMMON__PinEntryDetails {					// Networker System Access Pin Number Entry - App Interface
																		//
	char	aucNumberToCompareWith[NWS_PINENT_COMMON__MAX_PIN_LENGTH];	// - Pin number to compare against
	char*	pstrTopStatusText;											// - Text to display in top status line
	char*	pstrPinDescription;											// - Description of pin

} NWS_PINENT_COMMON__PinEntryDetails;

typedef struct	NWS_PINENT_COMMON__UserControlDetails {					// Networker System Access Pin Number Entry - User Control Specific (UI)
																		//
	NWS_PINENT_COMMON__CONTROL_BUTTONS		ButtonPressed;				// - Control button pressed

} NWS_PINENT_COMMON__UserControlDetails;

typedef struct	NWS_PINENT_COMMON__UserAccessDetails {					// Networker System Access Pin Number Entry - User Access Control
																		//
	NWS_PINENT_COMMON__UserControlDetails	Control;					// - User access UI control interface : Pin Entry
	GENSTRDT__String						NumberEntered;				// - Number entered string
	bool									bInvalidPin;				// - Invalid pin flag

} NWS_PINENT_COMMON__UserAccessDetails;

typedef struct	NWS_PINENT_COMMON__ControlDetails {						// Networker System Access Pin Number Entry - Main Control Structure
																		//
	NWS_PINENT_COMMON__UserAccessDetails	UserAccess;					// - User access
	NWS_PINENT_COMMON__PinEntryDetails*		pAppInterface;				// - Application interface data

} NWS_PINENT_COMMON__ControlDetails;
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

#endif /* NWS_PINENT_COMMON_H_ */
