//*********************************************************************************************************************************************************************************
// tftdspif.c
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
#include		<tftdspif.h>
#include		<FT800_touchifc.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<gentimdt.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT BL max level for lo power operation
#define			TFTDSPIF__MAX_BL_LEVEL_LOPWR_MODE		16
// TFT BL max level for hi power operation
#define			TFTDSPIF__MAX_BL_LEVEL_HIPWR_MODE		128
// Timeout when no user input
#define			TFTDSPIF__NO_INPUT_IMEOUT_TIME_1S		60
//
// TFT Display State Control
typedef enum	TFTDSPIF__STATE {

	TFTDSPIF__STATE_IDLE 		 = 0,
	TFTDSPIF__STATE_DRIVER_INIT  = 1,
	TFTDSPIF__STATE_DISPLAY_INIT = 2,
	TFTDSPIF__STATE_READY  		 = 3

} TFTDSPIF__STATE;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	TFTDSPIF__TFTDisplay {							// TFT Display
																//
	TFTDSPIF__STATE				State;							// - Main state
																//
	uint16_t					uiHeight;						// - Height
	uint16_t					uiWidth;						// - Width
																//
	GENTIMDT__TimingCounter		NoTouchTimeoutTimingCounter;	// - No touch timeout to exit config menu
	bool						bInitialised;					// - Initialised flag, emilio 07-10-16
																//
} TFTDSPIF__TFTDisplay;											//
																//
