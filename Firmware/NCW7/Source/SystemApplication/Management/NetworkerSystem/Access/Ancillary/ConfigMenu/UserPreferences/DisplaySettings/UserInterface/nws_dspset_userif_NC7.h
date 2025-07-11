//*********************************************************************************************************************************************************************************
// nws_dspset_userif_NC7.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Display Settings User Interface - NC7 Specific.
 *
 * 				Networker system display setting access UI - N-C7 specific.
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

#ifndef 		NWS_DSPSET_USERIF_NC7_H_
#define		 	NWS_DSPSET_USERIF_NC7_H_

#include		<com_defn.h>
#include		<nws_dspset_common.h>

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
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void						NWS_DSPSET_USERIF_NC7__InitBeforeStateEntry(NWS_DSPSET_COMMON__ControlDetails* pDisplaySettingAccess);

extern void						NWS_DSPSET_USERIF_NC7__UpdateDisplay(NWS_DSPSET_COMMON__ControlDetails* pDisplaySettingAccess);
extern void						NWS_DSPSET_USERIF_NC7__UpdateControl(NWS_DSPSET_COMMON__ControlDetails* pDisplaySettingAccess);
//*********************************************************************************************************************************************************************************

#endif /* NWS_DSPSET_USERIF_NC7_H_ */
