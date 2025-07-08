//*********************************************************************************************************************************************************************************
// uitftdsp_color.h
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Description: UI TFT Display - Color Details.
 *
 * 				Color setting functions.
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

#ifndef 		UITFTDSP_COLOR_H_
#define 		UITFTDSP_COLOR_H_

#include		<com_defn.h>

//*********************************************************************************************************************************************************************************
// APPLICATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// General color definitions
// - Black
#define         UITFTDSP_COLOR__BLACK          						0x00000000
// - White
#define			UITFTDSP_COLOR__WHITE 								0x00FFFFFF
// - Screen background
#define			UITFTDSP_COLOR__BACKGROUND 							0x0001224D
// - Top status
#define			UITFTDSP_COLOR__TOP_STATUS_LINE						0x00011530
// - Top status menu divider
#define         UITFTDSP_COLOR__TOP_STATUS_MENU_DIVIDER    			0x00002840
// - Passive text
#define			UITFTDSP_COLOR__PASSIVE_TEXT 						0x00538FB0
//
// Button color definitions
#define			UITFTDSP_COLOR__BUTTON_OUTER						0x00304870

#define			UITFTDSP_COLOR__BUTTON_INNER_ACTIVE					0x00B0C8E8
#define			UITFTDSP_COLOR__BUTTON_TEXT_ACTIVE					0x00000000

#define			UITFTDSP_COLOR__BUTTON_INNER_PASSIVE				0x00203858
#define			UITFTDSP_COLOR__BUTTON_TEXT_PASSIVE					0x00FFFFFF

#define			UITFTDSP_COLOR__BUTTON_TEMP_HEATING					0x00F53918
#define			UITFTDSP_COLOR__BUTTON_TEMP_COOLING					0x000080FF
//
// Comfort level color definitions (Evap Cooling) - Supplied by email 07Jan2015
// - Level 1   6B,C9,DB = 01101011 11001001 11011011 = 1011 0111 0011 1011 = B7 3B = 59,183 (from bitmap)
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_01			0x006BC9DB
// - Level 2
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_02			0x0069C3D8
// - Level 3
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_03			0x0065BCD5
// - Level 4
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_04			0x0062B3D1
// - Level 5
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_05			0x005EAACD
// - Level 6
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_06			0x005EAACD
// - Level 7
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_07			0x005495C3
// - Level 8
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_08			0x00508ABE
// - Level 9
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_09			0x004B7FB9
// - Level 10
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_10			0x004674B3
// - Level 11
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_11			0x004169AE
// - Level 12
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_12			0x003D5FAA
// - Level 13
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_13			0x003956A5
// - Level 14
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_14			0x00354DA2
// - Level 15
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_15			0x0032469E
// - Level 16  30,40,9C = 00110000 01000000 10011100 = 1001 1001 0001 0011 = 99h 13h = 19,153
#define			UITFTDSP_COLOR__EVPCOOL_COMFORT_LEVEL_16			0x0030409C
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
extern void						UITFTDSP_COLOR__SetCurrentColor(uint32_t vulColor);
//*********************************************************************************************************************************************************************************

#endif /* UITFTDSP_COLOR_H_ */