static TFTDSPIF__TFTDisplay		mTFTDisplay;					// Container
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const struct	GENTIMDT__Timing	TFTDSPIF__kTFTDisplayNoTouchTimeoutTiming = { &mTFTDisplay.NoTouchTimeoutTimingCounter };
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void						DisplayState_Idle(void);
static void						DisplayState_DriverInit(void);
static void						DisplayState_DisplayInit(void);
static void						DisplayState_Ready(void);
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
 * @brief:	Initialises the TFT display.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__Initialise(void)
{
	mTFTDisplay.State 		 = TFTDSPIF__STATE_IDLE;
	mTFTDisplay.bInitialised = false;

	FT800_TOUCHIFC__Init( TFTDSPIF__DISPLAY_H, TFTDSPIF__DISPLAY_W );
	FT800_COPRCCMD__Init();
	FT800_DEVICEIF__Init();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display - Config refresh.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bConfiguredForOperation(bool vbUtilityAccessToUI)
{
	bool		bConfigured = true;

	if ( vbUtilityAccessToUI )
	{
		// DO NOT ACCESS FT800...
	}
	else
	if ( ( bConfigured = FT800_DEVICEIF__bDeviceIsReady() ) == true )
	{
		GL050TN33_DISPIF__ConfigRefresh();
	}

	return bConfigured;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display - Indicates if initialisation has been completed.
 * @param:	None.
 * @retval:	bool = true is a one-shot.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bInitialised(void)
{
	bool 	bInitialised = mTFTDisplay.bInitialised;

	mTFTDisplay.bInitialised = false;

	return bInitialised;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display update - 1ms processes.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__Update01ms(void)
{
	FT800_DEVICEIF__Update01ms();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display update - Main loop processes.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__Update(void)
{
	switch ( mTFTDisplay.State )
	{
		case TFTDSPIF__STATE_IDLE:			DisplayState_Idle();		break;
		case TFTDSPIF__STATE_DRIVER_INIT:	DisplayState_DriverInit();	break;
		case TFTDSPIF__STATE_DISPLAY_INIT:	DisplayState_DisplayInit();	break;
		case TFTDSPIF__STATE_READY:			DisplayState_Ready();		break;

		default:
			// TODO : Invalid State - Force Reset??
			break;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to invoke initialisation procedure.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__StartInitialisation(void)
{
	mTFTDisplay.State        = TFTDSPIF__STATE_DRIVER_INIT;
	mTFTDisplay.bInitialised = false;

	FT800_DEVICEIF__FT800DeviceInit_Config();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when TFT display interface is ready for use (Initialisation procedure completed).
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bDisplayIsReady(void)
{
	return (bool)( mTFTDisplay.State == TFTDSPIF__STATE_READY );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display no touch timer check - If zero return true.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bNoTouchTimeout(void)
{
	return (bool)( mTFTDisplay.NoTouchTimeoutTimingCounter.ulValue == 0 );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display touch update - Main loop processes.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__TouchUpdate(void)
{
	// Start timer if touch detected, emilio 04-01-16
	if ( FT800_TOUCHIFC__bTouchPressDetected() ) { GENTIMDT__StartTimer_TB_1SEC( TFTDSPIF__NO_INPUT_IMEOUT_TIME_1S, &mTFTDisplay.NoTouchTimeoutTimingCounter ); }

	FT800_TOUCHIFC__Update();

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises the touch tag value details for rapid update processing - The initial values result in no rapid update.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__RapidUpdateDataInit(void)
{
	FT800_TOUCHIFC__RapidUpdateDataInit();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Provides application level access to define the value details to permit rapid value update.
 * @param:	pTouchTagValue - Pointer to the touch tag value details.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__RapidUpdateDataDefine(FT800_TOUCHIFC__FT800TouchTagValue* pTouchTagValue)
{
	FT800_TOUCHIFC__RapidUpdateDataDefine( pTouchTagValue );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display touch update - Returns tag value.
 * @param:	None.
 * @retval:	uint8_t.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							TFTDSPIF__ucTouchTagValue(void)
{
	return FT800_TOUCHIFC__ucTagToProcess();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display touch update - Flag touch processed.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__TouchTagFlagProcessed(void)
{
	FT800_TOUCHIFC__FlagTagProcessed();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display touch update - Returns touch press.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bTouchPressDetected(void)
{
	return FT800_TOUCHIFC__bTouchPressDetected();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	TFT Display touch update - Flag touch press processed.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__FlagTouchPressDetected(void)
{
	FT800_TOUCHIFC__FlagTouchPressDetected();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags whether or not touch tag values can be assigned.
 * @param:	vbAllow	- Flag to allow/disallow touch tag values.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__SetTouchTagMask(bool vbAllow)
{
	FT800_TOUCHIFC__SetTagMask( vbAllow );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the current tag mask value.
 * @param:	None.
 * @retval:	0 or 1.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							TFTDSPIF__ucTouchTagMask(void)
{
	return FT800_TOUCHIFC__ucTagMask();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Touch cal init.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__TouchCalInit(void)
{
	FT800_DEVICEIF__SwitchBacklight( FUNC_LOOP, 128 );

	FT800_TOUCHIFC__CalInit();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Touch cal redo.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__TouchCalRedo(void)
{
	FT800_DEVICEIF__SwitchBacklight( FUNC_LOOP, 128 );

	FT800_TOUCHIFC__CalRedo();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates cal process completed and that transform values need to be saved.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bTouchCalDoneValidSaveValues(void)
{
	return FT800_TOUCHIFC__bCalDoneValidSaveValues();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates cal process completed but fatal error occurred.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bTouchCalDoneFatal(void)
{
	return FT800_TOUCHIFC__bCalDoneFatal();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates cal process completed.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							TFTDSPIF__bTouchCalDoneValid(void)
{
	return FT800_TOUCHIFC__bCalDoneValid();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions switch backlight off/on.
 * @param:	vAction		  - Action to take within function (Loop until complete or break out).
 * 			vbLoPowerMode - Flag indicating whether system in lo power mode.
 * 			vucLevel	  - Level to switch backlight on at (1 - 128 : 0 is off).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		TFTDSPIF__SwitchBacklight(FUNC_ACTION vAction, bool vbLoPowerMode, uint8_t vucLevel)
{
	if ( vbLoPowerMode )
	{
		if ( vucLevel > TFTDSPIF__MAX_BL_LEVEL_LOPWR_MODE ) { vucLevel = TFTDSPIF__MAX_BL_LEVEL_LOPWR_MODE; }
	}
	else
	{
		if ( vucLevel > TFTDSPIF__MAX_BL_LEVEL_HIPWR_MODE ) { vucLevel = TFTDSPIF__MAX_BL_LEVEL_HIPWR_MODE; }
	}

	return FT800_DEVICEIF__SwitchBacklight( vAction, vucLevel );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reset all touch screen transform values.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							TFTDSPIF__ResetTransformValues(void)
{
	FT800_TOUCHIFC__ResetTransformValues();
	return;
}
/*--------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Transform values A-F..
 * @param:	None.
 * @retval:	uint32_t.
 *------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						TFTDSPIF__ulTransformAValue(void)
{
	return FT800_TOUCHIFC__ulTransformAValue();
}
uint32_t						TFTDSPIF__ulTransformBValue(void)
{
	return FT800_TOUCHIFC__ulTransformBValue();
}
uint32_t						TFTDSPIF__ulTransformCValue(void)
{
	return FT800_TOUCHIFC__ulTransformCValue();
}
uint32_t						TFTDSPIF__ulTransformDValue(void)
{
	return FT800_TOUCHIFC__ulTransformDValue();
}
uint32_t						TFTDSPIF__ulTransformEValue(void)
{
	return FT800_TOUCHIFC__ulTransformEValue();
}
uint32_t						TFTDSPIF__ulTransformFValue(void)
{
	return FT800_TOUCHIFC__ulTransformFValue();
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display State - Idle.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DisplayState_Idle(void)
{
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display State - Driver Init.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DisplayState_DriverInit(void)
{
	FT800_DEVICEIF__FT800DeviceInit_Update();

	if ( FT800_DEVICEIF__FT800DeviceInit_bCompletedAndPassed() )
	{
		// FT800 driver initialisation completed successfully
		mTFTDisplay.State = TFTDSPIF__STATE_DISPLAY_INIT;
	}
	else
	if ( FT800_DEVICEIF__FT800DeviceInit_bCompletedButFailed() )
	{
		// Error during FT800 initialisation - Reset
		MCUAXESS__ResetMicrocontroller();
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display State - Display Init.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DisplayState_DisplayInit(void)
{
#if (TFTDSPIF__TFT_DISPLAY_TYPE == GL050TN33)

	if ( GL050TN33_DISPIF__DisplayInit_bPerformed() )
	{
		mTFTDisplay.State = TFTDSPIF__STATE_READY;

		mTFTDisplay.bInitialised = true;
	}
	else
	{
		mTFTDisplay.State = TFTDSPIF__STATE_DRIVER_INIT;

		FT800_DEVICEIF__FT800DeviceInit_Config();
	}

#endif
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display State - Ready.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						DisplayState_Ready(void)
{
	return;
}
//*********************************************************************************************************************************************************************************
