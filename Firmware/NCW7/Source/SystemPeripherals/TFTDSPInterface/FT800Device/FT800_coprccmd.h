//*********************************************************************************************************************************************************************************
// FT800_coprccmd.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: FT800 Coprocessor Engine Command Interface.
 *
 *				Provides access to the coprocessor FIFO buffer of the FT800 using 3 distinct processes, the required process defined using the function:
 *
 *											FT800_COPRCCMD__CPCmd__DefineAccessType()
 *
 *				Process - 'FT800_COPRCCMD__BAT_QUEUE_XFER'
 *				------------------------------------------
 *				With this process coprocessor commands are queued into a local buffer and then this queue transferred to the  coprocessor FIFO buffer within the FT800.
 *				The following steps must be followed when using this method:
 *
 *				1) Define the access type as follows:
 *						- FT800_COPRCCMD__CPCmd__DefineAccessType(FT800_COPRCCMD__BAT_QUEUE_XFER).
 *
 *				2) Queue the required commands using the functions where applicable:
 *						- FT800_COPRCCMD__CPCmdQueue__CMD_XXX().
 *
 *				3) Issue the queued commands to the FIFO buffer of the FT800 using the function:
 *						- FT800_COPRCCMD__CPCmd__IssueQueue().
 *
 *				Before issuing the queued buffer to the FT800 a check is performed to ensure the buffer is empty.  Only 4096 bytes of data can be queued.  If this is exceeded
 *				data is no longer queued and therefore lost.
 *
 *				For example to clear the display to the color red, the following is required:
 *
 *						FT800_DEVICEIF__XFER_STATUS	XferStatus;
 *
 *						FT800_COPRCCMD__CPCmd__DefineAccessType(FT800_COPRCCMD__BAT_QUEUE_XFER);
 *
 *						FT800_COPRCCMD__CPCmdQueue__CMD_DLSTART();
 *						FT800_COPRCCMD__CPDatQueue__Value(FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB(255, 0, 0));
 *						FT800_COPRCCMD__CPDatQueue__Value(FT800_DISPLIST__DLCmd__CLEAR(1, 1, 1));
 *						FT800_COPRCCMD__CPDatQueue__Value(FT800_DISPLIST__DLCmd__DISPLAY());
 *						FT800_COPRCCMD__CPCmdQueue__CMD_SWAP(FT800_DISPLIST__DL_SWAP_TYPE_FRAME);
 *
 *						XferStatus = FT800_COPRCCMD__CPCmd__IssueQueue(FUNC_LOOP);
 *
 *				Note that in this example XferStatus is not checked when the queue is issued.
 *
 *
 *				Process - 'FT800_COPRCCMD__BAT_CONTINUOUS'
 *				------------------------------------------
 *				With this process coprocessor commands are wriiten to the FIFO buffer as needed with the FT800 instructed to process the commands once the required commands have
 *				been transferred.  The following steps must be followed when using this method:
 *
 *				1) Define the access type as follows:
 *						- FT800_COPRCCMD__CPCmd__DefineAccessType(FT800_COPRCCMD__BAT_CONTINUOUS).
 *
 *				2) Wait for FIFO buffer to be empty using the function:
 *						- FT800_COPRCCMD__CPCmd__BufferStatus().
 *
 *				3) Issue the commands using the functions:
 *						- FT800_COPRCCMD__CPCmdIssue__CMD_XXX().
 *
 *				4) Instruct the FT800 to process the commands using the function:
 *						- FT800_COPRCCMD__CPCmd__BufferDefine().
 *
 *
 *				For example to clear the display to the color red, the following is required:
 *
 *						FT800_DEVICEIF__XFER_STATUS	XferStatus;
 *						bool						bBufferEmpty = false;
 *
 *						FT800_COPRCCMD__CPCmd__DefineAccessType(FT800_COPRCCMD__BAT_CONTINUOUS);
 *
 *						do
 *						{
 *							XferStatus = FT800_COPRCCMD__CPCmd__BufferStatus(FUNC_LOOP, &bBufferEmpty);
 *						}
 *						while ( (XferStatus != FT800_DEVICEIF__XS_DONE_ERROR) && (!bBufferEmpty) );
 *
 *						XferStatus = FT800_COPRCCMD__CPCmdIssue__CMD_DLSTART(FUNC_LOOP);
 *						XferStatus = FT800_COPRCCMD__CPDatIssue__Value(FUNC_LOOP, FT800_DISPLIST__DLCmd__CLEAR_COLOR_RGB(255, 0, 0));
 *						XferStatus = FT800_COPRCCMD__CPDatIssue__Value(FUNC_LOOP, FT800_DISPLIST__DLCmd__CLEAR(1, 1, 1));
 *						XferStatus = FT800_COPRCCMD__CPDatIssue__Value(FUNC_LOOP, FT800_DISPLIST__DLCmd__DISPLAY());
 *						XferStatus = FT800_COPRCCMD__CPCmdIssue__CMD_SWAP(FUNC_LOOP, FT800_DISPLIST__DL_SWAP_TYPE_FRAME);
 *
 *						XferStatus = FT800_COPRCCMD__CPCmd__BufferDefine(FUNC_LOOP);
 *
 *				Note that in this example XferStatus is not checked as commands are issued.
 *
 *
 *				Process - 'FT800_COPRCCMD__BAT_CHECK_BUFF'
 *				------------------------------------------
 *				With this process the coprocessot FIFO buffer within the FT800 is checked to see if enough space is available for the intended command.  If so the command is
 *				written to the FIFO and the FT800 instructed to process the	command immediately.  This is the only method that can be used when performing readback of values
 *				via the	coprocessor engine as the FT800 is instructed to immediately process the command once it issued so for readback the value can be extracted for the
 *				calling code.
 *
 *				The following steps must be followed when using this method:
 *
 *				1) Define the access type as follows:
 *						- FT800_COPRCCMD__CPCmd__DefineAccessType(FT800_COPRCCMD__BAT_CHECK_BUFF).
 *
 *				2) Issue the command and monitor the transfer status returned.  A value of 'FT800_DEVICEIF__XS_ACTV'
 *				   indicates that the process of issuing the command or processing of the command by the FT800 is still
 *				   active.
 *
 *				It is recommended to use _PASS action with this process method as processing of the FIFO buffer by the FT800 takes time.  The _PASS method allows other
 *				processes to be executed in the absence of an RTOS.
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

