//*********************************************************************************************************************************************************************************
// FT800_deviceif.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: FT800 Device Interface
 *
 * 				Based on FT800 Series Programmer Guide v2.0 (01/07/2014):
 *
 *				Provides the necessary functions to access the FT800 device via SPI Channel 1 interface.
 *
 *				The following function calls are mandatory:
 *
 *					1) FT800_DEVICEIF__Update01ms() - To be updated in 1ms timebase.
 *
 *				Device initialisation is performed using the following functions:
 *
 *					1) FT800_DEVICEIF__FT800DeviceInit_Config() 			 - Begin device initialisation.
 *					2) FT800_DEVICEIF__FT800DeviceInit_Update() 			 - Process device initialisation.
 *					3) FT800_DEVICEIF__FT800DeviceInit_bCompletedAndPassed() - Check for completion (Passed).
 *					   FT800_DEVICEIF__FT800DeviceInit_bCompletedButFailed() - Check for completion (Failed).
 *
 *
 *				TRANSFERRING DATA
 *				-----------------
 *
 *				SPI data transfer is via interrupt so the following coding is required in the SPI1 interrupt service  routine:
 *
 *							void 			SPI1_IRQHandler(void)
 *							{
 *								uint8_t		ucValue;
 *
 *								if ( (SPI1->SR & SPI_SR_RXNE) != 0)
 *								{
 *									FT800_DEVICEIF__UpdateAfterRx(SPI1->DR);
 *								}
 *
 *								if ( (SPI1->SR & SPI_SR_TXE) != 0)
 *								{
 *									if (FT800_DEVICEIF__bTxByteAfterUpdate(&ucValue))
 *									{
 *										SPI1->DR = ucValue;
 *									}
 *									else
 *									{
 *										SPI1->CR2 &= ~SPI_CR2_TXEIE;
 *									}
 *								}
 *
 *								return;
 *							}
 *
 *				The following functions allow data to be transferred to the FT800:
 *
 *						- FT800_DEVICEIF__WrHostCommand()	: Write specific host command to FT800.
 *						- FT800_DEVICEIF__Wr32() 			: Write 4 byte value to FT800.
 *						- FT800_DEVICEIF__Wr16() 			: Write 2 byte value to FT800.
 *						- FT800_DEVICEIF__Wr08() 			: Write 1 byte value to FT800.
 *						- FT800_DEVICEIF__WrN8() 			: Write N bytes to FT800.
 *
 *				The following functions allow data to be transferred from the FT800:
 *
 *						- FT800_DEVICEIF__Rd32() : Read 4 byte value from FT800.
 *						- FT800_DEVICEIF__Rd16() : Read 2 byte value from FT800.
 *						- FT800_DEVICEIF__Rd08() : Read 1 byte value from FT800.
 *						- FT800_DEVICEIF__RdN8() : Read N bytes from FT800.
 *
 *				As the data transfer is via SPI interrupt there is no need to wait for the transaction to complete within any of these data transfer functions.  Therefore the
 *				option is provided to either wait for the transaction to complete or pass through and re-enter the function so as to allow other processing to take place.
 *				All access functions pass a parameter - vAction which can have one of the following values:
 *
 *						1) FUNC_LOOP - Wait for SPI transfer to complete.
 *						2) FUNC_PASS - Pass through function.
 *
 *				All transfer functions return the XferStatus value which is one of the following:
 *
 *						- FT800_DEVICEIF__XS_ACTV		: Transfer is still active (not completed)
 *						- FT800_DEVICEIF__XS_DONE_VALID : Transfer has completed and passed.
 *						- FT800_DEVICEIF__XS_DONE_ERROR : Transfer has completed but failed.
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

#ifndef 		FT800_DEVICEIF_H_
#define 		FT800_DEVICEIF_H_

