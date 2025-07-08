
//*********************************************************************************************************************************************************************************
// FT800_displist.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: FT800 Display List Interface.
 *
 * 				Based on FT800 Series Programmer Guide v2.0 (01/07/2014):
 *
 *				Display list commands and functions to access the display list RAM directly.
 *
 *				There are two methods that can be used to update the display list RAM within the FT800:
 *
 *					- Direct access mode.
 *					- Queue data then access mode.
 *
 *
 *				Direct Access Mode
 *				------------------
 *				With this mode commands are written directly ro the FT800 using the SPI interface.
 *				The steps are as foolows:
 *
 *				1) Check whether the display list RAM is ready to accept data using the function:
 *						- FT800_DISPLIST__CheckDLRAMReady().
 *
 *				2) Reset the internal offset counter used:
 *						- FT800_DISPLIST__ResetDLOffset().
 *
 *				3) Write the commands required to the FT800 device using the function:
 *						- FT800_DISPLIST__WriteDLCommand().
 *
 *				4) Once the required display list commands have been written the list needs to be swapped out using:
 *						- FT800_DISPLIST__SwapDLBuffer().
 *
 *				When the FT800 is being accessed either _LOOP or _PASS action can be used.  _LOOP action waits for the SPI transfer to complete.
 *
 *				For example to clear the display to the color red, the following is required:
 *
 *						FT800_DEVICEIF__XFER_STATUS	XferStatus;
 *						bool						bDLRAMReady = false;
 *
 *						XferStatus = FT800_DISPLIST__CheckDLRAMReady(FUNC_LOOP, &bDLRAMReady);
 *
 *						if (bDLRAMReady)
 *						{
 *							FT800_DISPLIST__ResetDLOffset();
 *							XferStatus = FT800_DISPLIST__WriteDLCommand(FUNC_LOOP, FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB(255, 0, 0));
 *							XferStatus = FT800_DISPLIST__WriteDLCommand(FUNC_LOOP, FT800_DISPLIST__DLCmd__CLEAR(1, 1, 1));
 *							XferStatus = FT800_DISPLIST__WriteDLCommand(FUNC_LOOP, FT800_DISPLIST__DLCmd__DISPLAY());
 *							XferStatus = FT800_DISPLIST__SwapDLBuffer(FUNC_LOOP, FT800_DISPLIST__DL_SWAP_TYPE_FRAME);
 *						}
 *
 *				Note that in this example XferStatus is not checked.
 *
 *
 *				Queue Data Then Access Mode
 *				---------------------------
 *				With this mode commands are queued into an internal buffer 8192 bytes long the same length as the FT800 display
 *				list buffer.  Once queued one function can be used to check that the display list RAM is ready, write the queued
 *				data to the display list RAM of the FT800 and then swap out this display list.
 *
 *				1) Reset the queue using the function:
 *						- FT800_DISPLIST__ResetDLQueue().
 *
 *				2) Queue the commands to be written to the display list RAM using the function:
 *						- FT800_DISPLIST__QueueDLCommand().
 *
 *				3) Ensure the display list RAM is ready to accept commands using the function:
 *						- FT800_DISPLIST__CheckDLRAMReady().
 *
 *				4) Write the queued commands to the FT800 device using the function:
 *						- FT800_DISPLIST__WriteDLQueue().
 *
 *				5) Swap the display list buffer out:
 *						- FT800_DISPLIST__SwapDLBuffer().
 *
 *				Steps 3 to 5 can be done using a single function call: FT800_DISPLIST__WriteDLQueueAndSwapDLBuffer().
 *
 *				For example to set clear the display to the color red, the following is required:
 *
 *						FT800_DEVICEIF__XFER_STATUS	XferStatus;
 *
 *						FT800_DISPLIST__ResetDLQueue();
 *
 *						FT800_DISPLIST__QueueDLCommand(FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB(255, 0, 0));
 *						FT800_DISPLIST__QueueDLCommand(FT800_DISPLIST__DLCmd__CLEAR(1, 1, 1));
 *						FT800_DISPLIST__QueueDLCommand(FT800_DISPLIST__DLCmd__DISPLAY());
 *
 *						XferStatus = FT800_DISPLIST__WriteDLQueueAndSwapDLBuffer(FUNC_LOOP, FT800_DISPLIST__DL_SWAP_TYPE_FRAME);
 *
 *				This method only involves the one access to the SPI.
 *
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

