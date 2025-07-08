//*********************************************************************************************************************************************************************************
// FT800_touchifc.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: FT800 Touch Interface.
 *
 * 				Provides access to TFT display touchscreen related functions.
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

#ifndef 		FT800_TOUCHIFC_H_
#define 		FT800_TOUCHIFC_H_

#include		<com_defn.h>
#include		<gensavul.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct FT800_TOUCHIFC__FT800TouchTagValue {			// Touch Tag Value Details
															//
	bool		bAllowForRapidUpdate;						// - Allow for rapid update when long press
	bool 		bWrapAroundEnabled;							// - Wrap around value
	bool 		bIncreasing;								// - Increasing value
	uint8_t 	ucMinimum;									// - Minimum value
	uint8_t 	ucMaximum;									// - Maximum value
	uint8_t 	ucActual;									// - Actual value

} FT800_TOUCHIFC__FT800TouchTagValue;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformA;
extern const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformB;
extern const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformC;
extern const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformD;
extern const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformE;
extern const struct	GENSAVUL__SavedULValue	FT800_TOUCHIFC__kFT800TouchCalTransformF;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern const struct	GENTIMDT__Timing		FT800_TOUCHIFC__kTouchTagDebounceTiming;
extern const struct	GENTIMDT__Timing		FT800_TOUCHIFC__kTouchTagRapidIntervalTiming;
extern const struct	GENTIMDT__Timing		FT800_TOUCHIFC__kTouchCalWaitBeforeReadbackTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	FUNCTION DECLARATIONS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void							FT800_TOUCHIFC__Init(uint16_t vuiDisplayHeight, uint16_t vuiDisplayWidth);

extern void							FT800_TOUCHIFC__RapidUpdateDataInit(void);
extern void							FT800_TOUCHIFC__RapidUpdateDataDefine(FT800_TOUCHIFC__FT800TouchTagValue* pTouchTagValue);

extern void							FT800_TOUCHIFC__Update(void);

extern uint8_t						FT800_TOUCHIFC__ucTagToProcess(void);
extern void							FT800_TOUCHIFC__FlagTagProcessed(void);
extern bool							FT800_TOUCHIFC__bTouchPressDetected(void);
extern void							FT800_TOUCHIFC__FlagTouchPressDetected(void);

extern void							FT800_TOUCHIFC__SetTagMask(bool vbAllow);
extern uint8_t						FT800_TOUCHIFC__ucTagMask(void);

extern void							FT800_TOUCHIFC__CalInit(void);
extern void							FT800_TOUCHIFC__CalRedo(void);
extern bool							FT800_TOUCHIFC__bCalDoneValidSaveValues(void);
extern bool							FT800_TOUCHIFC__bCalDoneFatal(void);
extern bool							FT800_TOUCHIFC__bCalDoneValid(void);

extern void							FT800_TOUCHIFC__ResetTransformValues(void);

extern uint32_t						FT800_TOUCHIFC__ulTransformAValue(void);
extern uint32_t						FT800_TOUCHIFC__ulTransformBValue(void);
extern uint32_t						FT800_TOUCHIFC__ulTransformCValue(void);
extern uint32_t						FT800_TOUCHIFC__ulTransformDValue(void);
extern uint32_t						FT800_TOUCHIFC__ulTransformEValue(void);
extern uint32_t						FT800_TOUCHIFC__ulTransformFValue(void);
//*********************************************************************************************************************************************************************************

#endif 	/* FT800_TOUCHIF_H_ */
