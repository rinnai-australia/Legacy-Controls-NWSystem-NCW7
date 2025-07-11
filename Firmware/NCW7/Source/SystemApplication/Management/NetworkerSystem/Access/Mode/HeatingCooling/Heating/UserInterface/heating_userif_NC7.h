//*********************************************************************************************************************************************************************************
// cooling_userif_NC7.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System Heating Display/Control - N-C7 Specific.
 *
 * 				Networker heating UI.
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

#ifndef 		HEATING_USERIF_NC7_H_
#define 		HEATING_USERIF_NC7_H_

#include		<com_defn.h>
#include		<htgclg_common.h>

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
extern void							HEATING_USERIF_NC7__UpdateDisplay_Mode_Programming(HTGCLG_COMMON__ControlDetails* pHeating);
extern void							HEATING_USERIF_NC7__UpdateControl_Mode_Programming(HTGCLG_COMMON__ControlDetails* pHeating);

extern void							HEATING_USERIF_NC7__UpdateDisplay_Mode_Off(HTGCLG_COMMON__ControlDetails* pHeating);
extern void							HEATING_USERIF_NC7__UpdateControl_Mode_Off(HTGCLG_COMMON__ControlDetails* pHeating);

extern void							HEATING_USERIF_NC7__UpdateDisplay_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pHeating);
extern void							HEATING_USERIF_NC7__UpdateControl_Mode_FanOnly(HTGCLG_COMMON__ControlDetails* pHeating);

extern void							HEATING_USERIF_NC7__UpdateDisplay_Mode_On(HTGCLG_COMMON__ControlDetails* pHeating);
extern void							HEATING_USERIF_NC7__UpdateControl_Mode_On(HTGCLG_COMMON__ControlDetails* pHeating);
//*********************************************************************************************************************************************************************************

#endif /* HEATING_USERIF_NC7_H_ */