#ifndef 		FT800_DISPLIST_H_
#define 		FT800_DISPLIST_H_

#include		<com_defn.h>
#include		<FT800_deviceif.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum	FT800_DISPLIST__DL_SWAP_TYPE {

	FT800_DISPLIST__DL_SWAP_TYPE_LINE  = FT800__REG_DLSWAP_LINE,
	FT800_DISPLIST__DL_SWAP_TYPE_FRAME = FT800__REG_DLSWAP_FRAME

} FT800_DISPLIST__DL_SWAP_TYPE;

typedef enum	FT800_DISPLIST__ALPHA_FUNC {

	FT800_DISPLIST__ALPHA_FUNC__NEVER    = 0,
	FT800_DISPLIST__ALPHA_FUNC__LESS     = 1,
	FT800_DISPLIST__ALPHA_FUNC__LEQUAL   = 2,
	FT800_DISPLIST__ALPHA_FUNC__GREATER  = 3,
	FT800_DISPLIST__ALPHA_FUNC__GEQUAL   = 4,
	FT800_DISPLIST__ALPHA_FUNC__EQUAL    = 5,
	FT800_DISPLIST__ALPHA_FUNC__NOTEQUAL = 6,
	FT800_DISPLIST__ALPHA_FUNC__ALWAYS   = 7

} FT800_DISPLIST__ALPHA_FUNC;

typedef enum	FT800_DISPLIST__BEGIN_PRIMITIVE {

	FT800_DISPLIST__BEGIN_PRIM__BITMAPS      = 1,
	FT800_DISPLIST__BEGIN_PRIM__POINTS       = 2,
	FT800_DISPLIST__BEGIN_PRIM__LINES        = 3,
	FT800_DISPLIST__BEGIN_PRIM__LINE_STRIP   = 4,
	FT800_DISPLIST__BEGIN_PRIM__EDGE_STRIP_R = 5,
	FT800_DISPLIST__BEGIN_PRIM__EDGE_STRIP_L = 6,
	FT800_DISPLIST__BEGIN_PRIM__EDGE_STRIP_A = 7,
	FT800_DISPLIST__BEGIN_PRIM__EDGE_STRIP_B = 8,
	FT800_DISPLIST__BEGIN_PRIM__RECTS        = 9

} FT800_DISPLIST__BEGIN_PRIMITIVE;

typedef enum 	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT {

	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__ARGB1555 = 0,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__L1 	   = 1,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__L4 	   = 2,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__L8 	   = 3,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__RGB332   = 4,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__ARGB2    = 5,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__ARGB4    = 6,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__RGB565   = 7,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__PALETTED = 8,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__TEXT8X8  = 9,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__TEXTVGA  = 10,
	FT800_DISPLIST__BITMAP_LAYOUT_FORMAT__BARGRAPH = 11

} FT800_DISPLIST__BITMAP_LAYOUT_FORMAT;

typedef enum 	FT800_DISPLIST__BITMAP_SIZE_FILTER {

	FT800_DISPLIST__BITMAP_SIZE_FILTER__NEAREST  = 0,
	FT800_DISPLIST__BITMAP_SIZE_FILTER__BILINEAR = 1

} FT800_DISPLIST__BITMAP_SIZE_FILTER;

typedef enum	FT800_DISPLIST__BITMAP_SIZE_WRAP {

	FT800_DISPLIST__BITMAP_SIZE_WRAP__BORDER = 0,
	FT800_DISPLIST__BITMAP_SIZE_WRAP__REPEAT = 1

} FT800_DISPLIST__BITMAP_SIZE_WRAP;

