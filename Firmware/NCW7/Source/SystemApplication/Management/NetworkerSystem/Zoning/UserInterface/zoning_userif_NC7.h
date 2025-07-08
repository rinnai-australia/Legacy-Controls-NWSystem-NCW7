//*********************************************************************************************************************************************************************************
// zoning_userif_NC7.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Networker System - Zoning Control (UI - N-C7).
 *
 * 				This module provides the code required for zoning based UI.
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

#ifndef 		ZONING_USERIF_NC7_H_
#define 		ZONING_USERIF_NC7_H_

#include		<com_defn.h>
#include		<zoning_common.h>
#include		<genstrdt.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			ZONING_USERIF_NC7__ZONE_DESCRIPTION_MAX_LENGTH		16
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
extern const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneADescription;
extern const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneBDescription;
extern const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneCDescription;
extern const struct	GENSTRDT__StringData	ZONING_USERIF_NC7__kZoneDDescription;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern const struct	GENTIMDT__Timing		ZONING_USERIF_NC7__kZoningUIZoneAOnButtonFlashShowTiming;
extern const struct	GENTIMDT__Timing		ZONING_USERIF_NC7__kZoningUIZoneBOnButtonFlashShowTiming;
extern const struct	GENTIMDT__Timing		ZONING_USERIF_NC7__kZoningUIZoneCOnButtonFlashShowTiming;
extern const struct	GENTIMDT__Timing		ZONING_USERIF_NC7__kZoningUIZoneDOnButtonFlashShowTiming;
extern const struct	GENTIMDT__Timing		ZONING_USERIF_NC7__kZoningUICancelAccessTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void					ZONING_USERIF_NC7__Initialise(ZONING_COMMON__ControlDetails* pZoning);

extern GENSTRDT__String*	ZONING_USERIF_NC7__pZoneDescription(TWBIFUTL__ZONE vZone);
extern void					ZONING_USERIF_NC7__FlagZoneNameAltered(TWBIFUTL__ZONE vZone);
extern bool					ZONING_USERIF_NC7__bZoneNameAltered(TWBIFUTL__ZONE vZone);

extern void					ZONING_USERIF_NC7__InitialiseBeforeFirstAccess(void);

extern void					ZONING_USERIF_NC7__UpdateDisplay_ProgrammingZones(ZONING_COMMON__ControlDetails* pZoning);
extern void					ZONING_USERIF_NC7__UpdateControl_ProgrammingZones(ZONING_COMMON__ControlDetails* pZoning);

extern void					ZONING_USERIF_NC7__UpdateDisplay_InstalledAndEnabledZones(ZONING_COMMON__ControlDetails* pZoning);
extern void					ZONING_USERIF_NC7__UpdateControl_InstalledAndEnabledZones(ZONING_COMMON__ControlDetails* pZoning);
//*********************************************************************************************************************************************************************************

#endif 	/* ZONING_USERIF_NC7_H_ */
