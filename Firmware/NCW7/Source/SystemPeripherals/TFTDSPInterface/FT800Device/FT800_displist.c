//*********************************************************************************************************************************************************************************
// FT800_displist.c
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
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>
#include		<mcuaxess.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	FT800_DISPLIST__QUEUE_MODE_STATE {				// FT800 Display List Queue Mode State
																//
	FT800_DISPLIST__QMS__WAIT_DL_READY = 0,						// - Ready
	FT800_DISPLIST__QMS__WRITE_QUEUE   = 1,						// - Write the queue
	FT800_DISPLIST__QMS__SWAP_BUFFER   = 2,						// - Swap to buffer for update
	FT800_DISPLIST__QMS__COMPLETED     = 3						// - Completed

} FT800_DISPLIST__BUFFER_MODE_STATE;

typedef struct	FT800DisplayList {								// FT800 Display List
																//
	uint16_t							uiRAMOffset;			// - Offset in RAM
																//
	FT800_DISPLIST__BUFFER_MODE_STATE	QueueModeState;			// - Queue mode state
	FT800_DEVICEIF__Buffer				DataBuffer;				// - Data buffer to access
																//
} FT800DisplayList;												//
																//
static FT800DisplayList		mFT800DisplayList;					// Container

#define			FT800_DISPLIST__UM_BUFFER_MAX_BYTES		8192						// Single update mode buffer
static uint8_t	maucFT800DispListUMBuffer[FT800_DISPLIST__UM_BUFFER_MAX_BYTES];		//
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
 * @brief:	Resets the DL RAM offset value.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DISPLIST__ResetDLOffset(void)
{
	mFT800DisplayList.uiRAMOffset = 0;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Updates the Display RAM with the command passed.  If flagged to this function will loop continuously until the command is written.
 * @param:	vAction	   - Action to take within function (Loop until complete or break out).
 * 			vulCommand - The display list command to execute.
 * @retval:	The transfer status - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DISPLIST__WriteDLCommand(FUNC_ACTION vAction, uint32_t vulCommand)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;

	if ( ( XferStatus = FT800_DEVICEIF__Wr32( vAction, (uint32_t)FT800__RAM_DL + (uint32_t)mFT800DisplayList.uiRAMOffset, vulCommand ) ) == FT800_DEVICEIF__XS_DONE_VALID )
	{
		mFT800DisplayList.uiRAMOffset += 4;
	}

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	When queueing display list commands before writing to the display list RAM this function must first be called to reset the queue.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DISPLIST__ResetDLQueue(void)
{
	mFT800DisplayList.DataBuffer.pucData     = &maucFT800DispListUMBuffer[0];
	mFT800DisplayList.DataBuffer.ulMaxLength = FT800_DISPLIST__UM_BUFFER_MAX_BYTES;
	mFT800DisplayList.DataBuffer.ulLength 	 = 0;

	mFT800DisplayList.QueueModeState = FT800_DISPLIST__QMS__WAIT_DL_READY;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Queues the command passed into the next available spot in the buffer.  If no more space is available the command is	ignored.
 * @param:	vulCommand - The command to queue.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							FT800_DISPLIST__QueueDLCommand(uint32_t vulCommand)
{
	if ( mFT800DisplayList.DataBuffer.ulLength < ( mFT800DisplayList.DataBuffer.ulMaxLength - 4 ) )
	{
		mFT800DisplayList.DataBuffer.pucData[mFT800DisplayList.DataBuffer.ulLength++] = (vulCommand & 0x000000FF) >> 0;
		mFT800DisplayList.DataBuffer.pucData[mFT800DisplayList.DataBuffer.ulLength++] = (vulCommand & 0x0000FF00) >> 8;
		mFT800DisplayList.DataBuffer.pucData[mFT800DisplayList.DataBuffer.ulLength++] = (vulCommand & 0x00FF0000) >> 16;
		mFT800DisplayList.DataBuffer.pucData[mFT800DisplayList.DataBuffer.ulLength++] = (vulCommand & 0xFF000000) >> 24;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Performs the following actions within the one function:
 * 				- FT800_DISPLIST__CheckDLRAMReady().
 * 				- FT800_DISPLIST__WriteDLQueue().
 * 				- FT800_DISPLIST__SwapDLBuffer().
 * @param:	vAction	  - Action to take within function (Loop until complete or break out).
 * 			vSwapType - The swap type (after line - 1, after frame - 2).
 * @retval:	The transfer status - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DISPLIST__WriteDLQueueAndSwapDLBuffer(FUNC_ACTION vAction, FT800_DISPLIST__DL_SWAP_TYPE vSwapType)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	bool						bDLRAMReady;

	// NB : 'mFT800DisplayList.QueueModeState' initialised in function FT800_DISPLIST__ResetDLQueue().

	do
	{
		if ( mFT800DisplayList.QueueModeState == FT800_DISPLIST__QMS__WAIT_DL_READY )
		{
			if ( ( XferStatus = FT800_DISPLIST__CheckDLRAMReady( vAction, &bDLRAMReady ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				if ( bDLRAMReady ) { mFT800DisplayList.QueueModeState = FT800_DISPLIST__QMS__WRITE_QUEUE; }
			}
		}
		if ( mFT800DisplayList.QueueModeState == FT800_DISPLIST__QMS__WRITE_QUEUE )
		{
			if ( ( XferStatus = FT800_DISPLIST__WriteDLQueue( vAction ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				mFT800DisplayList.QueueModeState = FT800_DISPLIST__QMS__SWAP_BUFFER;
			}
		}
		if ( mFT800DisplayList.QueueModeState == FT800_DISPLIST__QMS__SWAP_BUFFER )
		{
			if ( ( XferStatus = FT800_DISPLIST__SwapDLBuffer( vAction, vSwapType ) ) == FT800_DEVICEIF__XS_DONE_VALID )
			{
				mFT800DisplayList.QueueModeState = FT800_DISPLIST__QMS__COMPLETED;
			}
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( mFT800DisplayList.QueueModeState != FT800_DISPLIST__QMS__COMPLETED ) && ( XferStatus != FT800_DEVICEIF__XS_DONE_ERROR ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Writes the queued display list commands to the FT800 device.
 * @param:	vAction	- Action to take within function (Loop until complete or break out).
 * @retval:	The transfer status - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DISPLIST__WriteDLQueue(FUNC_ACTION vAction)
{
	return FT800_DEVICEIF__WrN8( vAction, (uint32_t)FT800__RAM_DL, &mFT800DisplayList.DataBuffer );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Swaps the current display list buffer using the swap type specified.
 * @param:	vAction   - Action to take within function (Loop until complete or break out).
 * 			vSwapType - The swap type (after line - 1, after frame - 2).
 * @retval:	The transfer status - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DISPLIST__SwapDLBuffer(FUNC_ACTION vAction, FT800_DISPLIST__DL_SWAP_TYPE vSwapType)
{
	return FT800_DEVICEIF__Wr32( vAction, FT800__REG_DLSWAP, vSwapType );
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Reads the state of the REG_DLSWAP register to check if the DL RAM is ready to be updated.  Only check the return flag when the transfer status is
 * 			'FT800_DEVICEIF__XS_DONE_VALID'.
 * @param:	vAction		 - Action to take within function (Loop until complete or break out).
 * 			pbDLRAMReady - Pointer to return flag.
 * @retval:	The transfer status - FT800_DEVICEIF__XFER_STATUS.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
FT800_DEVICEIF__XFER_STATUS		FT800_DISPLIST__CheckDLRAMReady(FUNC_ACTION vAction, bool* pbDLRAMReady)
{
	FT800_DEVICEIF__XFER_STATUS	XferStatus;
	uint8_t						ucSwapType;

	do
	{
		if ( ( XferStatus = FT800_DEVICEIF__Rd08( vAction, FT800__REG_DLSWAP, &ucSwapType ) ) == FT800_DEVICEIF__XS_DONE_VALID )
		{
			*pbDLRAMReady = (ucSwapType == 0);
		}
	}
	while ( ( vAction == FUNC_LOOP ) && ( XferStatus == FT800_DEVICEIF__XS_ACTV ) );

	return XferStatus;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for ALPHA_FUNC.
 * @param:	vFunc  - The function (FT800_DISPLIST__ALPHA_FUNC).
 * 			vucRef - Reference value.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__ALPHA_FUNC(FT800_DISPLIST__ALPHA_FUNC vFunc, uint8_t vucRef)
{
	uint32_t	ulCommand = 0x09000000;

	ulCommand |= (uint32_t)vFunc  << 8;
	ulCommand |= (uint32_t)vucRef << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BEGIN.
 * @param:	vPrimitive - The primitive (FT800_DISPLIST__BEGIN_PRIMITIVE).
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BEGIN(FT800_DISPLIST__BEGIN_PRIMITIVE vPrimitive)
{
	uint32_t	ulCommand = 0x1F000000;

	ulCommand |= (uint32_t)( vPrimitive & 0x0F ) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_HANDLE.
 * @param:	vHandle	- Bitmap handle.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_HANDLE(uint8_t vHandle)
{
	uint32_t	ulCommand = 0x05000000;

	ulCommand |= (uint32_t)( vHandle & 0x1F ) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_LAYOUT.
 * @param:	vFormat		  - Pixel format.
 * 			vuiLineStride - Bitmap linestride in bytes.
 * 			vuiHeight	  - Bitmap height in lines.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_LAYOUT(FT800_DISPLIST__BITMAP_LAYOUT_FORMAT vFormat, uint16_t vuiLineStride, uint16_t vuiHeight)
{
	uint32_t	ulCommand = 0x07000000;

	ulCommand |= (uint32_t)(vFormat & 0x1F) << 19;
	ulCommand |= (uint32_t)vuiLineStride    << 9;
	ulCommand |= (uint32_t)vuiHeight      	<< 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_SIZE.
 * @param:	vFilter	  - Filtering mode.
 * 			vWrapX	  - X wrap mode.
 * 			vWrapY	  - Y wrap mode.
 * 			vuiWidth  - Bitmap width in pixels.
 * 			vuiHeight - Bitmap height in pixels.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_SIZE(FT800_DISPLIST__BITMAP_SIZE_FILTER vFilter, FT800_DISPLIST__BITMAP_SIZE_WRAP vWrapX, FT800_DISPLIST__BITMAP_SIZE_WRAP vWrapY, uint16_t vuiWidth, uint16_t vuiHeight)
{
	uint32_t	ulCommand = 0x08000000;

	ulCommand |= (uint32_t)( vFilter & 0x01 ) << 20;
	ulCommand |= (uint32_t)( vWrapX  & 0x01 ) << 19;
	ulCommand |= (uint32_t)( vWrapY  & 0x01 ) << 18;
	ulCommand |= (uint32_t)vuiWidth  << 9;
	ulCommand |= (uint32_t)vuiHeight << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_SOURCE.
 * @param:	vulSRAMAddx - Bitmap adress in FT800 graphics SRAM.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_SOURCE(uint32_t vulSRAMAddx)
{
	uint32_t	ulCommand = 0x01000000;

	ulCommand |= vulSRAMAddx;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_TRANSFORM_A.
 * @param:	vulA - Coefficient A of the bitmap transform matrix.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_A(uint32_t vulA)
{
	uint32_t	ulCommand = 0x15000000;

	ulCommand |= vulA;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_TRANSFORM_B.
 * @param:	vulB - Coefficient B of the bitmap transform matrix.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_B(uint32_t vulB)
{
	uint32_t	ulCommand = 0x16000000;

	ulCommand |= vulB;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_TRANSFORM_C.
 * @param:	vulC - Coefficient C of the bitmap transform matrix.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_C(uint32_t vulC)
{
	uint32_t	ulCommand = 0x17000000;

	ulCommand |= vulC;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_TRANSFORM_D.
 * @param:	vulD - Coefficient D of the bitmap transform matrix.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_D(uint32_t vulD)
{
	uint32_t	ulCommand = 0x18000000;

	ulCommand |= vulD;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_TRANSFORM_E.
 * @param:	vulE - Coefficient E of the bitmap transform matrix.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_E(uint32_t vulE)
{
	uint32_t	ulCommand = 0x19000000;

	ulCommand |= vulE;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BITMAP_TRANSFORM_F.
 * @param:	vulF - Coefficient F of the bitmap transform matrix.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_F(uint32_t vulF)
{
	uint32_t	ulCommand = 0x1F000000;

	ulCommand |= vulF;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for BLEND_FUNC.
 * @param:	vSrc - How to compute source blending factor.
 * 			vDst - How to compute destination blending factor.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__BLEND_FUNC(FT800_DISPLIST__BLEND_FUNC_TYPE vSrc, FT800_DISPLIST__BLEND_FUNC_TYPE vDst)
{
	uint32_t	ulCommand = 0x0B000000;

	ulCommand |= (uint32_t)( vSrc & 0x07 ) << 3;
	ulCommand |= (uint32_t)( vDst & 0x07 ) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for CALL.
 * @param:	vuiDestAddx - destination address in RAM_DL.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__CALL(uint16_t vuiDestAddx)
{
	uint32_t	ulCommand = 0x1D000000;

	ulCommand |= (uint32_t)vuiDestAddx;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for CELL.
 * @param:	vucCell	- Bitmap cell number.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__CELL(uint8_t vucCell)
{
	uint32_t	ulCommand = 0x06000000;

	ulCommand |= (uint32_t)vucCell;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for CLEAR.
 * @param:	vucColorBuffer	 - Clear color buffer when 1.
 * 			vucStencilBuffer - Clear stencil buffer when 1.
 * 			vucTagBuffer	 - Clear tag buffer when 1.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__CLEAR(uint8_t vucColorBuffer, uint8_t vucStencilBuffer, uint8_t vucTagBuffer)
{
	uint32_t	ulCommand = 0x26000000UL;

	ulCommand |= (uint32_t)vucColorBuffer   << 2;
	ulCommand |= (uint32_t)vucStencilBuffer << 1;
	ulCommand |= (uint32_t)vucTagBuffer 	<< 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for CLEAR_COLOR_A.
 * @param:	vucAlpha - Alpha value used.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__CLEAR_COLOR_A(uint8_t vucAlpha)
{
	uint32_t	ulCommand = 0x0F000000;

	ulCommand |= (uint32_t)vucAlpha << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for CLEAR_COLOR_RGB.
 * @param:	vucR - Red color
 * 			vucG - Green color
 * 			vucB - Blue color
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB(uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	uint32_t	ulCommand = 0x02000000UL;

	ulCommand |= (uint32_t)vucR << 16;
	ulCommand |= (uint32_t)vucG << 8;
	ulCommand |= (uint32_t)vucB << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for CLEAR_STENCIL.
 * @param:	vucStencil - Stencil value used when clearing the buffer.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__CLEAR_STENCIL(uint8_t vucStencil)
{
	uint32_t	ulCommand = 0x11000000;

	ulCommand |= (uint32_t)vucStencil << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for CLEAR_TAG.
 * @param:	vucTag - Tag value used when clearing the buffer.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__CLEAR_TAG(uint8_t vucTag)
{
	uint32_t	ulCommand = 0x12000000;

	ulCommand |= (uint32_t)vucTag << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for COLOR_A.
 * @param:	vucTag - Alpha for the current color.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__COLOR_A(uint8_t vucAlpha)
{
	uint32_t	ulCommand = 0x10000000;

	ulCommand |= (uint32_t)vucAlpha << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for COLOR_A.
 * @param:	vucR - Enable/disable of red channel (1 = Enable).
 *			vucG - Enable/disable of green channel (1 = Enable).
 *			vucB - Enable/disable of blue channel (1 = Enable).
 *			vucA - Enable/disable of alpha channel (1 = Enable).
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__COLOR_MASK(uint8_t vucR, uint8_t vucG, uint8_t vucB, uint8_t vucA)
{
	uint32_t	ulCommand = 0x20000000;

	ulCommand |= (uint32_t)(vucR & 0x01) << 3;
	ulCommand |= (uint32_t)(vucG & 0x01) << 2;
	ulCommand |= (uint32_t)(vucB & 0x01) << 1;
	ulCommand |= (uint32_t)(vucA & 0x01) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for COLOR_RGB.
 * @param:	vucR - Red color
 * 			vucG - Green color
 * 			vucB - Blue color
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__COLOR_RGB(uint8_t vucR, uint8_t vucG, uint8_t vucB)
{
	uint32_t	ulCommand = 0x04000000;

	ulCommand |= (uint32_t)vucR << 16;
	ulCommand |= (uint32_t)vucG << 8;
	ulCommand |= (uint32_t)vucB << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for DISPLAY.
 * @param:	None.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__DISPLAY(void)
{
	uint32_t	ulCommand = 0x00000000UL;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for END.
 * @param:	None.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__END(void)
{
	uint32_t	ulCommand = 0x21000000;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for JUMP.
 * @param:	vuiDestAddx	- Destination address in RAM_DL.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__JUMP(uint16_t vuiDestAddx)
{
	uint32_t	ulCommand = 0x1E000000;

	ulCommand |= (uint32_t)vuiDestAddx;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for LINE_WIDTH.
 * @param:	vuiWidth - Line width .
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__LINE_WIDTH(uint16_t vuiWidth)
{
	uint32_t	ulCommand = 0x0E000000;

	ulCommand |= (uint32_t)(vuiWidth & 0xFFF);

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for MACRO.
 * @param:	vucMacro - Macro register to read.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__MACRO(uint8_t vucMacro)
{
	uint32_t	ulCommand = 0x25000000;

	ulCommand |= (uint32_t)(vucMacro & 0x01);

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for MACRO.
 * @param:	vucMacro - Point radius in 1/16 pixel.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__POINT_SIZE(uint32_t vulSize)
{
	uint32_t	ulCommand = 0x0D000000;

	ulCommand |= (vulSize & 0x01FFFF);

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for RESTORE_CONTEXT.
 * @param:	None.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__RESTORE_CONTEXT(void)
{
	uint32_t	ulCommand = 0x23000000;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for RETURN.
 * @param:	None.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__RETURN(void)
{
	uint32_t	ulCommand = 0x24000000;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for SAVE_CONTEXT.
 * @param:	None.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__SAVE_CONTEXT(void)
{
	uint32_t	ulCommand = 0x22000000;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for SCISSOR_SIZE.
 * @param:	vuiWidth  - Width of scissor clip rectangle in pixels.
 * 			vuiHeight - Height of scissor clip rectangle in pixels.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__SCISSOR_SIZE(uint16_t vuiWidth, uint16_t vuiHeight)
{
	uint32_t	ulCommand = 0x1C000000;

	ulCommand |= (uint32_t)(vuiWidth  & 0x3FF) << 10;
	ulCommand |= (uint32_t)(vuiHeight & 0x3FF) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for SCISSOR_XY.
 * @param:	viX - X coordinate of scissor clip rectangle in pixels (-1024 to 1023).
 * 			viY - Y coordinate of scissor clip rectangle in pixels (-1024 to 1023).
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__SCISSOR_XY(int viX, int viY)
{
	uint32_t	ulCommand = 0x1B000000;

	ulCommand |= (uint32_t)(viX & 0x1FF) << 9;
	ulCommand |= (uint32_t)(viY & 0x1FF) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for STENCIL_FUNC.
 * @param:	vFunc   - Specifies the test function (Sames constants of ALPHA_FUNC).
 * 			vucRef  - Specifies the reference value.
 *			vucMask	- Specifes the mask that is ANDed with the reference value.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__STENCIL_FUNC(FT800_DISPLIST__ALPHA_FUNC vFunc, uint8_t vucRef, uint8_t vucMask)
{
	uint32_t	ulCommand = 0x0A000000;

	ulCommand |= (uint32_t)(vFunc   & 0x0F) << 16;
	ulCommand |= (uint32_t)(vucRef  & 0xFF) << 8;
	ulCommand |= (uint32_t)(vucMask & 0xFF) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for STENCIL_MASK.
 * @param:	vucMask - Used to enable writing stencil bits.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__STENCIL_MASK(uint8_t vucMask)
{
	uint32_t	ulCommand = 0x13000000;

	ulCommand |= (uint32_t)vucMask;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for STENCIL_OP.
 * @param:	vSFail - Action to take when stencil test fails.
 * 			vSPass - Action to take when stencil test passes.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__STENCIL_OP(FT800_DISPLIST__STENCIL_TEST_ACTION vSFail, FT800_DISPLIST__STENCIL_TEST_ACTION vSPass)
{
	uint32_t	ulCommand = 0x0C000000;

	ulCommand |= (uint32_t)(vSFail & 0x07) << 3;
	ulCommand |= (uint32_t)(vSPass & 0x07) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for TAG.
 * @param:	vucTagValue	- Tag value to attach.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__TAG(uint8_t vucTagValue)
{
	uint32_t	ulCommand = 0x03000000;

	ulCommand |= (uint32_t)vucTagValue;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for TAG_MASK.
 * @param:	vucTagMask - Mask that allows tag buffer to be updated.
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__TAG_MASK(uint8_t vucTagMask)
{
	uint32_t	ulCommand = 0x14000000;

	ulCommand |= (uint32_t)(vucTagMask & 0x01);

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for VERTEX2F.
 * @param:	vuiX - Signed X coordinate in 1/16 pixel precision (-16384 to +16383).
 * 			vuiY - Signed Y coordinate in 1/16 pixel precision (-16384 to +16383).
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__VERTEX2F(int viX, int viY)
{
	uint32_t	ulCommand = 0x40000000;

	ulCommand |= (uint32_t)(viX & 0x7FFF) << 15;
	ulCommand |= (uint32_t)(viY & 0x7FFF) << 0;

	return ulCommand;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Display list command for VERTEX2II.
 * @param:	vuiX - Signed X coordinate in pixels (-1024 tp 1023).
 * 			vuiY - Signed Y coordinate in pixles (-1024 tp 1023).
 * @retval:	DL Command (uint32_t).
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						FT800_DISPLIST__DLCmd__VERTEX2II(int viX, int viY, uint8_t vucHandle, uint8_t vucCell)
{
	uint32_t	ulCommand = 0x80000000;

	ulCommand |= (uint32_t)(viX       & 0x1FF) << 21;
	ulCommand |= (uint32_t)(viY       & 0x1FF) << 12;
	ulCommand |= (uint32_t)(vucHandle & 0x07)  << 7;
	ulCommand |= (uint32_t)(vucCell   & 0x1F)  << 0;

	return ulCommand;
}
//*********************************************************************************************************************************************************************************