#include		<com_defn.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FT800 Memory Map Constants
#define			FT800__RAM_G							0x000000			// Main graphics RAM
#define			FT800__ROM_CHIPID						0x0C0000			// Chip Identification
#define			FT800__ROM_FONT							0x0BB23C			// Font table and bitmap
#define			FT800__ROM_FONT_ADDR					0x0FFFFC			// Font table pointer address
#define			FT800__RAM_DL							0x100000			// Display list RAM
#define			FT800__RAM_PAL							0x102000			// Palette RAM
#define			FT800__REG								0x102400			// Registers
#define			FT800__RAM_CMD							0x108000			// Command buffer
#define			FT800__RAM_SCREENSHOT					0x1C2000
//
// FT800 Registers
#define			FT800__REG_ID							0x102400			// Identification register
#define			FT800__REG_FRAMES						0x102404			// Frame counter since reset
#define			FT800__REG_CLOCK						0x102408			// Clock cycles since reset
#define			FT800__REG_FREQUENCY					0x10240C			// Main clock frequency
#define			FT800__REG_RENDERMODE					0x102410			// Rendering mode
#define			FT800__REG_SNAPY						0x102414			// Scan line select for RENDERMODE
#define			FT800__REG_SNAPSHOT						0x102418			// Trigger for RENDERMODE 1
#define			FT800__REG_CPURESET						0x10241C			// Graphics, audio and touch engines reset control
#define			FT800__REG_TAP_CRC						0x102420			// Live video tap crc
#define			FT800__REG_TAP_MASK						0x102424			// Live video tap mask
#define			FT800__REG_HCYCLE						0x102428			// Horizontal total cycle count
#define			FT800__REG_HOFFSET						0x10242C			// Horizontal display start offset
#define			FT800__REG_HSIZE						0x102430			// Horizontal display pixel count
#define			FT800__REG_HSYNC0						0x102434			// Horizontal sync fall offset
#define			FT800__REG_HSYNC1						0x102438			// Horizontal sync rise offset
#define			FT800__REG_VCYCLE						0x10243C			// Vertical total cycle count
#define			FT800__REG_VOFFSET						0x102440			// Vertical display start offset
#define			FT800__REG_VSIZE						0x102444			// Vertical display pixel count
#define			FT800__REG_VSYNC0						0x102448			// Vertical sybc fall offset
#define			FT800__REG_VSYNC1						0x10244C			// Vertical sybc rise offset

#define			FT800__REG_DLSWAP_LINE					0x01				// Display list swap - By line
#define			FT800__REG_DLSWAP_FRAME					0x02				// Display list swap - By frame
#define			FT800__REG_DLSWAP						0x102450			// Display list swap control

#define			FT800__REG_ROTATE						0x102454			// Screen 180 degree rotate
#define			FT800__REG_OUTBITS						0x102458			// Output bit resolution, 3x3x3 bits
#define			FT800__REG_DITHER						0x10245C			// Output dither enable
#define			FT800__REG_SWIZZLE						0x102460			// Output RGB signal swizzle
#define			FT800__REG_CSPREAD						0x102464			// Output clock spreading enable
#define			FT800__REG_PCLK_POL						0x102468			// PCLK polarity - 0 = Output on PCLK rising edge
																			//				   1 = Output on PCLK falling edge
#define			FT800__REG_PCLK							0x10246C			// PCLK frequency divider
#define			FT800__REG_TAG_X						0x102470			// Tag query X coordinate
#define			FT800__REG_TAG_Y						0x102474			// Tag query Y coordinate
#define			FT800__REG_TAG							0x102478			// Tag query result
#define			FT800__REG_VOL_PB						0x10247C			// Volume for playback
#define			FT800__REG_VOL_SOUND					0x102480			// Volume for synthesizer sound
#define			FT800__REG_SOUND						0x102484			// Sound effect select
#define			FT800__REG_PLAY							0x102488			// Start effect playback

#define			FT800__REG_GPIO__DISPLAY_ENABLE_MASK	0x80				// GPIO mask bits
#define			FT800__REG_GPIO_DIR						0x10248C			// GPIO pin direction
#define			FT800__REG_GPIO							0x102490			// GPIO

