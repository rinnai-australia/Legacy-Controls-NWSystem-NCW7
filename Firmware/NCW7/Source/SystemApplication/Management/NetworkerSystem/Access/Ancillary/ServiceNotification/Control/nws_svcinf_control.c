//*********************************************************************************************************************************************************************************
// nws_svcinf_control.c
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
#include		<nws_svcinf_common.h>
#include		<gentimdt.h>

#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
#include		<nws_svcinf_userif_NC7.h>
#endif

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define		NWS_SVCINF_CONTROL__AUTO_EXIT_DELAY_INTERVAL_1S		60
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static NWS_SVCINF_COMMON__ControlDetails	mServiceInfo;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	NWS_SVCINF_CONTROL__kServiceInfoAutoExitDelayTiming = {	&mServiceInfo.AutoExitDelayTimingCounter };
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
 * @brief:	Init service info details.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SVCINF_CONTROL__ServiceInfoPrompt_Init(void)
{
	mServiceInfo.Details.MessageType = NWS_SVCINF_COMMON__MT__NONE;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Update service info details.
 * @param:	pDetails - Pointer to service info details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SVCINF_CONTROL__ServiceInfoPrompt_FlagToShowIt(NWS_SVCINF_COMMON__ServiceInfoDetails* pDetails)
{
	// Define details
	mServiceInfo.Details.MessageType = pDetails->MessageType;
	mServiceInfo.Details.GroupID     = pDetails->GroupID;
	mServiceInfo.Details.UnitID      = pDetails->UnitID;
	mServiceInfo.Details.ucFaultCode = pDetails->ucFaultCode;
	mServiceInfo.Details.ucMaxUnits  = pDetails->ucMaxUnits;
	// Start timeout timer
	GENTIMDT__StartTimer_TB_1SEC( NWS_SVCINF_CONTROL__AUTO_EXIT_DELAY_INTERVAL_1S, &mServiceInfo.AutoExitDelayTimingCounter );
	// Clear flags
	mServiceInfo.UserAccess.bResetPressed = false;
	mServiceInfo.UserAccess.bClearPressed = false;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Service info details to be displayed?
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							NWS_SVCINF_CONTROL__ServiceInfoPrompt_bShowIt(void)
{
	return (bool)( mServiceInfo.Details.MessageType != NWS_SVCINF_COMMON__MT__NONE );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates if reset pressed by user.
 * @param:	None.
 * @retval: bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							NWS_SVCINF_CONTROL__ServiceInfoPrompt_bResetFlagged(void)
{
	bool		bResetPressed = mServiceInfo.UserAccess.bResetPressed;

	mServiceInfo.UserAccess.bResetPressed = false;

	return bResetPressed;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates if clear pressed by user.
 * @param:	None.
 * @retval: bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							NWS_SVCINF_CONTROL__ServiceInfoPrompt_bClearFlagged(void)
{
	bool		bClearPressed = mServiceInfo.UserAccess.bClearPressed;

	mServiceInfo.UserAccess.bClearPressed = false;

	return bClearPressed;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	User interface update - Config menu state.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_SVCINF_CONTROL__UserifUpdate(void)
{
	// Clear any button press
	mServiceInfo.UserAccess.Control.ButtonPressed = NWS_SVCINF_COMMON__CB__NONE_PRESSED;
	//
	// User inteface update
#if (BUILD_INFO__NCX_DEVICE == COM_DEFN__NCX_DEVICE__NC7)
	NWS_SVCINF_USERIF_NC7__UpdateDisplay( &mServiceInfo );
	NWS_SVCINF_USERIF_NC7__UpdateControl( &mServiceInfo );
#endif
	//
	// Process any control actions
	switch ( mServiceInfo.UserAccess.Control.ButtonPressed )
	{
		case NWS_SVCINF_COMMON__CB__BACK:
		case NWS_SVCINF_COMMON__CB__OK:
			// Just get out
			mServiceInfo.Details.MessageType = NWS_SVCINF_COMMON__MT__NONE;
			break;

		case NWS_SVCINF_COMMON__CB__RESET:
			// Flag reset to occur the get out
			mServiceInfo.UserAccess.bResetPressed = true; mServiceInfo.Details.MessageType = NWS_SVCINF_COMMON__MT__NONE;
			break;

		case NWS_SVCINF_COMMON__CB__CLEAR:
			// Flag clear to occur the get out
			mServiceInfo.UserAccess.bClearPressed = true; mServiceInfo.Details.MessageType = NWS_SVCINF_COMMON__MT__NONE;
			break;

		default:
			break;
	}

	if ( mServiceInfo.AutoExitDelayTimingCounter.ulValue == 0 ) { mServiceInfo.Details.MessageType = NWS_SVCINF_COMMON__MT__NONE; }

	return;
}
//*********************************************************************************************************************************************************************************