#ifndef 		FT800_COPRCCMD_H_
#define 		FT800_COPRCCMD_H_

#include		<com_defn.h>
#include		<FT800_displist.h>
#include		<FT800_deviceif.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FT800 Option masks
#define			FT800_COPRCCMD__OPTION_MASK__OPT_DEFAULT		0x00000000
#define			FT800_COPRCCMD__OPTION_MASK__OPT_3D				0x00000000 // 0x00000001
#define			FT800_COPRCCMD__OPTION_MASK__OPT_RGB565			0x00000000 // 0x00000001
#define			FT800_COPRCCMD__OPTION_MASK__OPT_MONO			0x00000001 // 0x00000002
#define			FT800_COPRCCMD__OPTION_MASK__OPT_NODL			0x00000002 // 0x00000004
#define			FT800_COPRCCMD__OPTION_MASK__OPT_FLAT			0x00000100
#define			FT800_COPRCCMD__OPTION_MASK__OPT_SIGNED			0x00000100
#define			FT800_COPRCCMD__OPTION_MASK__OPT_CENTERX		0x00000200
#define			FT800_COPRCCMD__OPTION_MASK__OPT_CENTERY		0x00000400
#define			FT800_COPRCCMD__OPTION_MASK__OPT_CENTER 		0x00000600
#define			FT800_COPRCCMD__OPTION_MASK__OPT_RIGHTX 		0x00000800
#define			FT800_COPRCCMD__OPTION_MASK__OPT_NOBACK 		0x00001000
#define			FT800_COPRCCMD__OPTION_MASK__OPT_NOTICKS 		0x00002000
#define			FT800_COPRCCMD__OPTION_MASK__OPT_NOHM	 		0x00004000
#define			FT800_COPRCCMD__OPTION_MASK__OPT_NOPOINTER 		0x00004000
#define			FT800_COPRCCMD__OPTION_MASK__OPT_NOSECS 		0x00008000
#define			FT800_COPRCCMD__OPTION_MASK__OPT_NOHANDS 		0x0000C000

