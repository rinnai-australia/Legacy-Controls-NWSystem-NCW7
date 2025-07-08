//*********************************************************************************************************************************************************************************
//
//	Module:			N-C7 File Listing.
//
//	File: 			hdrfiles_NC7.h.
//
//	Description:	This lists the files specific to the N-C7.
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Modifications List
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	Version:		N-C7 - v090
//  Date:			01/10/2020
//	Modifier:  		Emilio R. La Greca
//	Reason:         Release
//
//*********************************************************************************************************************************************************************************
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <app_main.h>								:	"/SystemApplication/Main"
// <uitiming.h>								:	"/SystemApplication/Management/UserInterface/Timing"
// <i2cclmgr.h>								:	"/SystemApplication/Management/I2CControl"
// <tstifmgr.h>								:	"/SystemApplication/Management/I2CControl"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// DATA
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <paramdsc.h>								:	"/SystemApplication/Data/ParameterDescription"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NETWORKER SYSTEM - OVERALL ACCESS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_userif_NC7.h>						:	"/SystemApplication/Management/NetworkerSystem/Access/Overall/UserInterface"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NETWORKER SYSTEM - REVERSE CYCLE AIRCON
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NETWORKER SYSTEM - HEATING/COOLING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <heating_userif_NC7.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Mode/HeatingCooling/Heating/UserInterface"
// <cooling_userif_NC7.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Mode/HeatingCooling/Cooling/UserInterface"
// <htgclg_userif_NC7.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Mode/HeatingCooling/UserInterface"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NETWORKER SYSTEM - EVAP COOLING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <evpcool_userif_NC7.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Mode/EvpCool/UserInterface"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NETWORKER SYSTEM - ANCILLARY
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Config Menu
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - Access
// <nws_cfgmnu_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/Access/Control"
// <nws_cfgmnu_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/Access/UserInterface"
// <nws_cfgmnu_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/Access/Common"
//
// - Clean Screen
// <nws_clnscn_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/CleanScreen/Control"
// <nws_clnscn_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/CleanScreen/UserInterface"
// <nws_clnscn_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/CleanScreen/Common"
//
// Clock Setting
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_clkset_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/ClockSetting/UserInterface"
//
// Parameter Access
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_prmaxs_userif_NC7.h"				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/InstallerSetup/ParameterAccess/UserInterface"
//
// Installer Setup Menu
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_isetup_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/InstallerSetup/Access/Control"
// <nws_isetup_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/InstallerSetup/Access/UserInterface"
// <nws_isetup_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/InstallerSetup/Access/Common"
//
// Custom Setup Menu
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_csetup_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/InstallerSetup/CustomSetup/Control"
// <nws_csetup_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/InstallerSetup/CustomSetup/UserInterface"
// <nws_csetup_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/InstallerSetup/CustomSetup/Common"
//
// PIN Entry
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_pinent_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/PinEntry/Control"
// <nws_pinent_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/PinEntry/UserInterface"
// <nws_pinent_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/PinEntry/Common"
//
// User Preferences
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_usrprf_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/Access/Control"
// <nws_usrprf_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/Access/UserInterface"
// <nws_usrprf_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/Access/Common"
//
// - User Preferences : Display Settings
// <nws_dspset_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/DisplaySettings/Control"
// <nws_dspset_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/DisplaySettings/UserInterface"
// <nws_dspset_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/DisplaySettings/Common"
//
// - User Preferences : Zone Names
// <nws_zonenm_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/ZoneNames/Control"
// <nws_zonenm_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/ZoneNames/UserInterface"
// <nws_zonenm_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/ZoneNames/Common"
//
// - System Settings
// <nws_sysset_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/SystemSettings/UserInterface"
//
// - Lock/Unlock
// <nws_kpdlck_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ConfigMenu/UserPreferences/LockUnlock/UserInterface"
//
// General Info Prompt
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_prompt_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/InformationPrompt/Control"
// <nws_prompt_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/InformationPrompt/UserInterface"
// <nws_prompt_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/InformationPrompt/Common"
//
// Service Info
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <nws_svcinf_control.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ServiceNotification/Control"
// <nws_svcinf_userif_NC7.h>				:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ServiceNotification/UserInterface"
// <nws_svcinf_common.h>					:	"/SystemApplication/Management/NetworkerSystem/Access/Ancillary/ServiceNotification/Common"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SERVICE LOGGING
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NETWORKER SYSTEM - COMMON UTILITIES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NETWORKER SYSTEM - ZONING MANAGEMENT
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <zoning_userif_NC7.h>					:	"/SystemApplication/Management/NetworkerSystem/Zoning/UserInterface"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TWO-WIRE BUS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// USER INTERFACE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <uiscncap.h>								:	"/SystemApplication/Management/UserInterface/ScreenCapture"
// <uicommon.h>								:	"/SystemApplication/Management/UserInterface/Common"
// <uikeybrd.h>								:	"/SystemApplication/Management/UserInterface/Keyboards"
//
// <uitftdsp.h>								:	"/SystemApplication/Management/UserInterface/TFTDisplay"
//
// <uitftdsp_image.h>						:	"/SystemApplication/Management/UserInterface/TFTDisplay/Images"
// <uitftdsp_image_heating.h>				:	"/SystemApplication/Management/UserInterface/TFTDisplay/Images/Heating"
// <uitftdsp_image_evpcool.h>				:	"/SystemApplication/Management/UserInterface/TFTDisplay/Images/EvpCool"
// <uitftdsp_image_cooling.h>				:	"/SystemApplication/Management/UserInterface/TFTDisplay/Images/Cooling"
// <uitftdsp_image_common.h>				:	"/SystemApplication/Management/UserInterface/TFTDisplay/Images/Common"
//
// <uitftdsp_number.h>						:	"/SystemApplication/Management/UserInterface/TFTDisplay/Numbers"
// <uitftdsp_button.h>						:	"/SystemApplication/Management/UserInterface/TFTDisplay/Buttons"
// <uitftdsp_shape.h>						:	"/SystemApplication/Management/UserInterface/TFTDisplay/Shapes"
// <uitftdsp_text.h>						:	"/SystemApplication/Management/UserInterface/TFTDisplay/Text"
// <uitftdsp_color.h>						:	"/SystemApplication/Management/UserInterface/TFTDisplay/Colors"
// <uitftdsp_fonts.h>						:	"/SystemApplication/Management/UserInterface/TFTDisplay/Fonts"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// NON-VOLATILE MEMORY INTERFACE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// PERIPHERAL
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <tftdspif.h>								:	"/SystemPeripherals/TFTDSPInterface"
// <GL050TN33_dispif.h>						:	"/SystemPeripherals/TFTDSPInterface/GL050TN33Display"
// <FT800_displist.h>						:	"/SystemPeripherals/TFTDSPInterface/FT800Device"
// <FT800_touchifc.h>						:	"/SystemPeripherals/TFTDSPInterface/FT800Device"
// <FT800_coprccmd.h>						:	"/SystemPeripherals/TFTDSPInterface/FT800Device"
// <FT800_deviceif.h>						:	"/SystemPeripherals/TFTDSPInterface/FT800Device"
// <pwrmgtif.h>								:	"/SystemPeripherals/SWITCHInterface"
// <switchif.h>								:	"/SystemPeripherals/SWITCHInterface"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// GENERAL
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MCU ACCESS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <mcuaxess.h>								:	"/DeviceFiles/MicrocontrollerAccess"
// <mcuaxess_usbserif.h>					:	"/DeviceFiles/MicrocontrollerAccess"
// <mcuaxess_twbcomif.h>					:	"/DeviceFiles/MicrocontrollerAccess"
// <mcuaxess_tftdrvif.h>					:	"/DeviceFiles/MicrocontrollerAccess"
// <mcuaxess_tempmsif.h>					:	"/DeviceFiles/MicrocontrollerAccess"
// <mcuaxess_rtmclkif.h>					:	"/DeviceFiles/MicrocontrollerAccess"
// <mcuaxess_i2ccomif.h>					:	"/DeviceFiles/MicrocontrollerAccess"
// <mcuaxess_def.h>							:	"/DeviceFiles/MicrocontrollerAccess"
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//*********************************************************************************************************************************************************************************
