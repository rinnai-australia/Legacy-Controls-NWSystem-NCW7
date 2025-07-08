//*********************************************************************************************************************************************************************************
// build_info.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Build Information.
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 *	Microcontroller - STM32F103RG
 *
 *  Compiler:
 *
 *	TrueSTUDIO
 *  Version:	9.3.0
 *	Build: 		20190212-0734
 *
 *	Optimization Level - None.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	Release Information
  *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 *	NC7 - v094 (22-04-25)
 *
 * 			NC7094 - NCX
 * 			------------
 *			MASTER OPERATION
 *			- Timing command sequences initialising the relevant device command sequence on timeout detected.
 *			--> For the above change done for 093 release, the command sequence execution timeout timer was not started when RQSTHAIL queued so the RQSTHAIL was never sent.
 *			    This was detected with 539 interface which cycles through HAILing all possible Networkers (1 thru 14) so was not being registered - All other controllers that
 *			    just HAIL 0x21 so do not have this issue of not being registered.
 *			    This has been corrected and tested with a 539 interface.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 *	NC7 - v093 (23-10-24)
 *
 * 			NC7093 - NCX
 * 			------------
 * 			SLAVE OPERATION:
 *			- Force CMD_HAIL resetting commmand sequence control when this command issued to make sure it gets out.
 *			- Limit send hail interval timer to 10s minimum changing to a 1 second timer.
 *
 *			MASTER OPERATION
 *			- Timing command sequences initialising the relevant device command sequence on timeout detected.
 *			- Check for HAIL commands received from a slave device:
 *						--> First 20 HAILS detected in succession will force reinitialising of the TWB Comms System.
 *						--> Next 20 HAILS detected in succession will force WD reset.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 *	NC7 - v092 (23-09-24)
 *
 * 			NC7092 - NCX
 * 			------------
 *			Fan speed level setting communicated with AUTO command was not being set to zero when all zones disabled so N-G2 would run fan at max speed for cooldown instead of at
 *			the last speed being operated at.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 *	NC7 - v091 (02-09-24)
 *
 * 			NC7091 - NCX
 * 			------------
 *			# Add registration monitor function for slave networker.  This to address the issue where slave networker is stuck in Hailing Master mode.
 *			  Upon the slave being deregistered the following process is followed:
 *				- Wait a further 10s for registration to be detected (CMD_HAIL issued every 2000ms).
 *				- If still not registered cease bus activity for 10s.
 *				- Initialise the TWB.
 *				- Wait a further 10s for registration to be detected (CMD_HAIL issued every 2000ms).
 *				- If still not registered force a watchdog reset.
 *
 *			# In function SetQuietTicksBeforeTx() of module twbcomif.c for slave networkers the quiet ticks count is set to 2.
 *			  Was set too long especially for N-BW2 (UnitID 11) device which could cause non-recovery issue.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 *	NC7 - v090 (01-10-20)		- Implementation
 *
 *			NC7090
 * 				- SYSNX integration : All code reviewed with formatting updated to latest standard.
 *				- Environmental control algorithm update.
 *				- Dual Fuel function added as per NC6030 release
 *
 * 			NC7090_DFDISP - Dual fuel display update.
 *				- Add display icons an UI functionality to support SmartSens function.
 *
 *			NC7090_SCNCAP - Screen capture fix.
 *
 *			NC7090_OTSCHECK - OD temperature displayed when module comms fault lost.
 *							  When module comms fault detected all temperatures marked as invalid (nwsutils) - This affects N-CX.
 *
 *							- Zone name descriptions corrupted.
 *
 *							- Allow parameter option to display ambient temperature
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *
 *  Compiled with v5.3.0 of TrueSTUDIO C Compiler.
 *
 *	NC7 - v089
 *
 *	Changes - NC7 - v089 (NC6v29)		(27-07-20)
 *	Implement fix to resolve synhronization issue observed in N-C6 networker controller (N-C6 v29 update).
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	NC7 - v088
 *
 *	Changes - emilio 03-10-18
 *  Master accessing another device to upate settings was not waiting a small delay before ussuing zone update command.
 *	This would cause set point to revert back to previous temperature so readjustment would be needed.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	NC7 - v087
 *
 *	Changes - emilio 11-07-18
 *  MTSP operation with master N-C7 not assigned to a zone.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	NC7 - v086
 *
 *	Changes - emilio 03-07-17
 *	Problem with slave N-C7s becoming unresponsive resolved with this release.
 *
 *	UI update is performed via main loop update every 20ms while system control performed via 10ms real time timebase update.
 *  It is believed a conflict was occuring causing access to the FT800 to be stopped.
 *
 *	The 20ms update to the FT800 was done because it was found that updating the device with the next frame was taking at least 15ms.
 *	After careful examination this was found to be incorrect - Most of the time the main loop update of the FT800 was waiting for the FT800 circular buffer RD/WR registers to
 *	match (update of the last frame transmitted).
 * 	This has been changed such that every 20ms a check of the RD/WR circular buffer registers is done - If no match then no update of the FT800 can be done - This check is now
 *	done from the 10ms real time timebase so that the evential UI update is done within this 10ms real time update in line with the control system update.
 *
 *	Image download and any other FT800 utility access functions are done in the main loop as they take time - When these are being performed a utility update flag is set so that
 *  the UI is not updated until this flag is reset in the main loop.  Because of this during the image download the SPI1 interrupt priority is made the lowest to allow the 10ms
 *  interrupt to fire.  Once image download has completed SPI1 interrupt priority is restored to being higher than the 10ms interrupt so it can allow FT800 comms during the
 *	real time timebase interrupt.
 *
 *	Also when the screen is blanked out due to no user activity after some time, user interface update processing still occurs, just the display is physically not updated by
 *  writing the frame data to the FT800 - This ensures variables updated due to control system update are translated to UI updates so that when the display is woken up it will
 *  represent the actual state of the system.
 *
 *	After timing checks with oscilloscope there was enough time to slow the SCK to the FT800 to 4.5MHz thereby giving more time for code processing during FT800 update.
 *
 *	Other changes:
 *	  	- Backlight shutdown and resume has been simplified and is the last process that is checked after UI update.
 *		- Touch detect is performed using a timebase of 10ms as opposed to 20ms to give more responsive user control.
 *		- When FT800 cannot be configured the MCU is reset.
 *		- For FCT access when EEPROM access is the current startup state, the startup state advances to the next state immediately.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  NC7 - v085
 *
 *  Changes - emilio 23-06-17
 *	Restore value saving by slave as this is not cause of screen blank problem.
 *	When display is shutdown set display shutdown active flag to stop display list update.
 *	Every 5 seconds while display is shutdown force a core reset and reconfigure the FT800.  The backlight level is forced to zero
 *  immediately after the core reset to avoid display flicker.
 *  When detecting if a touch has occurred the "FT800__REG_TOUCH_RAW_XY" register is accessed instead of the FT800__REG_TOUCH_DIRECT_XY
 *  register which uses the transform A-F values to indicate if a touch has occurred.
 *  Remove wait for interrupt in main().
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  NC7 - v084
 *
 *	Changes - emilio 16-06-17
 *	No saved values saving to eeprom when operating as slave.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	NC7 - v083
 *
 *	Changes - emilio 07-06-17
 *	Add wait for interrupt in main loop.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	NC7 - v082
 *
 *	Changes - emilio 26-05-17
 *	SPI restart before update
 *	Remove display list for blanking as this taking too long to execute.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	NC7 - v081
 *
 *	Trial only - no release.
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  NC7 - v080
 *
 *  Changes - emilio 05-04-17
 *	- One second debounce on switch toggle detect.
 *  - When updating FT800 touch cal values, ensure values are non-zero before update.
 *  - When saving of general values invoked, do not flag page that holds touch cal values to be saved.  Instead use separate function - NVMIFMGR_StartSaveProcess_TouchCalValues().
 *
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *	NC7 - v079
 *
 *	Changes - jma 18-10-16
 *	1) Allow for both master and slave networkers to revert back to Climate Control Operational State upon exiting System Lockout Control menu.
 *	2) Allow for a slave networker to prompt a remote access message when System Settings menu being accessed by the master networker.
 *
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef 		BUILD_INFO_H_
#define 		BUILD_INFO_H_

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Thermistor type to use - When neither defined EPCOS thermistor supported
//#define     	THERMISTORSHINSHIN      1
#define     	THERMISTORMITSUBISHI    1
//
// NC7 for linked files
#define			BUILD_INFO__NCX_DEVICE				COM_DEFN__NCX_DEVICE__NC7		// NC7 for linked files