typedef enum	FT800_COPRCCMD__BUFFER_ACCESS_TYPE {	// Coprocessor command buffer access type
														//
	FT800_COPRCCMD__BAT_QUEUE_XFER = 0,					// - Queue commands in local buffer then initiate transfer
	FT800_COPRCCMD__BAT_CONTINUOUS = 1,					// - Continuously update buffer
	FT800_COPRCCMD__BAT_CHECK_BUFF = 2					// - Always check buffer for available space

} FT800_COPRCCMD__BUFFER_ACCESS_TYPE;
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
extern void							FT800_COPRCCMD__Init(void);

extern void							FT800_COPRCCMD__CPCmd__DefineAccessType(FT800_COPRCCMD__BUFFER_ACCESS_TYPE vAccessType);

extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmd__IssueQueue(FUNC_ACTION vAction);

extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmd__BufferStatus(FUNC_ACTION vAction, bool *pBufferEmpty);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmd__BufferDefine(FUNC_ACTION vAction);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_DLSTART(void);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_DLSTART(FUNC_ACTION vAction);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SWAP(FT800_DISPLIST__DL_SWAP_TYPE vSwapType);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SWAP(FUNC_ACTION vAction, FT800_DISPLIST__DL_SWAP_TYPE vSwapType);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_COLDSTART(void);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_COLDSTART(FUNC_ACTION vAction);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_INTERRUPT(uint32_t vulDelayMS);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_INTERRUPT(FUNC_ACTION vAction, uint32_t vulDelayMS);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_APPEND(uint32_t vulPointer, uint32_t vulNumBytes);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_APPEND(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes);

extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_REGREAD(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t* pulRegisterValue);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMWRITE(uint32_t vulPointer, uint32_t vulNumBytes);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_MEMWRITE(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_INFLATE(uint32_t vulPointer);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_INFLATE(FUNC_ACTION vAction, uint32_t vulPointer);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_LOADIMAGE(uint32_t vulPointer, uint32_t vulOptionsMask);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_LOADIMAGE(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulOptionsMask);

extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_MEMCRC(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes, uint32_t* pulCRC);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMZERO(uint32_t vulPointer, uint32_t vulNumBytes);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_MEMZERO(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulNumBytes);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMSET(uint32_t vulPointer, uint32_t vulValue, uint32_t vulNumBytes);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_MEMSET(FUNC_ACTION vAction, uint32_t vulPointer, uint32_t vulValue, uint32_t vulNumBytes);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_MEMCPY(uint32_t vulPointerDest, uint32_t vulPointerSource, uint32_t vulNumBytes);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_MEMCPY(FUNC_ACTION vAction, uint32_t vulPointerDest, uint32_t vulPointerSource, uint32_t vulNumBytes);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_BUTTON(int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_BUTTON(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_CLOCK(int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiHr, uint16_t vuiMin, uint16_t vuiSec, uint16_t vuiMSec);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_CLOCK(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiHr, uint16_t vuiMin, uint16_t vuiSec, uint16_t vuiMSec);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_FGCOLOR(uint8_t vucR, uint8_t vucG, uint8_t vucB);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_FGCOLOR(FUNC_ACTION vAction, uint8_t vucR, uint8_t vucG, uint8_t vucB);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_BGCOLOR(uint8_t vucR, uint8_t vucG, uint8_t vucB);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_BGCOLOR(FUNC_ACTION vAction, uint8_t vucR, uint8_t vucG, uint8_t vucB);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_GRADCOLOR(uint8_t vucR, uint8_t vucG, uint8_t vucB);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_GRADCOLOR(FUNC_ACTION vAction, uint8_t vucR, uint8_t vucG, uint8_t vucB);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_GAUGE(int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiMajor, uint16_t vuiMinor, uint16_t vuiValue, uint16_t vuiRange);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_GAUGE(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiMajor, uint16_t vuiMinor, uint16_t vuiValue, uint16_t vuiRange);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_GRADIENT(int16_t viX0, int16_t viY0, uint32_t vulRGB0, int16_t viX1, int16_t viY1, uint32_t vulRGB1);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_GRADIENT(FUNC_ACTION vAction, int16_t viX0, int16_t viY0, uint32_t vulRGB0, int16_t viX1, int16_t viY1, uint32_t vulRGB1);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_KEYS(int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrKeysString);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_KEYS(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrKeysString);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_PROGRESS(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_PROGRESS(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SCROLLBAR(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiSize, uint16_t vuiRange);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SCROLLBAR(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiSize, uint16_t vuiRange);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SLIDER(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SLIDER(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint16_t vuiOptionsMask, uint16_t vuiValue, uint16_t vuiRange);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_DIAL(int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiValue);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_DIAL(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viR, uint16_t vuiOptionsMask, uint16_t vuiValue);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_TOGGLE(int16_t viX, int16_t viY, int16_t viW, int16_t viFont, uint16_t vuiOptionsMask, uint16_t vuiState, const char* pstrLabel);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_TOGGLE(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viFont, uint16_t vuiOptionsMask, uint16_t vuiState, const char* pstrLabel);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_TEXT(int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_TEXT(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, const char* pstrText);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_NUMBER(int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, int32_t vilNumber);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_NUMBER(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiOptionsMask, int32_t vilNumber);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_LOADIDENTITY(void);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_LOADIDENTITY(FUNC_ACTION vAction);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SETMATRIX(void);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SETMATRIX(FUNC_ACTION vAction);

extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_GETMATRIX(FUNC_ACTION vAction, int32_t* pilA, int32_t* pilB, int32_t* pilC, int32_t* pilD, int32_t* pilE, int32_t* pilF);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_GETPTR(FUNC_ACTION vAction, uint32_t* pulPointer);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_GETPROPS(FUNC_ACTION vAction, uint32_t* pulPointer, uint32_t* pulWidth, uint32_t* pulHeight);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SCALE(int32_t vilSx, int32_t vilSy);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SCALE(FUNC_ACTION vAction, int32_t vilSx, int32_t vilSy);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_ROTATE(int32_t vilA);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_ROTATE(FUNC_ACTION vAction, int32_t vilA);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_TRANSLATE(int32_t vilTx, int32_t vilTy);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_TRANSLATE(FUNC_ACTION vAction, int32_t vilTx, int32_t vilTy);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_CALIBRATE(void);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_CALIBRATE(FUNC_ACTION vAction, uint32_t* pulResult);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SPINNER(int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiStyle, uint16_t vuiScale);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SPINNER(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viFont, uint16_t vuiStyle, uint16_t vuiScale);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SCREENSAVER(void);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SCREENSAVER(FUNC_ACTION vAction);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SKETCH(int16_t viX, int16_t viY, uint16_t vuiW, uint16_t vuiH, uint32_t vulPtr, uint16_t vuiFormat);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SKETCH(FUNC_ACTION vAction, int16_t viX, int16_t viY, uint16_t vuiW, uint16_t vuiH, uint32_t vulPtr, uint16_t vuiFormat);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_STOP(void);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_STOP(FUNC_ACTION vAction);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SETFONT(uint32_t vulFont, uint32_t vulPtr);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SETFONT(FUNC_ACTION vAction, uint32_t vulFont, uint32_t vulPtr);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_TRACK(int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint8_t vucTag);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_TRACK(FUNC_ACTION vAction, int16_t viX, int16_t viY, int16_t viW, int16_t viH, uint8_t vucTag);

extern void							FT800_COPRCCMD__CPCmdQueue__CMD_SNAPSHOT(uint32_t vulPointer);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPCmdIssue__CMD_SNAPSHOT(FUNC_ACTION vAction, uint32_t vulPointer);

extern void							FT800_COPRCCMD__CPDatQueue__Value(uint32_t vulValue);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPDatIssue__Value(FUNC_ACTION vAction, uint32_t vulValue);

extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CPDatIssue__Buffer(FUNC_ACTION vAction, uint8_t* pucBuffer, uint32_t vulNumBytes);
extern FT800_DEVICEIF__XFER_STATUS	FT800_COPRCCMD__CheckCPFIFOEmpty(FUNC_ACTION vAction, bool* pbFIFOIsEmpty);
//*********************************************************************************************************************************************************************************

#endif /* FT800_COPRCCMD_H_ */
