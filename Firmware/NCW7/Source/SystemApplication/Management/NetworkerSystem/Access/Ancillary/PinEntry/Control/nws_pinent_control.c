//*********************************************************************************************************************************************************************************
// nws_pinent_control.c
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Release Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * Version:		N-C7 	   - v090
 * Date:		01/10/2020
 * Modifier:	Emilio R. La Greca
 *
 * Original release.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include		<com_defn.h>
#include		<nws_cfgmnu_common.h>
#include		<nws_pinent_control.h>
#include		<genstrdt.h>
#include		<nwsutils.h>

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
#include		<nws_pinent_userif_NC7.h>
#include		<tftdspif.h>
#endif

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static NWS_PINENT_COMMON__ControlDetails	mPinEntry;
static char									mstrPINEntered[NWS_PINENT_COMMON__MAX_PIN_LENGTH+1];
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
 * @brief:	Initialises data ready for processing once this state is entered.
 * @param:	pPinEntryAppInterface - Pointer to application specific details required for this interface.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PINENT_CONTROL__InitBeforeStateEntry(NWS_PINENT_COMMON__PinEntryDetails* pPinEntryAppInterface)
{
	// App related info define
	mPinEntry.pAppInterface = pPinEntryAppInterface;
	// User access init
	mPinEntry.UserAccess.bInvalidPin = false;
	// Define string detail
	GENSTRDT__InitString( NWS_PINENT_COMMON__MAX_PIN_LENGTH, (char*)&mstrPINEntered[0], &mPinEntry.UserAccess.NumberEntered );

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_PINENT_USERIF_NC7__InitBeforeStateEntry( &mPinEntry );
#endif

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Pin Number Entry state.
 * @param:	vbStateJustEntered    - Indicates if this state has just been entered (first time in this fcn).
 * 			pPinEntryAppInterface - Pointer to application specific details required for this interface.
 * 			pPinAccess			  - PIN access state.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PINENT_CONTROL__UserifUpdate(bool vbStateJustEntered, NWS_PINENT_COMMON__PinEntryDetails* pPinEntryAppInterface, NWS_PINENT_CONTROL__PIN_ACCESS* pPinAccess)
{
	uint8_t		ucIndex;
	//
	// Init any data when state just entered
	if ( vbStateJustEntered ) { NWS_PINENT_CONTROL__InitBeforeStateEntry( pPinEntryAppInterface ); }
	//
	// Clear any button press
	mPinEntry.UserAccess.Control.ButtonPressed = NWS_PINENT_COMMON__CB__NONE_PRESSED;
	//
	// User inteface update
#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_PINENT_USERIF_NC7__UpdateDisplay( &mPinEntry );
	NWS_PINENT_USERIF_NC7__UpdateControl( &mPinEntry );
#endif
	//
	// Process any control actions
	switch ( mPinEntry.UserAccess.Control.ButtonPressed )
	{
		case NWS_PINENT_COMMON__CB__OK:
			// Check the pin number entered
			if ( mPinEntry.UserAccess.NumberEntered.uiLength != NWS_PINENT_COMMON__MAX_PIN_LENGTH )
			{
				mPinEntry.UserAccess.bInvalidPin = true;
			}
			else
			{
				for ( ucIndex = 0; ucIndex < NWS_PINENT_COMMON__MAX_PIN_LENGTH; ucIndex++ )
				{
					if ( mPinEntry.UserAccess.NumberEntered.pstrText[ucIndex] != mPinEntry.pAppInterface->aucNumberToCompareWith[ucIndex] )
					{
						mPinEntry.UserAccess.bInvalidPin = true;
					}
				}
			}

			if ( !mPinEntry.UserAccess.bInvalidPin )
			{
				*pPinAccess = NWS_PINENT_CONTROL__PA__GRANTED;
			}
			break;

		case NWS_PINENT_COMMON__CB__CANCEL:
			// Cancel pin number entry
			*pPinAccess = NWS_PINENT_CONTROL__PA__CANCELLED;
			break;

		default:
			if ( TFTDSPIF__bNoTouchTimeout() ) { *pPinAccess = NWS_PINENT_CONTROL__PA__CANCELLED; }
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Pin Number Entry state (Get the pin number entered - No checking).
 * @param:	vbStateJustEntered    - Indicates if this state has just been entered (first time in this fcn).
 * 			pPinEntryAppInterface - Pointer to application specific details required for this interface.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PINENT_CONTROL__UserifUpdate_GetPinNumber(bool vbStateJustEntered, NWS_PINENT_COMMON__PinEntryDetails* pPinEntryAppInterface)
{
	// Init any data when state just entered
	if ( vbStateJustEntered ) { NWS_PINENT_CONTROL__InitBeforeStateEntry(pPinEntryAppInterface); }
	//
	// Clear any button press
	mPinEntry.UserAccess.Control.ButtonPressed = NWS_PINENT_COMMON__CB__NONE_PRESSED;
	//
	// User inteface update
#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_PINENT_USERIF_NC7__UpdateDisplay( &mPinEntry );
	NWS_PINENT_USERIF_NC7__UpdateControl( &mPinEntry );
#endif
	//
	// Process any control actions
	switch ( mPinEntry.UserAccess.Control.ButtonPressed )
	{
		case NWS_PINENT_COMMON__CB__OK:
			// Nothing to do here - Calling code checks - NWS_PINENT_CONTROL__bButtonPressed_Ok()
			break;

		case NWS_PINENT_COMMON__CB__CANCEL:
			// Nothing to do here - Calling code checks - NWS_PINENT_CONTROL__bButtonPressed_Cancel()
			break;

		default:
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns a pointer to the string structure holding the pin number entered - Note that the numbers are in ascii format.
 * @param:	None.
 * @retval:	GENSTRDT__String*.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
GENSTRDT__String*				NWS_PINENT_CONTROL__pPinNumberEntered(void)
{
	return &mPinEntry.UserAccess.NumberEntered;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when "Ok" button has been pressed - Call straight after NWS_PINENT_CONTROL__UserifUpdate_GetPinNumber().
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							NWS_PINENT_CONTROL__bButtonPressed_Ok(void)
{
	return (bool)( mPinEntry.UserAccess.Control.ButtonPressed == NWS_PINENT_COMMON__CB__OK );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when "Back" button has been pressed - Call straight after NWS_PINENT_CONTROL__UserifUpdate_GetPinNumber().
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							NWS_PINENT_CONTROL__bButtonPressed_Cancel(void)
{
	return (bool)( mPinEntry.UserAccess.Control.ButtonPressed == NWS_PINENT_COMMON__CB__CANCEL );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags when PIN invalid to display corresponding string.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_PINENT_CONTROL__FlagPinInvalid(void)
{
	mPinEntry.UserAccess.bInvalidPin = true;
	return;
}
//*********************************************************************************************************************************************************************************
