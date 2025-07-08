//*********************************************************************************************************************************************************************************
// nws_userif_NC7.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Access User Interface - N-C7 Specific.
 *
 * 				Networker system access UI - N-C7 specific.
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

#ifndef 		NWS_USERIF_NC7_H_
#define 		NWS_USERIF_NC7_H_

#include		<com_defn.h>
#include		<nws_access_common.h>
#include		<twbifutl.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
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
extern const struct	GENTIMDT__Timing		NWS_ACCESS_USERIF_NC7__kNetworkerSystemInstallParamAccessExitTiming;
extern const struct	GENTIMDT__Timing		NWS_ACCESS_USERIF_NC7__kNetworkerSystemServiceParamAccessExitTiming;
extern const struct	GENTIMDT__Timing		NWS_ACCESS_USERIF_NC7__kNetworkerSystemDisplayWaitAfterInitTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void								NWS_ACCESS_USERIF_NC7__ConfigMenu_AccessInit(void);
extern void								NWS_ACCESS_USERIF_NC7__ConfigMenu_AccessUpdate(TWBIFUTL__NWSYS_OPNSTATE vOperationalState);
extern TWBIFUTL__OPERATING_STATE_NC7	NWS_ACCESS_USERIF_NC7__CurrentConfigMenuAccessState(void);

extern void								NWS_ACCESS_USERIF_NC7__UpdateDisplay_SlaveNotRegistered(void);
extern void								NWS_ACCESS_USERIF_NC7__UpdateControl_SlaveNotRegistered(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem);

extern bool								NWS_ACCESS_USERIF_NC7__bUtilityAccessToUI(void);
extern bool								NWS_ACCESS_USERIF_NC7__bScreenCaptureInProgress(void);
extern void								NWS_ACCESS_USERIF_NC7__UtilityUpdate(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem);

extern void								NWS_ACCESS_USERIF_NC7__ActionBeforeUpdate(uint32_t vulBackgroundColor, NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem);
extern void								NWS_ACCESS_USERIF_NC7__ActionAfterUpdate(void);

extern void								NWS_ACCESS_USERIF_NC7__UpdateDisplayControl_Common(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem);

extern void								NWS_ACCESS_USERIF_NC7__IndicateUserAccessCompleted(void);

extern void								NWS_ACCESS_USERIF_NC7__Mode_NoAppliance_UpdateDisplay(void);
extern void								NWS_ACCESS_USERIF_NC7__Mode_NoAppliance_UpdateControl(NWS_ACCESS_COMMON__ControlDetails* pNetworkerSystem);
//*********************************************************************************************************************************************************************************

#endif /* NWS_USERIF_NC7_H_ */
