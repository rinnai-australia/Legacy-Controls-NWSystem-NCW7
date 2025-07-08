//*********************************************************************************************************************************************************************************
// GL050TN33_dispif.c
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Release Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 * Version:		N-C7  	   - v090
 * Date:		01/10/2020
 * Modifier:  	Emilio R. La Greca *
 * Original release.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include		<com_defn.h>
#include		<GL050TN33_dispif.h>
#include		<FT800_coprccmd.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
#define			GL050TN33_DISPIF__Tv			288		// DE V Period Time
#define			GL050TN33_DISPIF__Tvd			272		// DE V Display Area
#define			GL050TN33_DISPIF__Tvb			16		// DE V Blanking
#define			GL050TN33_DISPIF__Tvsync0		5		// VSYNC0
#define			GL050TN33_DISPIF__Tvsync1		10		// VSYNC1

#define			GL050TN33_DISPIF__Th			525		// DE H Period Time
#define			GL050TN33_DISPIF__Thd			480		// DE H Display Area
#define			GL050TN33_DISPIF__Thb			45		// DE H Blanking
#define			GL050TN33_DISPIF__Thsync0		5		// HSYNC0
#define			GL050TN33_DISPIF__Thsync1		10		// HSYNC1

#define			GL050TN33_DISPIF__PCLK_VALUE	4		// 36MHz / 4 = 9MHz
#define			GL050TN33_DISPIF__PCLK_POL		0		// Polarity
*/ // Settings replaced with those from FTDI AN312 to allow swap by frame to function
#define			GL050TN33_DISPIF__Tv				292								// DE V Period Time
#define			GL050TN33_DISPIF__Tvd				GL050TN33_DISPIF__DISPLAY_H		// DE V Display Area
#define			GL050TN33_DISPIF__Tvb				12								// DE V Blanking
#define			GL050TN33_DISPIF__Tvsync0			0								// VSYNC0
#define			GL050TN33_DISPIF__Tvsync1			10								// VSYNC1

#define			GL050TN33_DISPIF__Th				548								// DE H Period Time
#define			GL050TN33_DISPIF__Thd				GL050TN33_DISPIF__DISPLAY_W		// DE H Display Area
#define			GL050TN33_DISPIF__Thb				43								// DE H Blanking
#define			GL050TN33_DISPIF__Thsync0			0								// HSYNC0
#define			GL050TN33_DISPIF__Thsync1			41								// HSYNC1

