//*********************************************************************************************************************************************************************************
// uitiming.c
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

#include		<com_defn.h>
#include		<gentimdt.h>
#include		<nws_pinent_userif_NC7.h>
#include		<nws_csetup_userif_NC7.h>
#include		<evpcool_userif_NC7.h>
#include		<zoning_userif_NC7.h>
#include		<uikeybrd.h>
#include		<uitftdsp.h>
#include		<FT800_touchifc.h>

#include		<nws_userif_NC7.h>
#include		<mcuaxess.h>
#include		<uicommon.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Structure
typedef struct  UITimingDetails {							// This structure provides a pointer so a table of timing pointers can be created
															//
	const GENTIMDT__Timing* 	pData;						// Used to point to module level timing variable

} UITimingDetails;
//
// Array of timer pointers giving access to module level timers to be updated
static const struct	UITimingDetails		mkaUITiming[] = {

	{ &EVPCOOL_USERIF_NC7__kWaterDropDelayIconDisplayTiming						},
	{ &ZONING_USERIF_NC7__kZoningUIZoneAOnButtonFlashShowTiming					},
	{ &ZONING_USERIF_NC7__kZoningUIZoneBOnButtonFlashShowTiming					},
	{ &ZONING_USERIF_NC7__kZoningUIZoneCOnButtonFlashShowTiming					},
	{ &ZONING_USERIF_NC7__kZoningUIZoneDOnButtonFlashShowTiming					},

	{ &ZONING_USERIF_NC7__kZoningUICancelAccessTiming							},
	{ &UIKEYBRD__kAlphaKeyboardFlashCursorTiming								},
	{ &NWS_PINENT_USERIF_NC7__kNumericKeyboardFlashCursorTiming					},
	{ &NWS_CSETUP_USERIF_NC7__kMessageDotTiming									},
	{ &UITFTDSP__kTFTControlBacklightOnTimeoutTiming							},

	{ &UITFTDSP__kTFTControlBacklightRampUpDownTiming							},
	{ &UITFTDSP__kTFTControlFlash1HzTiming										},
	{ &UITFTDSP__kTFTControlFlash2HzTiming										},
	{ &UITFTDSP__kTFTControlFlash4HzTiming										},

	{ &FT800_TOUCHIFC__kTouchTagDebounceTiming 									},
	{ &FT800_TOUCHIFC__kTouchTagRapidIntervalTiming								},
	{ &FT800_TOUCHIFC__kTouchCalWaitBeforeReadbackTiming						},
	{ &UITFTDSP_IMAGE__kFanBladeDelayIconDisplayTiming     						},
	{ &UITFTDSP_IMAGE__kActiveImagePersistenceTiming							},

	{ &UITFTDSP_BUTTON__kActiveButtonPersistenceTiming							},
	{ &NWS_ACCESS_USERIF_NC7__kNetworkerSystemInstallParamAccessExitTiming		},
	{ &NWS_ACCESS_USERIF_NC7__kNetworkerSystemServiceParamAccessExitTiming		},

	{ &UICOMMON__kModeSwitchDelayTiming											},
	{ COM_DEFN__NULL															}	// Last entry MUST be NULL as this used to define end of table...
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct  UITimingControlDetails	{					// UI Timing Control
															//
	uint8_t						ucUITickCount;				// - User Interface Update tick count
	bool						bUpdateUI;					// - Update UI
															//
} UITimingControlDetails;									//
															//
static UITimingControlDetails	mUITimingControl;			// Container
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


//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialisation.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITIMING__Initialise(void)
{
	mUITimingControl.ucUITickCount = GENTIMDT__kucUI_TIMEBASE_10MS;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update - Call every 10ms.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							UITIMING__UpdateTimers(void)
{
	GENTIMDT__TimingCounter* 	pTimingCounter;
	uint32_t					ulCountValue;
	uint8_t						ucIndex = 0;
	//
	// Called every 10ms
	if ( mUITimingControl.ucUITickCount != 0 ) { mUITimingControl.ucUITickCount--; }
	if ( mUITimingControl.ucUITickCount != 0 ) { return; }
	//
	// 20ms has elapsed
	// - Update UI based timers
	while ( mkaUITiming[ucIndex].pData != COM_DEFN__NULL )
	{
        // Get pointer to counter data
    	pTimingCounter = mkaUITiming[ucIndex].pData->pCounter;
        // If update allowed do it!
        if ( pTimingCounter->bAllowUpdate )
        {
        	ulCountValue = pTimingCounter->ulValue;

          	if ( ulCountValue > 0 )
          	{
          		ulCountValue--;
            }

          	pTimingCounter->ulValue = ulCountValue;
        }

        ucIndex++;
	}
    // - Flag UI to be updated
    mUITimingControl.bUpdateUI = true;
    // - Restart timer
    mUITimingControl.ucUITickCount = GENTIMDT__kucUI_TIMEBASE_10MS;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when UI can be updated.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							UITIMING__bUpdateUI(void)
{
	bool		bUpdateUI = mUITimingControl.bUpdateUI;

	mUITimingControl.bUpdateUI = false;

	return bUpdateUI;
}
//*********************************************************************************************************************************************************************************