typedef enum	FT800_DISPLIST__BLEND_FUNC_TYPE {

	FT800_DISPLIST__BLEND_FUNC_TYPE__ZERO 	   			 = 0,
	FT800_DISPLIST__BLEND_FUNC_TYPE__ONE  	   			 = 1,
	FT800_DISPLIST__BLEND_FUNC_TYPE__SRC_ALPHA 		     = 2,
	FT800_DISPLIST__BLEND_FUNC_TYPE__DST_ALPHA 			 = 3,
	FT800_DISPLIST__BLEND_FUNC_TYPE__ONE_MINUS_SRC_ALPHA = 4,
	FT800_DISPLIST__BLEND_FUNC_TYPE__ONE_MINUS_DST_ALPHA = 5

} FT800_DISPLIST__BLEND_FUNC_TYPE;

typedef enum	FT800_DISPLIST__STENCIL_TEST_ACTION {

	FT800_DISPLIST__STENCIL_TEST_ACTION__ZERO    = 0,
	FT800_DISPLIST__STENCIL_TEST_ACTION__KEEP    = 1,
	FT800_DISPLIST__STENCIL_TEST_ACTION__REPLACE = 2,
	FT800_DISPLIST__STENCIL_TEST_ACTION__INCR    = 3,
	FT800_DISPLIST__STENCIL_TEST_ACTION__DECR    = 4,
	FT800_DISPLIST__STENCIL_TEST_ACTION__INVERT  = 5

} FT800_DISPLIST__STENCIL_TEST_ACTION;
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
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void							FT800_DISPLIST__ResetDLOffset(void);

extern FT800_DEVICEIF__XFER_STATUS	FT800_DISPLIST__WriteDLCommand(FUNC_ACTION vAction, uint32_t vulCommand);
extern void							FT800_DISPLIST__ResetDLQueue(void);
extern void							FT800_DISPLIST__QueueDLCommand(uint32_t vulCommand);

extern FT800_DEVICEIF__XFER_STATUS	FT800_DISPLIST__WriteDLQueueAndSwapDLBuffer(FUNC_ACTION vAction, FT800_DISPLIST__DL_SWAP_TYPE vSwapType);
extern FT800_DEVICEIF__XFER_STATUS	FT800_DISPLIST__WriteDLQueue(FUNC_ACTION vAction);

extern FT800_DEVICEIF__XFER_STATUS	FT800_DISPLIST__SwapDLBuffer(FUNC_ACTION vAction, FT800_DISPLIST__DL_SWAP_TYPE vSwapType);
extern FT800_DEVICEIF__XFER_STATUS	FT800_DISPLIST__CheckDLRAMReady(FUNC_ACTION vAction, bool* pbDLRAMReady);

extern uint32_t						FT800_DISPLIST__DLCmd__ALPHA_FUNC(FT800_DISPLIST__ALPHA_FUNC vFunc, uint8_t vucRef);

extern uint32_t						FT800_DISPLIST__DLCmd__BEGIN(FT800_DISPLIST__BEGIN_PRIMITIVE vPrimitive);

extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_HANDLE(uint8_t vHandle);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_LAYOUT(FT800_DISPLIST__BITMAP_LAYOUT_FORMAT vFormat, uint16_t vuiLineStride, uint16_t vuiHeight);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_SIZE(FT800_DISPLIST__BITMAP_SIZE_FILTER vFilter, FT800_DISPLIST__BITMAP_SIZE_WRAP vWrapX, FT800_DISPLIST__BITMAP_SIZE_WRAP vWrapY, uint16_t vuiWidth, uint16_t vuiHeight);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_SOURCE(uint32_t vulSRAMAddx);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_A(uint32_t vulA);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_B(uint32_t vulB);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_C(uint32_t vulC);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_D(uint32_t vulD);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_E(uint32_t vulE);
extern uint32_t						FT800_DISPLIST__DLCmd__BITMAP_TRANSFORM_F(uint32_t vulF);

