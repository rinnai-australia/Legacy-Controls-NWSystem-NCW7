//*********************************************************************************************************************************************************************************
// nws_kpdlck_userif_NC7.c
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
#include		<nws_kpdlck_common.h>
#include		<nws_kpdlck_userif_NC7.h>
#include		<nws_pinent_control.h>
#include		<nwsutils.h>
#include		<twbif502.h>
#include		<uitftdsp.h>
#include		<uicommon.h>
#include        <tftdspif.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	NWS_KPDLCK_USERIF_NC7__OPERATING_STATE {

	NWS_KPDLCK_USERIF_NC7__OS__UNLOCK = 0,
	NWS_KPDLCK_USERIF_NC7__OS__MASTER = 1,
	NWS_KPDLCK_USERIF_NC7__OS__SLAVE  = 2

} NWS_KPDLCK_USERIF_NC7__OPERATING_STATE;
//
// Top status header text
static const char		mkstrTopStatusText_Master[] = "MENU : SYSTEM LOCKOUT CONTROL";
static const char		mkstrTopStatusText_Remote[] = "ACCESS BARRED : System Lock/Unlock Access Active";
static const char		mkstrTopStatusText_Verify[] = "Verifying PIN Entered - Pleaae Wait";
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tag values
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_KPDLCK_USERIF_NC7__TAG_VALUE {

	NWS_KPDLCK_USERIF_NC7__TV__HELP_BTN	  		  	  	  = 1,
	NWS_KPDLCK_USERIF_NC7__TV__BACK_BTN	  		  	  	  = 2,

	NWS_KPDLCK_USERIF_NC7__TV__SELECT_LOCK_BTN 	          = 11,
	NWS_KPDLCK_USERIF_NC7__TV__SELECT_RESET_PINS_BTN      = 12,
	NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN1_BTN = 13,
	NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN2_BTN = 14

} NWS_KPDLCK_USERIF_NC7__TAG_VALUE;
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// PIN Entry Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum 	NWS_KPDLCK_USERIF_NC7__PIN_ENTRY {					// Pin Entry
																	//
	NWS_KPDLCK_USERIF_NC7__PE__UNLOCK       	= 0,				// - Unlock system,
	NWS_KPDLCK_USERIF_NC7__PE__LOCK         	= 1,				// - Lock system
	NWS_KPDLCK_USERIF_NC7__PE__MASTER_CLEAR 	= 2,				// - Clear user pins with master pin
	NWS_KPDLCK_USERIF_NC7__PE__USER_PIN1_ENTER  = 3,				// - User Pin 1 : Enter
	NWS_KPDLCK_USERIF_NC7__PE__USER_PIN1_ALTER  = 4,				// - User Pin 1 : Alter
	NWS_KPDLCK_USERIF_NC7__PE__USER_PIN1_VERIFY = 5,				// - User Pin 1 : Verify
	NWS_KPDLCK_USERIF_NC7__PE__USER_PIN2_ENTER  = 6,				// - User Pin 2 : Enter
	NWS_KPDLCK_USERIF_NC7__PE__USER_PIN2_ALTER  = 7,				// - User Pin 2 : Alter
	NWS_KPDLCK_USERIF_NC7__PE__USER_PIN2_VERIFY = 8,				// - User Pin 2 : Verify

} NWS_KPDLCK_USERIF_NC7__PIN_ENTRY;
//
// NB: Pin number not used so just leave as zeroes
static const NWS_PINENT_COMMON__PinEntryDetails		mkaPinEntry[] = {				// Use index : NWS_KPDLCK_USERIF_NC7__PIN_ENTRY

	//           Pin Number , Top Status Text		  		  , Pin Description
	{ { '0', '0', '0', '0' }, "MENU : SYSTEM LOCK/UNLOCK"	  , "Enter PIN to unlock:"  	},
	{ { '0', '0', '0', '0' }, "MENU : SYSTEM LOCK/UNLOCK"	  , "Enter PIN to lockout:" 	},
	{ { '0', '0', '0', '0' }, "MENU : RESET USER PIN NUMBERS" , "Enter Master PIN:" 		},
	{ { '0', '0', '0', '0' }, "MENU : ALTER USER PIN NUMBER 1", "Enter Current PIN:" 		},
	{ { '0', '0', '0', '0' }, "MENU : ALTER USER PIN NUMBER 1", "Enter New PIN:" 			},
	{ { '0', '0', '0', '0' }, "MENU : ALTER USER PIN NUMBER 1", "Verify New PIN:" 			},
	{ { '0', '0', '0', '0' }, "MENU : ALTER USER PIN NUMBER 2", "Enter Current PIN:" 		},
	{ { '0', '0', '0', '0' }, "MENU : ALTER USER PIN NUMBER 2", "Enter New PIN:" 			},
	{ { '0', '0', '0', '0' }, "MENU : ALTER USER PIN NUMBER 2", "Verify New PIN:" 			},
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Image Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaImage[]
typedef enum	NWS_KPDLCK_USERIF_NC7__IMAGE_NUMBER {

	NWS_KPDLCK_USERIF_NC7__IN__HELP_BTN = 0,
	NWS_KPDLCK_USERIF_NC7__IN__BACK_BTN = 1

} NWS_KPDLCK_USERIF_NC7__IMAGE_NUMBER;
//
// Array of images
static const UI_TFTIMAGE__ImageAppSpecificDetails	mkaImage[] = {
	//							   Image - Passive,						         Image - Active,   X,   Y, 						        Persist time, Tag
#if defined(INCLUDE_HELP_MENU_OPTION)
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_KPDLCK_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 386,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_KPDLCK_USERIF_NC7__TV__BACK_BTN  }, //   1
#else
	{ UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__HELP_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_KPDLCK_USERIF_NC7__TV__HELP_BTN  }, //   0
	{ UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_PASSIVE, UITFTDSP_IMAGE__IN__COMMON__BACK_BTN_ACTIVE, 429,   3, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, NWS_KPDLCK_USERIF_NC7__TV__BACK_BTN  }, //   1
#endif
};
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Button Details
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Number - Use as index to mkaButton[]
typedef enum	NWS_KPDLCK_USERIF_NC7__BUTTON_NUMBER {

	NWS_KPDLCK_USERIF_NC7__BN__SELECT_LOCK 	          = 0,
	NWS_KPDLCK_USERIF_NC7__BN__SELECT_RESET_PINS      = 1,
	NWS_KPDLCK_USERIF_NC7__BN__SELECT_ALTER_USER_PIN1 = 2,
	NWS_KPDLCK_USERIF_NC7__BN__SELECT_ALTER_USER_PIN2 = 3

} NWS_KPDLCK_USERIF_NC7__BUTTON_NUMBER;
//
// Array of buttons
static const UITFTDSP_BUTTON__ButtonDefinition		mkaButton[] = {
		// BUTTON																										  				      			        		         RECTANGLE						       TEXT
		//                Normal State,   HL,                              Persist time, 					    Style,	                                 			     Tag,       Color,   X,   Y,   W,   H,  LW  	   X,   Y,  XO,  YO, Color,                    FontSize,	       Justification, Text
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,            NWS_KPDLCK_USERIF_NC7__TV__SELECT_LOCK_BTN, {  {      0,  12,  56, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Lockout System"	       } } },	 //  0
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND,      NWS_KPDLCK_USERIF_NC7__TV__SELECT_RESET_PINS_BTN, {  {      0,  12, 110, 458,  44,  0  },  {  -1 , -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Reset User PIN Numbers"  } } },	 //  1
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN1_BTN, {  {      0,  12, 164, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Alter User PIN Number 1" } } },	 //  2
	{	UITFTDSP_BUTTON__STATE_PASSIVE, true, UITFTDSP_BUTTON__PRESS_INDICATE_TIME_SLOW, UITFTDSP_BUTTON__STYLE_ROUND, NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN2_BTN, {  {      0,  12, 218, 458,  44,  0  },  {  -1,  -1,   5,  -1,     0, UITFTDSP_FONTS__FS__ROUND_3, UITFTDSP_TEXT__TJ__LEFT, "Alter User PIN Number 2" } } },	 //  3
};
//
// Macro definition to simplify function calls : X = NWS_USRSET_USERIF_NC7__BUTTON_NUMBER type which indexes array
#define NWS_KPDLCK_USERIF_NC7__Button(X)		( (UITFTDSP_BUTTON__ButtonDefinition*)&mkaButton[X] )
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	LockUnlockDetails {									// Lock/Unlock UI related details
																	//
	NWS_KPDLCK_USERIF_NC7__OPERATING_STATE		OperatingState;		// - The operating state of the UI
	NWS_PINENT_COMMON__PinEntryDetails			PinEntry;			// - Pin entry selection for pin entry UI
	bool										bShowingPinEntry;	// - Showing the PIN entry screen?
	bool										bInitPinEntry;		// - PIN entry screen to be initialised?
																	//
} LockUnlockDetails;												//
																	//
static LockUnlockDetails	mLockUnlock;							// Container
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
static void						RapidUpdateControl_None(void);
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
 * @param:	pKeypadLockUnlock - Pointer to keypad lock/unlock info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__InitBeforeStateEntry(NWS_KPDLCK_COMMON__ControlDetails* pKeypadLockUnlock)
{
    // Action to take when first entering
	if ( NWSUTILS__bUserControlIsLockedOut() )
	{
		// Must unlock first (master/slave)
		mLockUnlock.OperatingState = NWS_KPDLCK_USERIF_NC7__OS__UNLOCK;
		// Just show pin entry screen
		mLockUnlock.bShowingPinEntry = true;
		mLockUnlock.bInitPinEntry    = true;
		// Define details
		mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__UNLOCK];
	}
	else
	if ( TWBIF502__bIsMasterType02Device )
	{
		// Unlocked - Master can allow the following options: - Lock out
		//													  - Clear user pin numbers
		//													  - Alter user pin 1
		//													  - Alter user pin 2
		mLockUnlock.OperatingState = NWS_KPDLCK_USERIF_NC7__OS__MASTER;
		// Shpw selection screen
		mLockUnlock.bShowingPinEntry = false;
		mLockUnlock.bInitPinEntry    = false;
	}
	else
	{
		// Unlocked - Slave can allow only locking of system
		mLockUnlock.OperatingState = NWS_KPDLCK_USERIF_NC7__OS__SLAVE;
		// Just show pin entry screen
		mLockUnlock.bShowingPinEntry = true;
		mLockUnlock.bInitPinEntry    = true;
		// Define details
		mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__LOCK];
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Keypad lock/unlock display update - NC7 specific.
 * @param:	pKeypadLockUnlock - Pointer to keypad lock/unlock info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__UpdateDisplay(NWS_KPDLCK_COMMON__ControlDetails* pKeypadLockUnlock)
{
    UICOMMON__TopStatusDetails		TopStatus;

	if ( !NWSUTILS__bThisUnitEnteredThePinNumberEntryState() )
	{
		// Not this unit - Remote pin entry
	    // Top status line update
	    // - Init structure
	    UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
		TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_Remote;
		TopStatus.TextFlashRate      = UITFTDSP__FR__1HZ;
		TopStatus.bFlashText         = true;
	    // - Perform update
		UICOMMON__TopStatus_Update( &TopStatus );
	}
	else
	if ( mLockUnlock.bShowingPinEntry )
	{
		NWS_PINENT_CONTROL__UserifUpdate_GetPinNumber( mLockUnlock.bInitPinEntry, &mLockUnlock.PinEntry );

		mLockUnlock.bInitPinEntry = false;
	}
	else
	if ( mLockUnlock.OperatingState == NWS_KPDLCK_USERIF_NC7__OS__MASTER )
	{
	    // Top status line update
	    // - Init structure
	    UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
#if defined(INCLUDE_HELP_MENU_OPTION)
	    TopStatus.pImage2 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_KPDLCK_USERIF_NC7__IN__HELP_BTN];
#endif
	    TopStatus.pImage3 = (UI_TFTIMAGE__ImageAppSpecificDetails*)&mkaImage[NWS_KPDLCK_USERIF_NC7__IN__BACK_BTN];
		TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_Master;
	    // - Perform update
		UICOMMON__TopStatus_Update( &TopStatus );
		//
		// Master keypad lock selection
		UITFTDSP__ShowButton( NWS_KPDLCK_USERIF_NC7__Button(NWS_KPDLCK_USERIF_NC7__BN__SELECT_LOCK) );
		UITFTDSP__ShowButton( NWS_KPDLCK_USERIF_NC7__Button(NWS_KPDLCK_USERIF_NC7__BN__SELECT_RESET_PINS) );
		UITFTDSP__ShowButton( NWS_KPDLCK_USERIF_NC7__Button(NWS_KPDLCK_USERIF_NC7__BN__SELECT_ALTER_USER_PIN1) );
		UITFTDSP__ShowButton( NWS_KPDLCK_USERIF_NC7__Button(NWS_KPDLCK_USERIF_NC7__BN__SELECT_ALTER_USER_PIN2) );
	}
	else
	if ( pKeypadLockUnlock->ucIndex == NWS_KPDLCK_COMMON__MAX_PIN_LENGTH )
	{
		// Verifying pin entered

	    // Top status line update
	    // - Init structure
	    UICOMMON__TopStatus_InitStructure( &TopStatus );
		// - Define requirements
		TopStatus.pstrTextToDisplay  = (char*)mkstrTopStatusText_Verify;
		TopStatus.TextFlashRate      = UITFTDSP__FR__4HZ;
		TopStatus.bFlashText         = true;
	    // - Perform update
		UICOMMON__TopStatus_Update( &TopStatus );
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Keypad lock/unlock control update - NC7 specific.
 * @param:	pKeypadLockUnlock - Pointer to keypad lock/unlock info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__UpdateControl(NWS_KPDLCK_COMMON__ControlDetails* pKeypadLockUnlock)
{
	static uint8_t	sucActiveTag = 0;
	uint8_t    		ucTagValue, ucIndex;

	if ( !NWSUTILS__bThisUnitEnteredThePinNumberEntryState() ) { return; }
	//
	// If here this unit is accessing this state - so allow control
	if ( mLockUnlock.bShowingPinEntry )
	{
		if ( NWS_PINENT_CONTROL__bButtonPressed_Ok() )
		{
			// Define the pin number entered
			for ( ucIndex = 0; ucIndex < NWS_KPDLCK_COMMON__MAX_PIN_LENGTH; ucIndex++ )
			{
				pKeypadLockUnlock->aucValue[ucIndex] = 0x0F;

				if ( ucIndex < NWS_PINENT_CONTROL__pPinNumberEntered()->uiLength )
				{
					pKeypadLockUnlock->aucValue[ucIndex] = NWS_PINENT_CONTROL__pPinNumberEntered()->pstrText[ucIndex] - 0x30;
				}
			}
			//
			// Process the pin number
			pKeypadLockUnlock->UserAccess.ButtonPressed = NWS_KPDLCK_COMMON__CB__VALIDATE_PIN_ENTRY; mLockUnlock.bShowingPinEntry = TWBIF502__bIsMasterType02Device;
		}
		else
		if ( NWS_PINENT_CONTROL__bButtonPressed_Cancel() )
		{
			// Just flag lock/unlock cancelled
			pKeypadLockUnlock->UserAccess.ButtonPressed = NWS_KPDLCK_COMMON__CB__LOCK_UNLOCK_CANCEL; mLockUnlock.bShowingPinEntry = false;
		}

		return;
	}
	//
	// Master keypad lock selection
	ucTagValue = TFTDSPIF__ucTouchTagValue();		// Latest tag value of any screen press ( 0 = no press)

	switch ( (NWS_KPDLCK_USERIF_NC7__TAG_VALUE)ucTagValue )
	{
		case NWS_KPDLCK_USERIF_NC7__TV__SELECT_LOCK_BTN:
		case NWS_KPDLCK_USERIF_NC7__TV__SELECT_RESET_PINS_BTN:
		case NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN1_BTN:
		case NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN2_BTN:
		case NWS_KPDLCK_USERIF_NC7__TV__BACK_BTN:
		case NWS_KPDLCK_USERIF_NC7__TV__HELP_BTN:
			sucActiveTag = ucTagValue; RapidUpdateControl_None();
			break;

		default:
			break;
	}

	TFTDSPIF__TouchTagFlagProcessed();

	if ( sucActiveTag != 0 )
	{
		ucTagValue = sucActiveTag;														// Save the tag value for use

		if ( ( sucActiveTag = UITFTDSP__TouchControl_ucActiveTag() ) == 0 )				// Check for removal of persistent tag
		{
			// Active tag now removed so continue with what was required
			switch ( (NWS_KPDLCK_USERIF_NC7__TAG_VALUE)ucTagValue )
			{
				case NWS_KPDLCK_USERIF_NC7__TV__SELECT_LOCK_BTN:
					// Define the access state
					pKeypadLockUnlock->AccessState = NWS_KPDLCK_COMMON__PNAS__SYS_LOCK_UNLOCK;
					// Just show pin entry screen
					mLockUnlock.bShowingPinEntry = true;
					mLockUnlock.bInitPinEntry    = true;
					// Define details
					mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__LOCK];
					break;

				case NWS_KPDLCK_USERIF_NC7__TV__SELECT_RESET_PINS_BTN:
					// Define the access state
					pKeypadLockUnlock->AccessState = NWS_KPDLCK_COMMON__PNAS__RESET_USER_PINS;
					// Just show pin entry screen
					mLockUnlock.bShowingPinEntry = true;
					mLockUnlock.bInitPinEntry    = true;
					// Define details
					mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__MASTER_CLEAR];
					break;

				case NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN1_BTN:
					// Define the access state
					pKeypadLockUnlock->AccessState = NWS_KPDLCK_COMMON__PNAS__ALTER_USER_PIN1;
					// Define alter state
					pKeypadLockUnlock->AlterationState = NWS_KPDLCK_COMMON__PAS__ENTER_CURRENT_PIN;
					// Just show pin entry screen
					mLockUnlock.bShowingPinEntry = true;
					mLockUnlock.bInitPinEntry    = true;
					// Define details
					mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__USER_PIN1_ENTER];
					break;

				case NWS_KPDLCK_USERIF_NC7__TV__SELECT_ALTER_USER_PIN2_BTN:
					// Define the access state
					pKeypadLockUnlock->AccessState = NWS_KPDLCK_COMMON__PNAS__ALTER_USER_PIN2;
					// Define alter state
					pKeypadLockUnlock->AlterationState = NWS_KPDLCK_COMMON__PAS__ENTER_CURRENT_PIN;
					// Just show pin entry screen
					mLockUnlock.bShowingPinEntry = true;
					mLockUnlock.bInitPinEntry    = true;
					// Define details
					mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__USER_PIN2_ENTER];
					break;

				case NWS_KPDLCK_USERIF_NC7__TV__BACK_BTN:
					if ( mLockUnlock.OperatingState == NWS_KPDLCK_USERIF_NC7__OS__MASTER )
					{
						pKeypadLockUnlock->UserAccess.ButtonPressed = NWS_KPDLCK_COMMON__CB__EXIT_MASTER_SELECT;
					}
					else
					{
						pKeypadLockUnlock->UserAccess.ButtonPressed = NWS_KPDLCK_COMMON__CB__LOCK_UNLOCK_CANCEL;
					}
					break;

				case NWS_KPDLCK_USERIF_NC7__TV__HELP_BTN:
					// TODO - Help button
					break;

				default:
					break;
			}
		}
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI update after a valid pin number has been entered.
 * @param:	pKeypadLockUnlock - Pointer to keypad lock/unlock info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__UpdateAfterPinEntryValid(NWS_KPDLCK_COMMON__ControlDetails* pKeypadLockUnlock)
{
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	UI update after an invalid pin number has been entered.
 * @param:	pKeypadLockUnlock - Pointer to keypad lock/unlock info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__UpdateAfterPinEntryInvalid(NWS_KPDLCK_COMMON__ControlDetails* pKeypadLockUnlock)
{
	if ( ( NWSUTILS__bThisUnitEnteredThePinNumberEntryState() ) && ( !TWBIF502__bIsMasterType02Device ) )
	{
		mLockUnlock.bShowingPinEntry = true;
		mLockUnlock.bInitPinEntry    = false;

		NWS_KPDLCK_USERIF_NC7__FlagPinInvalid();
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Check if UI will allow exit from state.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							NWS_KPDLCK_USERIF_NC7__bCanExitState(void)
{
	return true;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to flag pin entry UI when pin invalid.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__FlagPinInvalid(void)
{
	NWS_PINENT_CONTROL__FlagPinInvalid();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Function to reset flags controlling when PIN entry displayed.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__CancelDisplayingPinEntry(void)
{
	mLockUnlock.bShowingPinEntry = false;
	mLockUnlock.bInitPinEntry    = false;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Prepares the PIN entry screen for the next PIN entry alteration state.
 * @param:	pKeypadLockUnlock - Pointer to keypad lock/unlock info.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							NWS_KPDLCK_USERIF_NC7__UpdatePinEntryForNextPinAlterationState(NWS_KPDLCK_COMMON__ControlDetails* pKeypadLockUnlock)
{
	if ( pKeypadLockUnlock->AccessState == NWS_KPDLCK_COMMON__PNAS__ALTER_USER_PIN1 )
	{
		if ( pKeypadLockUnlock->AlterationState == NWS_KPDLCK_COMMON__PAS__ENTER_NEW_PIN )
		{
			mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__USER_PIN1_ALTER];
		}
		else
		{
			mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__USER_PIN1_VERIFY];
		}
	}
	else
	{
		if ( pKeypadLockUnlock->AlterationState == NWS_KPDLCK_COMMON__PAS__ENTER_NEW_PIN )
		{
			mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__USER_PIN2_ALTER];
		}
		else
		{
			mLockUnlock.PinEntry = (NWS_PINENT_COMMON__PinEntryDetails)mkaPinEntry[NWS_KPDLCK_USERIF_NC7__PE__USER_PIN2_VERIFY];
		}
	}

	mLockUnlock.bShowingPinEntry = true;
	mLockUnlock.bInitPinEntry    = true;

	return;
}
//*********************************************************************************************************************************************************************************


//*********************************************************************************************************************************************************************************
// MODULE FUNCTION DEFINITIONS
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Rapid update control - None.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static void						RapidUpdateControl_None(void)
{
	UITFTDSP__TouchControl_RapidUpdateDataInit();
	return;
}
//*********************************************************************************************************************************************************************************