#define			FT800__REG_INT_FLAGS					0x102498			// Interrupt flags, clear by read
#define			FT800__REG_INT_EN						0x10249C			// Global interrupt enable
#define			FT800__REG_INT_MASK						0x1024A0			// Interrupt enable mask
#define			FT800__REG_PLAYBACK_START				0x1024A4			// Audio playback RAM start address
#define			FT800__REG_PLAYBACK_LENGTH				0x1024A8			// Audio playback sample length (bytes)
#define			FT800__REG_PLAYBACK_READPTR				0x1024AC			// Audio playback current read pointer
#define			FT800__REG_PLAYBACK_FREQ				0x1024B0			// Audio playback sampling frequency (Hz)
#define			FT800__REG_PLAYBACK_FORMAT				0x1024B4			// Audio playback format
#define			FT800__REG_PLAYBACK_LOOP				0x1024B8			// Audio playback loop enable
#define			FT800__REG_PLAYBACK_PLAY				0x1024BC			// Start audio playback
#define			FT800__REG_PWM_HZ						0x1024C0			// BACKLIGHT PWM output frequency (Hz)
#define			FT800__REG_PWM_DUTY						0x1024C4			// BACKLIGHT PWM output duty cycle (0 = 0%, 128 = 100%)
#define			FT800__REG_MACRO_0						0x1024C8			// Display list macro command 0
#define			FT800__REG_MACRO_1						0x1024CC			// Display list macro command 1
#define			FT800__REG_CMD_READ						0x1024E4			// Command buffer read pointer
#define			FT800__REG_CMD_WRITE					0x1024E8			// Command buffer write pointer
#define			FT800__REG_CMD_DL						0x1024EC			// Command display list offset
#define			FT800__REG_TOUCH_MODE					0x1024F0			// Touch screen sampling mode
#define			FT800__REG_TOUCH_ADC_MODE				0x1024F4			// Select single ended (low power) or differential (accurate) sampling
#define			FT800__REG_TOUCH_CHARGE					0x1024F8			// Touch screen charge time )units of 6 clocks)
#define			FT800__REG_TOUCH_SETTLE					0x1024FC			// Touch screen settle time )units of 6 clocks)
#define			FT800__REG_TOUCH_OVERSAMPLE				0x102500			// Touch screen oversample factor
#define			FT800__REG_TOUCH_RZTHRESH				0x102504			// Touch screen resistance threshold
#define			FT800__REG_TOUCH_RAW_XY					0x102508			// Touch screen raw (x-MSB16; y-LSB16)
#define			FT800__REG_TOUCH_RZ						0x10250C			// Touch screen resistance
#define			FT800__REG_TOUCH_SCREEN_XY				0x102510			// Touch screen screen (x-MSB16; y-LSB16)
#define			FT800__REG_TOUCH_TAG_XY					0x102514			// Touch screen screen (x-MSB16; y-LSB16) used for tag lookup
#define			FT800__REG_TOUCH_TAG					0x102518			// Touch screen tag result
#define			FT800__REG_TOUCH_TRANSFORM_A			0x10251C			// Touch screen transform coefficient
#define			FT800__REG_TOUCH_TRANSFORM_B			0x102520			// Touch screen transform coefficient
#define			FT800__REG_TOUCH_TRANSFORM_C			0x102524			// Touch screen transform coefficient
#define			FT800__REG_TOUCH_TRANSFORM_D			0x102528			// Touch screen transform coefficient
#define			FT800__REG_TOUCH_TRANSFORM_E			0x10252C			// Touch screen transform coefficient
#define			FT800__REG_TOUCH_TRANSFORM_F			0x102530			// Touch screen transform coefficient
#define			FT800__REG_TOUCH_DIRECT_XY				0x102574			// Touch screen direct (x-MSB16; y-LSB16) conversions)
#define			FT800__REG_TOUCH_DIRECT_Z1Z2			0x102578			// Touch screen direct (z1-MSB16; z2-LSB16) conversions)
#define			FT800__REG_TRACKER						0x109000			// Track register (Track value - MSB16; Tag value - LSB8)

#define			FT800__REG_SCREENSHOT_READ				0x102554			// Screen shot read
#define			FT800__REG_SCREENSHOT_BUSY				0x1024D8			// Screen shot busy
#define			FT800__REG_SCREENSHOT_START				0x102418			// Screen shot start
#define			FT800__REG_SCREENSHOT_Y					0x102414			// Screen shot Y line
#define			FT800__REG_SCREENSHOT_EN				0x102410			// Screen shot enable
//
// Host Commands
#define			FT800__HOST_CMD_ACTIVE					0x00
#define			FT800__HOST_CMD_STANDBY					0x41
#define			FT800__HOST_CMD_SLEEP					0x42
#define			FT800__HOST_CMD_PWRDOWN					0x50
#define			FT800__HOST_CMD_CLKEXT					0x44
#define			FT800__HOST_CMD_CLK48M					0x62
#define			FT800__HOST_CMD_CLK36M					0x61
#define			FT800__HOST_CMD_CORERST					0x68
//
// FT800 Interrupt Flags
#define			FT800__MASK_INT_FLAG_CONVCOMPLETE		0x80				// Touch screen conversions completed
#define			FT800__MASK_INT_FLAG_CMDFLAG			0x40				// Command FIFO flag
#define			FT800__MASK_INT_FLAG_CMDEMPTY			0x20				// Command FIFO empty
#define			FT800__MASK_INT_FLAG_PLAYBACK			0x10				// Audio playback ended
#define			FT800__MASK_INT_FLAG_SOUND				0x08				// Sound effect ended
#define			FT800__MASK_INT_FLAG_TAG				0x04				// Touch screen tag value change
#define			FT800__MASK_INT_FLAG_TOUCH				0x02				// Touch screen touch detected
#define			FT800__MASK_INT_FLAG_SWAP				0x01				// Display list swap occurred
//
// Font sizes embedded within the device
typedef enum	FT800__FONT_SIZE_OFFSET {

	FT800__FSO__SMALL_1  = 0,
	FT800__FSO__SMALL_2  = 1,
	FT800__FSO__MEDIUM_1 = 2,
	FT800__FSO__MEDIUM_2 = 3,
	FT800__FSO__LARGE_1  = 4,
	FT800__FSO__LARGE_2  = 5

} FT800__FONT_SIZE_OFFSET;