#define			BUILD_INFO__RELEASE					COM_DEFN__ENABLE				// Enable for release

#if (BUILD_INFO__RELEASE == COM_DEFN__ENABLE)
// **** RELEASE DEFINES --> NEVER CHANGE THESE ****
#define			BUILD_INFO__WATCHDOG				COM_DEFN__ENABLE				// Watchdog timer					- Enable for production
#define			BUILD_INFO__FAN_LOG_HOURS_REDUCE	COM_DEFN__DISABLE				// Reduce fan hours scaling factor 	- Disable for production (for qualification only)
#define			BUILD_INFO__CYCLE_DUALFUEL_OD_TEMP	COM_DEFN__DISABLE				// Disable for release...
#define			BUILD_INFO__CLEAR_EEPROM			COM_DEFN__DISABLE				// Clear all eeprom

#else
// **** DEBUG DEFINES --> CHANGE AS NEEDED ****
#define			BUILD_INFO__WATCHDOG				COM_DEFN__ENABLE
#define			BUILD_INFO__FAN_LOG_HOURS_REDUCE	COM_DEFN__DISABLE				// Reduce fan hours scaling factor 	- Disable for production (for qualification only)
#define			BUILD_INFO__CYCLE_DUALFUEL_OD_TEMP	COM_DEFN__ENABLE				// Disable for release...
#define			BUILD_INFO__CLEAR_EEPROM			COM_DEFN__DISABLE				// Clear all eeprom

#endif

// LEGACY DEFINES:
// TEST DEFINES START -----------------------------------------------------------------------------------------------------------
//#define		ALLOW_DUMMY_TEST				1		// To check test routines 				- Switch off for production
//#define		LEAVE_BACKLIGHT_ON				1		// Leave backlight on					- Switch off for production
//#define		SERVICE_LOG_1SEC_IS_60MIN		1		// To speed up service logging			- Switch off for production
//#define		HEATER_CTCLE_TIMING				1		// Automatic heater cycle timing		- Switch off for production
//#define		CYCLE_TEMPERATURE				1		// Automatic thermistpr reading cycling - Switch off for production
//#define		INCLUDE_HELP_MENU_OPTION		1		// Help button option					- Switch off for production
//#define		FT800_DEVICE_ID_READ_FAIL_TEST	1		// To test read of FT800 ID failure		- Switch off for production
// TEST DEFINES END-- -----------------------------------------------------------------------------------------------------------
#define		USE_BRIVIS_LOGO				1
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include		<stdint.h>
#include 		"stm32f10x.h"
#include 		"core_cm3.h"

typedef void	(* const PFV_VOID)(void);
//*********************************************************************************************************************************************************************************

#endif /* BUILD_INFO_H_ */