extern uint32_t						FT800_DISPLIST__DLCmd__BLEND_FUNC(FT800_DISPLIST__BLEND_FUNC_TYPE vSrc, FT800_DISPLIST__BLEND_FUNC_TYPE vDst);

extern uint32_t						FT800_DISPLIST__DLCmd__CALL(uint16_t vuiDestAddx);
extern uint32_t						FT800_DISPLIST__DLCmd__CELL(uint8_t vucCell);

extern uint32_t						FT800_DISPLIST__DLCmd__CLEAR(uint8_t vucColorBuffer, uint8_t vucStencilBuffer, uint8_t vucTagBuffer);
extern uint32_t						FT800_DISPLIST__DLCmd__CLEAR_COLOR_A(uint8_t vucAlpha);
extern uint32_t						FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB(uint8_t vucR, uint8_t vucG, uint8_t vucB);
extern uint32_t						FT800_DISPLIST__DLCmd__CLEAR_STENCIL(uint8_t vucStencil);
extern uint32_t						FT800_DISPLIST__DLCmd__CLEAR_TAG(uint8_t vucTag);

extern uint32_t						FT800_DISPLIST__DLCmd__COLOR_A(uint8_t vucAlpha);
extern uint32_t						FT800_DISPLIST__DLCmd__COLOR_MASK(uint8_t vucR, uint8_t vucG, uint8_t vucB, uint8_t vucA);
extern uint32_t						FT800_DISPLIST__DLCmd__COLOR_RGB(uint8_t vucR, uint8_t vucG, uint8_t vucB);

extern uint32_t						FT800_DISPLIST__DLCmd__DISPLAY(void);
extern uint32_t						FT800_DISPLIST__DLCmd__END(void);
extern uint32_t						FT800_DISPLIST__DLCmd__JUMP(uint16_t vuiDestAddx);
extern uint32_t						FT800_DISPLIST__DLCmd__LINE_WIDTH(uint16_t vuiWidth);
extern uint32_t						FT800_DISPLIST__DLCmd__MACRO(uint8_t vucMacro);

extern uint32_t						FT800_DISPLIST__DLCmd__POINT_SIZE(uint32_t vulSize);

extern uint32_t						FT800_DISPLIST__DLCmd__RESTORE_CONTEXT(void);
extern uint32_t						FT800_DISPLIST__DLCmd__RETURN(void);

extern uint32_t						FT800_DISPLIST__DLCmd__SAVE_CONTEXT(void);

extern uint32_t						FT800_DISPLIST__DLCmd__SCISSOR_SIZE(uint16_t vuiWidth, uint16_t vuiHeight);
extern uint32_t						FT800_DISPLIST__DLCmd__SCISSOR_XY(int viX, int viY);

extern uint32_t						FT800_DISPLIST__DLCmd__STENCIL_FUNC(FT800_DISPLIST__ALPHA_FUNC vFunc, uint8_t vucRef, uint8_t vucMask);
extern uint32_t						FT800_DISPLIST__DLCmd__STENCIL_MASK(uint8_t vucMask);
extern uint32_t						FT800_DISPLIST__DLCmd__STENCIL_OP(FT800_DISPLIST__STENCIL_TEST_ACTION vSFail, FT800_DISPLIST__STENCIL_TEST_ACTION vSPass);

extern uint32_t						FT800_DISPLIST__DLCmd__TAG(uint8_t vucTagValue);
extern uint32_t						FT800_DISPLIST__DLCmd__TAG_MASK(uint8_t vucTagMask);

extern uint32_t						FT800_DISPLIST__DLCmd__VERTEX2F(int viX, int viY);
extern uint32_t						FT800_DISPLIST__DLCmd__VERTEX2II(int viX, int viY, uint8_t vucHandle, uint8_t vucCell);
//*********************************************************************************************************************************************************************************

#endif /* FT800_DISPLIST_H_ */