#define			GL050TN33_DISPIF__PCLK_VALUE		6								// 36MHz / 4 = 9MHz
#define			GL050TN33_DISPIF__PCLK_POL			1								// Polarity
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
 * @brief:	Performs the display initialisation process.
 * @param:	None.
 * @retval:	Flag indicating whether or not initialisation passed.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							GL050TN33_DISPIF__DisplayInit_bPerformed(void)
{
	bool		bNoError = true;

	// - HSIZE
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_HSIZE, GL050TN33_DISPIF__Thd) == FT800_DEVICEIF__XS_DONE_VALID );   						}
	// - HCYCLE
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_HCYCLE, GL050TN33_DISPIF__Th) == FT800_DEVICEIF__XS_DONE_VALID );   						}
	// - HOFFSET
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_HOFFSET, GL050TN33_DISPIF__Thb) == FT800_DEVICEIF__XS_DONE_VALID ); 						}
	// - HSYNC0
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_HSYNC0, GL050TN33_DISPIF__Thsync0) == FT800_DEVICEIF__XS_DONE_VALID );					}
	// - HSYNC1
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_HSYNC1, GL050TN33_DISPIF__Thsync1) == FT800_DEVICEIF__XS_DONE_VALID );					}
	// - VSIZE
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_VSIZE, GL050TN33_DISPIF__Tvd) == FT800_DEVICEIF__XS_DONE_VALID );						}
	// - VCYCLE
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_VCYCLE, GL050TN33_DISPIF__Tv) == FT800_DEVICEIF__XS_DONE_VALID );						}
	// - VOFFSET
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_VOFFSET, GL050TN33_DISPIF__Tvb) == FT800_DEVICEIF__XS_DONE_VALID );						}
	// - VSYNC0
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_VSYNC0, GL050TN33_DISPIF__Tvsync0) == FT800_DEVICEIF__XS_DONE_VALID );					}
	// - VSYNC1
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_VSYNC1, GL050TN33_DISPIF__Tvsync1) == FT800_DEVICEIF__XS_DONE_VALID );					}
	// - SWIZZLE
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_SWIZZLE, 0) == FT800_DEVICEIF__XS_DONE_VALID );											}
	// - PCLK_POL
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_PCLK_POL, GL050TN33_DISPIF__PCLK_POL) == FT800_DEVICEIF__XS_DONE_VALID );				}
	// - Touch mode
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr08(FUNC_LOOP, FT800__REG_TOUCH_MODE, 3) == FT800_DEVICEIF__XS_DONE_VALID );										}
	// - Touch threshold
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__Wr16(FUNC_LOOP, FT800__REG_TOUCH_RZTHRESH, GL050TN33_DISPIF__TOUCH_RZTHRESH) == FT800_DEVICEIF__XS_DONE_VALID );	}

	// Now define initial display list
	FT800_DISPLIST__ResetDLQueue();
	FT800_DISPLIST__QueueDLCommand( FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB( 0, 0, 0 ) );
	FT800_DISPLIST__QueueDLCommand( FT800_DISPLIST__DLCmd__CLEAR( 1, 1, 1 ) );
	FT800_DISPLIST__QueueDLCommand( FT800_DISPLIST__DLCmd__DISPLAY() );

	if ( bNoError ) { bNoError = (bool)( FT800_DISPLIST__WriteDLQueueAndSwapDLBuffer(FUNC_LOOP, FT800_DISPLIST__DL_SWAP_TYPE_FRAME) == FT800_DEVICEIF__XS_DONE_VALID );		}
	// Now enable the display
	if ( bNoError ) { bNoError = (bool)( FT800_DEVICEIF__EnableDisplay(FUNC_LOOP) == FT800_DEVICEIF__XS_DONE_VALID );														}
	// Start PCLK
	if ( bNoError ) { bNoError = (bool)( GL050TN33_DISPIF__StartDisplayRefresh(FUNC_LOOP) == FT800_DEVICEIF__XS_DONE_VALID );												}

	return bNoError;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display settings within FT800 configuration refresh.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							GL050TN33_DISPIF__ConfigRefresh(void)
{
	// Display setup - Register refresh
	// - HSIZE
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		  FT800__REG_HSIZE, 		   GL050TN33_DISPIF__Thd  );
	// - HCYCLE
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		 FT800__REG_HCYCLE, 		    GL050TN33_DISPIF__Th  );
	// - HOFFSET
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		FT800__REG_HOFFSET, 		   GL050TN33_DISPIF__Thb  );
	// - HSYNC0
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		 FT800__REG_HSYNC0, 	   GL050TN33_DISPIF__Thsync0  );
	// - HSYNC1
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		 FT800__REG_HSYNC1, 	   GL050TN33_DISPIF__Thsync1  );
	// - VSIZE
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		  FT800__REG_VSIZE, 		   GL050TN33_DISPIF__Tvd  );
	// - VCYCLE
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		 FT800__REG_VCYCLE, 			GL050TN33_DISPIF__Tv  );
	// - VOFFSET
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 	    FT800__REG_VOFFSET, 		   GL050TN33_DISPIF__Tvb  );
	// - VSYNC0
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		 FT800__REG_VSYNC0, 	   GL050TN33_DISPIF__Tvsync0  );
	// - VSYNC1
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 		 FT800__REG_VSYNC1, 	   GL050TN33_DISPIF__Tvsync1  );
	// - SWIZZLE
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 	    FT800__REG_SWIZZLE, 						       0  );
	// - PCLK_POL
	FT800_DEVICEIF__Wr16( FUNC_LOOP, 	   FT800__REG_PCLK_POL, 	  GL050TN33_DISPIF__PCLK_POL  );
	// - Touch mode
	FT800_DEVICEIF__Wr08( FUNC_LOOP, 	 FT800__REG_TOUCH_MODE, 							   3  );
	// - Touch threshold
	FT800_DEVICEIF__Wr16( FUNC_LOOP, FT800__REG_TOUCH_RZTHRESH, GL050TN33_DISPIF__TOUCH_RZTHRESH  );
	//
	// Enable display
	FT800_DEVICEIF__EnableDisplay( FUNC_LOOP );
	// Start PCLK
	GL050TN33_DISPIF__StartDisplayRefresh( FUNC_LOOP );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows display refresh to be started/stopped (PCLK enabled/disable).
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__StartDisplayRefresh(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__SetPCLKValue( vAction, GL050TN33_DISPIF__PCLK_VALUE );
}
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__StopDisplayRefresh(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__SetPCLKValue( vAction, 0 );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows screen shot function to be enabled/disabled.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotEnable(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__ScreenShot_EnableDisable( vAction, true );
}
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotDisable(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__ScreenShot_EnableDisable( vAction, false );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Defines the Y position for the screen shot capture - 0 based index.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * 			vuiYpos	- Y position for scan : Zero based.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotSetYPosition(FUNC_ACTION vAction, uint16_t vuiYpos)
{
	return FT800_DEVICEIF__ScreenShot_SetYPosition( vAction, vuiYpos );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Flags to start the screen shot.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotStart(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__ScreenShot_Start( vAction );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows screen shot read function to be started/stopped.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotReadStart(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__ScreenShot_ReadStartStop( vAction, true );
}
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotReadStop(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__ScreenShot_ReadStartStop( vAction, false );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows screen shot busy state to be checked.
 * @param:	vAction  - Action to take within function (Loop until complete or break out).
 * 			pbIsBusy - Pointer to memory holding busy status.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotCheckBusyStatus(FUNC_ACTION vAction, bool* pbIsBusy)
{
	return FT800_DEVICEIF__ScreenShot_CheckBusyStatus( vAction, pbIsBusy );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Gets the RGBA values for the current X position.
 * @param:	vAction - Action to take within function (Loop until complete or break out).
 * 			vuiXPos	- X position to read : Zero Based.
 * 			pRGBA	- Pointer to memory holding RGBA values.
 * @retval:	The status of the transfer - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		GL050TN33_DISPIF__ScreenShotReadRGBAValues(FUNC_ACTION vAction, uint16_t vuiXPos, FT800_DEVICEIF__RGBA* pRGBA)
{
	return FT800_DEVICEIF__ScreenShot_ReadRGBAValues( vAction, vuiXPos, pRGBA );
}
//*********************************************************************************************************************************************************************************
