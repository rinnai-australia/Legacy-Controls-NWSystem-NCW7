//*********************************************************************************************************************************************************************************
// mcuaxess.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: Microcontoller Access - STM32F103RG.
 *
 * 				Provides the functions needed to access the microcontroller I/O and peripherals that realise the N-C7 Control.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

#ifndef 		MCUAXESS_H_
#define 		MCUAXESS_H_

#include		<com_defn.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			MCUAXESS__BACKUP_REGISTER_01		1		// RTC Counter Value (b15..b00)
#define			MCUAXESS__BACKUP_REGISTER_02		2		// RTC Counter Value (b31..b16)
#define			MCUAXESS__BACKUP_REGISTER_03		3		// RTC Day
#define			MCUAXESS__BACKUP_REGISTER_04		4		// Current operating conditions
#define			MCUAXESS__BACKUP_REGISTER_05		5		// Clock saved indicator			NC6 - v02

#define			MCUAXESS__BACKUP_REGISTER_06		6		// Service Logging - Heater fan 	  (Unit 1 to 8)
#define			MCUAXESS__BACKUP_REGISTER_07		7		// 				   - Heater fan 	  (Unit 9 to 15)
#define			MCUAXESS__BACKUP_REGISTER_08		8		// Service Logging - Cooler fan 	  (Unit 1 to 8)
#define			MCUAXESS__BACKUP_REGISTER_09		9		// 				   - Cooler fan 	  (Unit 9 to 15)
#define			MCUAXESS__BACKUP_REGISTER_10		10		// Service Logging - Indoor fan 	  (Unit 1 to 8)
#define			MCUAXESS__BACKUP_REGISTER_11		11		// 				   - Indoor fan 	  (Unit 9 to 15)
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
extern void						MCUAXESS__MicrocontrollerInitialisation(void);

extern void						MCUAXESS__StartWatchdogCounter(void);
extern void						MCUAXESS__RefreshWatchdogCounter(void);
extern void						MCUAXESS__ResetMicrocontroller(void);
extern bool						MCUAXESS__bResetCausedByWatchdog(void);

extern void 					MCUAXESS__WriteBackupRegister(uint16_t vuiRegNo, uint16_t vuiValue);
extern uint16_t 				MCUAXESS__uiReadBackupRegister(uint16_t vuiRegNo);

extern void						MCUAXESS__UpdateIOState_M24M02_WC_Pin(uint8_t vucLogicLevel);

extern bool						MCUAXESS__b12VPowerDetected(void);
extern void						MCUAXESS__Connect5VUSB(onoff voState);

extern onoff					MCUAXESS__oSW1State(void);

extern void						MCUAXESS__SetPC3Hi(bool vbSetHi);
extern void						MCUAXESS__SetPB10Hi(bool vbSetHi);
extern void						MCUAXESS__TogglePB10(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	TFT Driver Access - mcuaxess_tftdrvif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void						MCUAXESS__FT800_InterfaceInit(void);
extern void						MCUAXESS__FT800_SPIPrioritySetLo(void);
extern void						MCUAXESS__FT800_SPIPrioritySetHi(void);
extern void						MCUAXESS__FT800_SPIInitTransfer(uint8_t vucByte);
extern void						MCUAXESS__FT800_SPIStopTransfer(void);
extern void						MCUAXESS__FT800_OperateInFastMode(void);
extern void						MCUAXESS__FT800_OperateInSlowMode(void);
extern bool						MCUAXESS__FT800_bDeviceSelected(void);
extern void						MCUAXESS__FT800_CS(IO_PIN vIOPin);
extern void						MCUAXESS__FT800_PD(IO_PIN vIOPin);
extern bool						MCUAXESS__FT800_bINTPinHi(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	External Comms Access - mcuaxess_usbserif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void						MCUAXESS__USBSerialInterface_Initialise(void);
extern void						MCUAXESS__USBSerialInterface_DisableReceiver(void);
extern void						MCUAXESS__USBSerialInterface_EnableReceiver(void);
extern void						MCUAXESS__USBSerialInterface_FlushReceiverBuffer(void);
extern void						MCUAXESS__USBSerialInterface_TxByte(uint8_t vucByte);
extern void						MCUAXESS__USBSerialInterface_EnableTxInt(void);
extern void						MCUAXESS__USBSerialInterface_DisableTxInt(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Two-wire Bus Comms Access - mcuaxess_twbcomif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void						MCUAXESS__TWBInterface_Initialise(bool vbDoFullInit);
extern void						MCUAXESS__TWBInterface_SwitchOff(void);
extern void						MCUAXESS__TWBInterface_DisableReceiverInterrupt(void);
extern void						MCUAXESS__TWBInterface_EnableReceiverInterrupt(void);
extern void						MCUAXESS__TWBInterface_DisableReceiver(void);
extern void						MCUAXESS__TWBInterface_EnableReceiver(void);
extern void						MCUAXESS__TWBInterface_FlushReceiver(void);
extern void						MCUAXESS__TWBInterface_TxByte(uint8_t vucByte);
extern uint16_t					MCUAXESS__TWBInterface_uiFreeRunningTimerCounterValue(void);
extern bool						MCUAXESS__TWBInterface_bBUSPowerDetected(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	I2C Access - mcuaxess_i2ccomif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void						MCUAXESS__I2CInterface_Initialise(void);
extern void						MCUAXESS__I2CInterface_SwitchOffHardware(void);
extern void						MCUAXESS__I2CInterface_SetStartCondition(void);
extern void						MCUAXESS__I2CInterface_SetStopCondition(void);
extern void						MCUAXESS__I2CInterface_SetAcknowledge(void);
extern void						MCUAXESS__I2CInterface_ClrAcknowledge(void);
extern void						MCUAXESS__I2CInterface_TxByte(uint8_t vucByte);
extern uint8_t					MCUAXESS__I2CInterface_ucByteRxed(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Thermistor Temperature Measurement Access - mcuaxess_tempmsif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void						MCUAXESS__TemperatureMeasurement_SwitchOnOff(onoff voState);
extern void						MCUAXESS__TemperatureMeasurement_EnableADC(void);
extern bool						MCUAXESS__TemperatureMeasurement_bADCalibrationDone(void);
extern uint16_t					MCUAXESS__TemperatureMeasurement_uiADCResult(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	RTC Access - mcuaxess_rtmclkif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern bool						MCUAXESS__RealTimeClock_bFailureDetectedOnConfigurationUpdate(void);
extern uint32_t					MCUAXESS__RealTimeClock_ulCounterRegister(void);
extern void						MCUAXESS__RealTimeClock_SetCounterRegister(uint32_t vulNewCounterValue);
//*********************************************************************************************************************************************************************************

#endif /* MCUAXESS_H_ */
