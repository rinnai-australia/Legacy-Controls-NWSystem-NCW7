//*********************************************************************************************************************************************************************************
// mcuaxess.c
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
#include		<mcuaxess.h>
#include		<mcuaxess_def.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define			MCUAXESS__SYSTEM_TIMEBASE_MS					10

#define			MCUAXESS__TOGGLE_SWITCH_DEBOUNCE_TIMING_TBUI	(20 / GENTIMDT__kucUI_TIMEBASE_10MS)
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
 * @brief:	This function configures the microcontroller ready for operation after a reset.  This function should only be called once after a reset has occurred.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__MicrocontrollerInitialisation(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1) System Configuration - Clock, PLL & Flash: 	Code from SystemInit() function
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	//
	// Reset the RCC clock configuration to the default reset state(for debug purpose)
	// - Set HSION bit
	RCC->CR |= (uint32_t)0x00000001;
	// - Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits
	RCC->CFGR &= (uint32_t)0xF8FF0000;
	// - Reset HSEON, CSSON and PLLON bits
	RCC->CR &= (uint32_t)0xFEF6FFFF;
	// - Reset HSEBYP bit
	RCC->CR &= (uint32_t)0xFFFBFFFF;
	// - Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits
	RCC->CFGR &= (uint32_t)0xFF80FFFF;
	// - Disable all interrupts and clear pending bits
	RCC->CIR = 0x009F0000;
	//
	// SYSCLK, HCLK, PCLK2 and PCLK1 configuration
	// - Enable HSE
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
	// - Wait till HSE is ready and if Time out is reached exit
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY; StartUpCounter++;
	}
	while ( ( HSEStatus == 0 ) && ( StartUpCounter != HSE_STARTUP_TIMEOUT ) );
	// - Define HSEStatus flag
	if ( ( RCC->CR & RCC_CR_HSERDY ) != RESET )
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
	    HSEStatus = (uint32_t)0x00;
	}

	if ( HSEStatus == (uint32_t)0x01 )
	{
		// Configure the Flash Latency cycles and enable prefetch buffer
		// - Enable Prefetch Buffer
	    FLASH->ACR |= FLASH_ACR_PRFTBE;
	    // - Flash 0 wait state
	    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
	    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
	    // Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers
	    // - HCLK = SYSCLK
	    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
	    // - PCLK2 = HCLK/2
	    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
	    // - PCLK1 = HCLK/2
	    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
	    // - PLL configuration -> (HSE) * 9 = 72MHz
	    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
	    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
	    // - Enable PLL
	    RCC->CR |= RCC_CR_PLLON;
	    // - Wait till PLL is ready
	    while ( ( RCC->CR & RCC_CR_PLLRDY ) == 0 )
	    {
	    }
	    // - Select PLL as system clock source
	    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
	    // - Wait till PLL is used as system clock source
	    while ( ( RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != (uint32_t)0x08 )
	    {
	    }
	}
	else
	{
		// If HSE fails to start-up, the application will have wrong clock configuration. User can add here some code to deal with this error
	}
	//
	// Vector Table Relocation in Internal FLASH
  	// SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Enable I/O Port Bus Clocks
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - GPIOA
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
	// - GPIOB
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
	// - GPIOC
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );
	// - GPIOD
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE );
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 3) Setup system tick timer - 10ms interval timer
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Set priority for Cortex-M0 system interrupts
	NVIC_SetPriority( SysTick_IRQn, MCUAXESS_DEF__IP_SYSTICK );
	// - Set reload register
	SysTick->LOAD  =  ( ( ( HCLK * MCUAXESS__SYSTEM_TIMEBASE_MS ) / 1000 ) ) - 1;
	// - Load the SysTick counter value
	SysTick->VAL   =  (0x00);
	// - Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL  =  (SysTick_CTRL_CLKSOURCE_Msk) | (SysTick_CTRL_ENABLE_Msk) | (SysTick_CTRL_TICKINT_Msk);
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 4) Setup system tick timer - 1ms interval timer
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// TIM6 used as 1ms timebase
	// - Enable the peripheral
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	// - Choose a prescaler of 720 -> 72000000 / 720 = 100000
	TIM6->PSC = 720;
	// - ARR every 1ms
	TIM6->ARR = 100;
	// - Only overflow generates event
	TIM6->CR1 |= TIM_CR1_URS;
	// - Interrupt on overflow
	TIM6->DIER |= TIM_DIER_UIE;
	// - Set priority level and enable interrupt (Global)
	NVIC_SetPriority( TIM6_IRQn, MCUAXESS_DEF__IP_TIM6 ); NVIC_EnableIRQ( TIM6_IRQn );
	// - Start the timer
	TIM6->CR1 |= TIM_CR1_CEN;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function starts the independent watchdog timer.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__StartWatchdogCounter(void)
{
	// Start the watchdog
	IWDG->KR = 0xCCCC;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function allows update of the independent watchdog with a 50ms timeout
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__RefreshWatchdogCounter(void)
{
	// Prescaler value update once status bit cleared
	do { ; } while ( ( (uint8_t)IWDG->SR & IWDG_SR_PVU ) != 0 );
	// Update reload value for 50ms WD timeout - LSI @ 40kHz with pre-scaler = 4 -> 10000 x 50ms = 500
	IWDG->KR = 0x5555; IWDG->PR  = IWDG_PR_PR_1;
	//
	// Reload value update once status bit cleared
	do { ; } while ( ( (uint8_t)IWDG->SR & IWDG_SR_RVU ) != 0 );
	// Update reload value for 50ms WD timeout - LSI @ 40kHz with pre-scaler = 4 -> 10000 x 50ms = 500
	IWDG->KR = 0x5555; IWDG->RLR = 0xFFF;
	//
	// Update the watchdog
	IWDG->KR = 0xAAAA;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function resets the micro via the NVIC.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__ResetMicrocontroller(void)
{
	NVIC_SystemReset();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function checks to see if a watchdog timeout caused a reset.
 * @param:	None.
 * @retval:	Flag indicating whether or not a watchdog timeout cause the reset.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS__bResetCausedByWatchdog(void)
{
	bool		bWatchdogReset = false;

	if ( ( RCC->CSR & RCC_CSR_IWDGRSTF ) != 0 )		// Check flag
	{
		bWatchdogReset = true;
	}

	RCC->CSR |= RCC_CSR_RMVF;						// Clear flag

	return bWatchdogReset;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function allows the value passed to be saved in the backup domain register at the register position specified.
 * @param:	vuiRegNo - The register within the backup domain to have data written to it.
 * 			vuiValue - The value to save to the backup register.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void 							MCUAXESS__WriteBackupRegister(uint16_t vuiRegNo, uint16_t vuiValue)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)BKP_BASE; tmp += (vuiRegNo * 4);

	*(__IO uint32_t *) tmp = vuiValue;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function allows the value saved at the specified backup domain register to be retrieved and returned to the calling code.
 * @param:	vuiRegNo - The register within the backup domain to have data retrieved from.
 * @retval:	The value returned from the backup domain register.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint16_t 						MCUAXESS__uiReadBackupRegister(uint16_t vuiRegNo)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)BKP_BASE; tmp += (vuiRegNo * 4);

    return (*(__IO uint16_t *) tmp);
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function allows control of the GPIOA.0 I/O line.
 * @param:	vucLogicLevel - The I/O level to set (0,1).
 * @retval:	Non
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__UpdateIOState_M24M02_WC_Pin(uint8_t vucLogicLevel)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1) GPIOA.0 Configuration: - Configure for output PP mode @ 2MHz speed
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRL register
	ulTempRegister = GPIOA->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); ulTempRegister |= GPIO_CRL_MODE0_1;
	// - Set current state of CRL register
	GPIOA->CRL = ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Update state of port pin
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if ( vucLogicLevel == 0 )
	{
		GPIOA->ODR &= ~(uint32_t)GPIO_ODR_ODR0;
	}
	else
	{
		GPIOA->ODR |=  (uint32_t)GPIO_ODR_ODR0;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function checks if 5V is supplied from 12V external supply.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS__b12VPowerDetected(void)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// MON_12VPWR line : GPIOC.13 Configuration: - Configure depends on state
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRL register
	ulTempRegister = GPIOC->CRH;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13); ulTempRegister |= GPIO_CRH_CNF13_1;
	// - Set current state of CRL register
	GPIOC->CRH = ulTempRegister;
	//
	// Check input register
	if ( ( GPIOC->IDR & GPIO_IDR_IDR13 ) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows connection/disconnection of the 5V USB to the 5V supply voltage.
 * @param:	voState	- ON state connects 5V USB to 5V supply.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__Connect5VUSB(onoff voState)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1) GPIOA.8 Configuration: - Configure for output PP mode @ 2MHz speed
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRH register
	ulTempRegister = GPIOA->CRH;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRH_CNF8 | GPIO_CRH_MODE8); ulTempRegister |= GPIO_CRH_MODE8_1;
	// - Set current state of CRH register
	GPIOA->CRH = ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Update state of port pin
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if ( voState == on )
	{
		GPIOA->ODR |=  (uint32_t)GPIO_ODR_ODR8;
	}
	else
	{
		GPIOA->ODR &= ~(uint32_t)GPIO_ODR_ODR8;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the ON/OFF state of switch 1.
 * @param:	None.
 * @retval:	onoff.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
onoff							MCUAXESS__oSW1State(void)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1) GPIOB.2 Configuration: - Configure for floating input mode
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRL register
	ulTempRegister = GPIOB->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2); ulTempRegister |= GPIO_CRL_CNF2_0;
	// - Set current state of CRL register
	GPIOB->CRL = ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Check state of port pin
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if ( ( GPIOB->IDR & (uint32_t)GPIO_IDR_IDR2 ) == 0 )
	{
		return on;
	}
	else
	{
		return off;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Sets port pin PC3 hi/lo.
 * @param:	vbSetHi - Flag indicating state to set (hi/lo).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__SetPC3Hi(bool vbSetHi)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1) GPIOC.3 Configuration: - Configure for output PP mode @ 2MHz speed
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRH register
	ulTempRegister = GPIOC->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3); ulTempRegister |= GPIO_CRL_MODE3_1;
	// - Set current state of CRH register
	GPIOC->CRL = ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Update state of port pin
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if ( vbSetHi ) { GPIOC->ODR |=  (uint32_t)GPIO_ODR_ODR3; } else { GPIOC->ODR &= ~(uint32_t)GPIO_ODR_ODR3; }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Sets port pin PB10 hi/lo.
 * @param:	vbSetHi - Flag indicating state to set (hi/lo).
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__SetPB10Hi(bool vbSetHi)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1) GPIOB.10 Configuration: - Configure for output PP mode @ 2MHz speed
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRH register
	ulTempRegister = GPIOB->CRH;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10); ulTempRegister |= GPIO_CRH_MODE10_1;
	// - Set current state of CRH register
	GPIOB->CRH = ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Update state of port pin
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if ( vbSetHi ) { GPIOB->ODR |=  (uint32_t)GPIO_ODR_ODR10; } else { GPIOB->ODR &= ~(uint32_t)GPIO_ODR_ODR10; }

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Toggle port pin PB10.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TogglePB10(void)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1) GPIOB.10 Configuration: - Configure for output PP mode @ 2MHz speed
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRH register
	ulTempRegister = GPIOB->CRH;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10); ulTempRegister |= GPIO_CRH_MODE10_1;
	// - Set current state of CRH register
	GPIOB->CRH = ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2) Update state of port pin
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if ( ( GPIOB->ODR & (uint32_t)GPIO_ODR_ODR10 ) != 0 )
	{
		GPIOB->ODR &= ~(uint32_t)GPIO_ODR_ODR10;
	}
	else
	{
		GPIOB->ODR |=  (uint32_t)GPIO_ODR_ODR10;
	}
	return;
}
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	TFT Driver Access - mcuaxess_tftdrvif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include		"mcuaxess_tftdrvif.h"
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Initialises the hardware I/O for interfacing the FT800 device to drive a TFT LCD.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__FT800_InterfaceInit(void)
{
	MCUAXESS_TFTDRVIF__FT800_InterfaceInit();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	These functions allow the SPI interrupt priority level to be set (hi/lo).
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__FT800_SPIPrioritySetLo(void)
{
	MCUAXESS_TFTDRVIF__FT800_SPIPrioritySetLo();
	return;
}
void							MCUAXESS__FT800_SPIPrioritySetHi(void)
{
	MCUAXESS_TFTDRVIF__FT800_SPIPrioritySetHi();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function initialises transfer over SPI to the FT800 device.
 * @param:	vucByte - First byte to transmit.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__FT800_SPIInitTransfer(uint8_t vucByte)
{
	MCUAXESS_TFTDRVIF__FT800_SPIInitTransfer(vucByte);
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function stops any SPI activity.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__FT800_SPIStopTransfer(void)
{
	MCUAXESS_TFTDRVIF__FT800_SPIStopTransfer();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions that allow operating the FT800 SPI in fast/slow mode.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__FT800_OperateInFastMode(void)
{
	MCUAXESS_TFTDRVIF__FT800_OperateInFastMode();
	return;
}
void							MCUAXESS__FT800_OperateInSlowMode(void)
{
	MCUAXESS_TFTDRVIF__FT800_OperateInSlowMode();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates whether or not the FT800 is currently chip selected.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS__FT800_bDeviceSelected(void)
{
	return MCUAXESS_TFTDRVIF__FT800_bDeviceSelected();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows selection/deselection of the FT800 device.
 * @param:	vIOPin - Flag indicating whether to reset/set the CS pin.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__FT800_CS(IO_PIN vIOPin)
{
	MCUAXESS_TFTDRVIF__FT800_CS( vIOPin );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows power-down/power-up of the FT800 device.
 * @param:	vIOPin - Flag indicating whether to reset/set the PD pin.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__FT800_PD(IO_PIN vIOPin)
{
	MCUAXESS_TFTDRVIF__FT800_PD( vIOPin );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when the INTn pin from FT800 is high.
 * @param:	None.
 * @retval:	bool - true when hi.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS__FT800_bINTPinHi(void)
{
	return MCUAXESS_TFTDRVIF__FT800_bINTPinHi();
}
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	External Comms Access - mcuaxess_usbserif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include		"mcuaxess_usbserif.h"
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function configures USART1 and associated I/O lines used to interface with the FT230X USB to serial onverter.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__USBSerialInterface_Initialise(void)
{
	MCUAXESS_USBSERIF__Initialise();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function disables the UART receiver used for USB comms.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__USBSerialInterface_DisableReceiver(void)
{
	MCUAXESS_USBSERIF__DisableReceiver();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function enables the UART receiver used for USB comms.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__USBSerialInterface_EnableReceiver(void)
{
	MCUAXESS_USBSERIF__EnableReceiver();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function flushes the UART receiver used for USB comms.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__USBSerialInterface_FlushReceiverBuffer(void)
{
	MCUAXESS_USBSERIF__FlushReceiverBuffer();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function transmits a byte via the UART transmitter used for USB comms.
 * @param:	vucByte - Byte to transmit.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__USBSerialInterface_TxByte(uint8_t vucByte)
{
	MCUAXESS_USBSERIF__TxByte( vucByte );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function enables the USART1 interrupt that indicates when the UART transmit register is empty.
 * @param:	vucByte - Byte to transmit.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__USBSerialInterface_EnableTxInt(void)
{
	MCUAXESS_USBSERIF__EnableTxInt();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function disables the USART1 interrupt that indicates when the transmit register is empty.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__USBSerialInterface_DisableTxInt(void)
{
	MCUAXESS_USBSERIF__DisableTxInt();
	return;
}
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Two-wire Bus Comms Access - mcuaxess_twbcomif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include		"mcuaxess_twbcomif.h"
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function configures all I/O lines and peripherals used to interface the two-wire bus of the control.
 * @param:	vbDoFullInit - Do full init flag.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*
void			MCUAXESS__TWBInterface_Initialise(void)
*/ // Pass Full Reset flag, NC7093
void			MCUAXESS__TWBInterface_Initialise(bool vbDoFullInit)
{
	MCUAXESS_TWBCOMIF__Initialise( vbDoFullInit );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function switches off the two-wire bus interface by disabling UART4 and all associated interrupts.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TWBInterface_SwitchOff(void)
{
	MCUAXESS_TWBCOMIF__SwitchOff();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions to enable/disable the two-wire bus byte receiver interrupt.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TWBInterface_DisableReceiverInterrupt(void)
{
	MCUAXESS_TWBCOMIF__DisableReceiverInterrupt();
	return;
}
void							MCUAXESS__TWBInterface_EnableReceiverInterrupt(void)
{
	MCUAXESS_TWBCOMIF__EnableReceiverInterrupt();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions to enable/disable the UART receiver used for TWB comms.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TWBInterface_DisableReceiver(void)
{
	MCUAXESS_TWBCOMIF__DisableReceiver();
	return;
}
void							MCUAXESS__TWBInterface_EnableReceiver(void)
{
	MCUAXESS_TWBCOMIF__EnableReceiver();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function flushes the UART receiver used for TWB comms.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TWBInterface_FlushReceiver(void)
{
	MCUAXESS_TWBCOMIF__FlushReceiver();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function transmits a byte via the UART transmitter used for TWB comms.
 * @param:	vucByte - Byte to transmit.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TWBInterface_TxByte(uint8_t vucByte)
{
	MCUAXESS_TWBCOMIF__TxByte( vucByte );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns the value of the free running counter.
 * @param:	None.
 * @retval:	Free running counter value.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint16_t						MCUAXESS__TWBInterface_uiFreeRunningTimerCounterValue(void)
{
	return MCUAXESS_TWBCOMIF__uiFreeRunningTimerCounterValue();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates whether or not the two-wire bus power supply is available.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS__TWBInterface_bBUSPowerDetected(void)
{
	return MCUAXESS_TWBCOMIF__bBUSPowerDetected();
}
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	I2C Access - mcuaxess_i2ccomif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include		"mcuaxess_i2ccomif.h"
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function configures all I/O lines and peripherals used to interface the I2C bus of the control.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__I2CInterface_Initialise(void)
{
	MCUAXESS_I2CCOMIF__Initialise();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function switches off the I2C bus interface by disabling I2C1 and all associated interrupts.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__I2CInterface_SwitchOffHardware(void)
{
	MCUAXESS_I2CCOMIF__SwitchOffHardware();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Sets the I2C start condition.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__I2CInterface_SetStartCondition(void)
{
	MCUAXESS_I2CCOMIF__SetStartCondition();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Sets the I2C stop condition.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__I2CInterface_SetStopCondition(void)
{
	MCUAXESS_I2CCOMIF__SetStopCondition();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Sets the I2C acknowledge condition.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__I2CInterface_SetAcknowledge(void)
{
	MCUAXESS_I2CCOMIF__SetAcknowledge();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Clears the I2C start condition.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__I2CInterface_ClrAcknowledge(void)
{
	MCUAXESS_I2CCOMIF__ClrAcknowledge();
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Transmits a byte over the I2C bus.
 * @param:	vucByte - The byte to transmit over the bus.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__I2CInterface_TxByte(uint8_t vucByte)
{
	MCUAXESS_I2CCOMIF__TxByte( vucByte );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Receives a byte over the I2C bus.
 * @param:	None.
 * @retval:	The byte received.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t							MCUAXESS__I2CInterface_ucByteRxed(void)
{
	return MCUAXESS_I2CCOMIF__ucByteRxed();
}
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Thermistor Temperature Measurement Access - mcuaxess_tempmsif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include		"mcuaxess_tempmsif.h"
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function switches power on/off to the thermistor measurement circuit.
 * @param:	voState	- The state (ON/OFF) to switch to the thermistor circuit.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TemperatureMeasurement_SwitchOnOff(onoff voState)
{
	MCUAXESS_TEMPMSIF__SwitchOnOff( voState );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function configures the I/O and peripherals used to measure thermistor temperature via the on board AD converter.
 * @param:	vbCalibrateAD - Flag that indicates whether ot not to perform self calibration of the AD
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__TemperatureMeasurement_EnableADC(void)
{
	MCUAXESS_TEMPMSIF__EnableADC( true );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Returns flag indicating when AD calibration done.
 * @param:	None.
 * @retval:	bool.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS__TemperatureMeasurement_bADCalibrationDone(void)
{
	return MCUAXESS_TEMPMSIF__bADCalibrationDone();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function initiates an AD conversion to measure thermistor temperature returning the result as a 16 bit value.
 * @param:	None.
 * @retval:	The converted result of AD channel representing the thermistor temperature.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint16_t						MCUAXESS__TemperatureMeasurement_uiADCResult(void)
{
	return MCUAXESS_TEMPMSIF__uiADCResult();
}
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	RTC Access - mcuaxess_rtmclkif.c
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include		"mcuaxess_rtmclkif.h"
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function configures the RTC peripheral for real time clock keeping if it is detected to have failed due to power loss on the Vbat pin.  If the RTC has failed true
 * 			is returned to indicate this.
 * @param:	None.
 * @retval:	A boolean flag indicating whether or not RTC failure was detected.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS__RealTimeClock_bFailureDetectedOnConfigurationUpdate(void)
{
	return MCUAXESS_RTMCLKIF__bRTCFailureDetectedOnConfigurationUpdate();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function returns the counter value used for RTC time keeping.
 * @param:	None.
 * @retval:	The 32-bit value representing the RTC time keepin couter value.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
uint32_t						MCUAXESS__RealTimeClock_ulCounterRegister(void)
{
	return MCUAXESS_RTMCLKIF__ulRTCCounterRegister();
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function sets the counter value used for RTC time keeping with the value passed.
 * @param:	vulNewCounterValue - The new value to be loaded into the counter used for RTC time keeping.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS__RealTimeClock_SetCounterRegister(uint32_t vulNewCounterValue)
{
	MCUAXESS_RTMCLKIF__SetRTCCounterRegister(vulNewCounterValue);
	return;
}
//*********************************************************************************************************************************************************************************