#define			FT800__FONT_BASE_CUSTOM		 0
#define			FT800__FONT_BASE_BOLD		16
#define			FT800__FONT_BASE_SQUARE		20
#define			FT800__FONT_BASE_ROUND		26
//
// Action to take when accessing a transfer function
//	- FUNC_LOOP : Wait for SPI transfer to complete.
//	- FUNC_PASS : Pass through function.
typedef enum { FUNC_PASS = 0, FUNC_LOOP = !FUNC_PASS } FUNC_ACTION;

typedef enum	FT800_DEVICEIF__XFER_STATUS {								// Transfer status
																			//
	FT800_DEVICEIF__XS_ACTV		  = 0,										// - Transfer is still active (not completed)
	FT800_DEVICEIF__XS_DONE_VALID = 1,										// - Transfer has completed and passed.
	FT800_DEVICEIF__XS_DONE_ERROR = 2										// - Transfer has completed but failed.

} FT800_DEVICEIF__XFER_STATUS;

typedef FT800_DEVICEIF__XFER_STATUS (* const PF_FT800_DEVICEIF__XFER_STATUS_VOID)(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	FT800_DEVICEIF__Buffer {			// General Buffer
													//
	uint8_t*	pucData;							// - Pointer to data container
	uint32_t	ulMaxLength;						// - Maximum length the buffer can have
	uint32_t	ulLength;							// - Actual length of data
	uint32_t	ulIndex;							// - Dedicated index into buffer
													//
} FT800_DEVICEIF__Buffer;							//

typedef struct	FT800_DEVICEIF__RGBA {				// RGBA Data
													//
	uint8_t		ucR;								// - Red
	uint8_t		ucG;								// - Green
	uint8_t		ucB;								// - Blue
	uint8_t		ucA;								// - Alpha
													//
} FT800_DEVICEIF__RGBA;								//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SAVED VALUES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TIMING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern const struct	GENTIMDT__Timing	FT800_DEVICEIF__kTFTDriverFailedDelayTiming;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION DECLARES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void								FT800_DEVICEIF__Init(void);
extern void								FT800_DEVICEIF__GoToSleep(void);
extern void								FT800_DEVICEIF__Update01ms(void);

extern void								FT800_DEVICEIF__FT800DeviceInit_Config(void);
extern void								FT800_DEVICEIF__FT800DeviceInit_Update(void);
extern bool								FT800_DEVICEIF__FT800DeviceInit_bCompletedAndPassed(void);
extern bool								FT800_DEVICEIF__FT800DeviceInit_bCompletedButFailed(void);

extern bool								FT800_DEVICEIF__bDeviceIsReady(void);
extern bool								FT800_DEVICEIF__bUpdateTxAfterRx(uint8_t* pucValue);

extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__WrHostCommand(FUNC_ACTION vAction, uint8_t vucHostCommand);

extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Wr32(FUNC_ACTION vAction, uint32_t vulAddx, uint32_t vulData);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Wr16(FUNC_ACTION vAction, uint32_t vulAddx, uint16_t vuiData);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Wr08(FUNC_ACTION vAction, uint32_t vulAddx, uint8_t vucData);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__WrN8(FUNC_ACTION vAction, uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer);

extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Rd32(FUNC_ACTION vAction, uint32_t vulAddx, uint32_t* pulData);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Rd16(FUNC_ACTION vAction, uint32_t vulAddx, uint16_t* puiData);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__Rd08(FUNC_ACTION vAction, uint32_t vulAddx, uint8_t* pucData);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__RdN8(FUNC_ACTION vAction, uint32_t vulAddx, FT800_DEVICEIF__Buffer* pBuffer);

extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__EnableDisplay(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__DisableDisplay(FUNC_ACTION vAction);

extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__SwitchBacklight(FUNC_ACTION vAction, uint8_t vucLevel);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__SetPCLKValue(FUNC_ACTION vAction, uint8_t vucValue);

extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_EnableDisable(FUNC_ACTION vAction, bool vbEnable);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_Start(FUNC_ACTION vAction);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_ReadStartStop(FUNC_ACTION vAction, bool vbStart);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_SetYPosition(FUNC_ACTION vAction, uint16_t vuiYpos);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_CheckBusyStatus(FUNC_ACTION vAction, bool* pbIsBusy);
extern FT800_DEVICEIF__XFER_STATUS		FT800_DEVICEIF__ScreenShot_ReadRGBAValues(FUNC_ACTION vAction, uint16_t vuiXPos, FT800_DEVICEIF__RGBA* pRGBA);
//*********************************************************************************************************************************************************************************

#endif 	/* FT800_DEVICEIF_H_ */
