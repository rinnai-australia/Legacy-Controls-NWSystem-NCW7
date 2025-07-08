//*********************************************************************************************************************************************************************************
// tftdspif.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: TFT Display Interface.
 *
 * 				Provides access to TFT display and driver modules for initialisation and control of the TFT display via FT800 device.
 *
 *				The following function calls are mandatory:
 *
 *					1) TFTDSPIF__Initialise() - To be called after reset.
 *					2) TFTDSPIF__Update() 	 - To be updated in main loop.
 *					3) TFTDSPIF__Update01ms() - To be updated in 1ms timebase.
 *
 *				Device initialisation TFT/Driver is done using the following functions:
 *
 *					1) TFTDSPIF__StartInitialisation()   - Begin initialisation.
 *					2) TFTDSPIF__bDisplayIsReady() 		- Check to determine when ready..
 *
 *				Touch interface wrapper functions are available.  Process touch updates using the function:
 *
 *					- TFTDSPIF__TouchUpdate().
 *
 *				Once a tag has been detected and processed it must be released using the function:
 *
 *					- TFTDSPIF__TouchTagFlagProcessed().
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Release Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * Version:		N-C7  	   - v090
 * Date:		01/10/2020
 * Modifier:  	Emilio R. La Greca
 *
 * Original release.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef 		TFTDSPIF_H_
#define 		TFTDSPIF_H_

#include		<com_defn.h>
#include		<FT800_touchifc.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Display Type
#define			TFTDSPIF__TFT_DISPLAY_TYPE	GL050TN33
//
// Display size
#if (TFTDSPIF__TFT_DISPLAY_TYPE == GL050TN33)

#include		"GL050TN33_dispif.h"
#define			TFTDSPIF__DISPLAY_H		GL050TN33_DISPIF__DISPLAY_H		// Define display height
#define			TFTDSPIF__DISPLAY_W		GL050TN33_DISPIF__DISPLAY_W		// Define display width

#endif
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
extern const struct	GENTIMDT__Timing	TFTDSPIF__kTFTDisplayNoTouchTimeoutTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void							TFTDSPIF__Initialise(void);

extern bool							TFTDSPIF__bConfiguredForOperation(bool vbUtilityAccessToUI);
extern bool							TFTDSPIF__bInitialised(void);

extern void							TFTDSPIF__Update01ms(void);
extern void							TFTDSPIF__Update(void);

extern void							TFTDSPIF__StartInitialisation(void);
extern bool							TFTDSPIF__bDisplayIsReady(void);

extern bool							TFTDSPIF__bNoTouchTimeout(void);
extern void							TFTDSPIF__TouchUpdate(void);

extern void							TFTDSPIF__RapidUpdateDataInit(void);
extern void							TFTDSPIF__RapidUpdateDataDefine(FT800_TOUCHIFC__FT800TouchTagValue* pTouchTagValue);

extern uint8_t						TFTDSPIF__ucTouchTagValue(void);
extern void							TFTDSPIF__TouchTagFlagProcessed(void);

extern bool							TFTDSPIF__bTouchPressDetected(void);
extern void							TFTDSPIF__FlagTouchPressDetected(void);

extern void							TFTDSPIF__SetTouchTagMask(bool vbAllow);
extern uint8_t						TFTDSPIF__ucTouchTagMask(void);

extern void							TFTDSPIF__TouchCalInit(void);
extern void							TFTDSPIF__TouchCalRedo(void);
extern bool							TFTDSPIF__bTouchCalDoneValidSaveValues(void);
extern bool							TFTDSPIF__bTouchCalDoneFatal(void);
extern bool							TFTDSPIF__bTouchCalDoneValid(void);

extern FT800_DEVICEIF__XFER_STATUS	TFTDSPIF__SwitchBacklight(FUNC_ACTION vAction, bool vbLoPowerMode, uint8_t vucLevel);

extern void							TFTDSPIF__ResetTransformValues(void);
extern uint32_t						TFTDSPIF__ulTransformAValue(void);
extern uint32_t						TFTDSPIF__ulTransformBValue(void);
extern uint32_t						TFTDSPIF__ulTransformCValue(void);
extern uint32_t						TFTDSPIF__ulTransformDValue(void);
extern uint32_t						TFTDSPIF__ulTransformEValue(void);
extern uint32_t						TFTDSPIF__ulTransformFValue(void);
//*********************************************************************************************************************************************************************************

#endif 	/* TFTDSPIF_H_ */
