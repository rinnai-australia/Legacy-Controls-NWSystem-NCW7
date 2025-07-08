//*********************************************************************************************************************************************************************************
// mcuaxess_tftdrvif.c
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
#include		<mcuaxess_tftdrvif.h>
#include		<mcuaxess_def.h>

//*********************************************************************************************************************************************************************************
// MODULE
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	MCUAXESS_TFTDRVIF__FT800Device {			// FT800 Device Details
															//
	bool		bOperateInFastMode;							// - Operate in fast mode flag
	bool		bSelected;									// - Device selected flag
															//
} MCUAXESS_TFTDRVIF__FT800Device;							//
															//
static MCUAXESS_TFTDRVIF__FT800Device	mFT800Device;		// Container
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
 * @brief:	Initialises the hardware I/O for interfacing the FT800 device to drive a TFT LCD.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS_TFTDRVIF__FT800_InterfaceInit(void)
{
	// Start with hi priority always
	MCUAXESS_TFTDRVIF__FT800_SPIPrioritySetHi();
	// Operate in slow mode
	mFT800Device.bOperateInFastMode = false;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	These functions allow the SPI interrupt priority level to be set (hi/lo).
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS_TFTDRVIF__FT800_SPIPrioritySetLo(void)
{
	NVIC_SetPriority( SPI1_IRQn, MCUAXESS_DEF__IP_SPI1_LO );
	return;
}
void							MCUAXESS_TFTDRVIF__FT800_SPIPrioritySetHi(void)
{
	NVIC_SetPriority( SPI1_IRQn, MCUAXESS_DEF__IP_SPI1_HI );
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function initialises transfer over SPI to the FT800 device.
 * @param:	vucByte - First byte to transmit.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS_TFTDRVIF__FT800_SPIInitTransfer(uint8_t vucByte)
{
	uint32_t		ulTempRegister;
	//
	// Reset the SPI1 (APB2) peripheral
	// - Reset the peripheral
	RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST; RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	RCC->APB2RSTR |=  RCC_APB2RSTR_AFIORST; RCC->APB2RSTR &= ~RCC_APB2RSTR_AFIORST;
	// - Disable the peripheral
	SPI1->CR1 = 0x00; SPI1->CR2 = 0x00;
	//
	// Enable the peripheral clock - SPI1 (APB2)
	RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN; RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB2ENR &= ~RCC_APB2ENR_AFIOEN; RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// FT800 SPI Interface I/O - GPIOA.5 SCK
	//						     GPIOA.6 MISO
	//						 	 GPIOA.7 MOSI
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// GPIOA.5 Configuration: - Configure for alternate function PP mode @2MHz
	// - Get current state of CRL register
	ulTempRegister = GPIOA->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5); ulTempRegister |= (GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5_1);
	// - Set current state of CRL register
	GPIOA->CRL = ulTempRegister;
	//
	// GPIOA.6 Configuration: - Configure for floating input mode
	// - Get current state of CRL register
	ulTempRegister = GPIOA->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6); ulTempRegister |= (GPIO_CRL_CNF6_0);
	// - Set current state of CRL register
	GPIOA->CRL = ulTempRegister;
	//
	// GPIOA.7 Configuration: - Configure for alternate function PP mode @2MHz
	// - Get current state of CRL register
	ulTempRegister = GPIOA->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7); ulTempRegister |= (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1);
	// - Set current state of CRL register
	GPIOA->CRL = ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// FT800 SPI Peripheral - Initialised when used
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Clear all control register bits
	SPI1->CR1 = 0x00; SPI1->CR2 = 0x00;
	// Mode 0 (CPHA = 0, CPOL = 0)
	SPI1->CR1 &= ~SPI_CR1_CPOL; SPI1->CR1 &= ~SPI_CR1_CPHA;
	// Master operation
	SPI1->CR1 |= SPI_CR1_MSTR;
	// PCLK2=72MHz
	if ( mFT800Device.bOperateInFastMode )
	{
		SPI1->CR1 |= SPI_CR1_BR_1;		// PCLK2/16 = 4.5MHz
		SPI1->CR1 |= SPI_CR1_BR_0;		//
	}
	else
	{
		SPI1->CR1 |= SPI_CR1_BR_1;		// PCLK2/16 = 4.5MHz
		SPI1->CR1 |= SPI_CR1_BR_0;		//
	}
	//
	// Interrupt Control
	// - Interrupt controller update
	NVIC_EnableIRQ( SPI1_IRQn );
	//
	// Select the device
	MCUAXESS_TFTDRVIF__FT800_CS(IO_PIN_RST);
	// Enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
	// Clear buffer
	uint8_t	ucValue = SPI1->DR;
	// Begin Transmit and enable interrupts
	SPI1->DR = vucByte;	SPI1->CR2 |= ( SPI_CR2_RXNEIE );

	(void)ucValue;

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	This function stops any SPI activity.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS_TFTDRVIF__FT800_SPIStopTransfer(void)
{
	// Make sure any SPI is completed
	while ( ( SPI1->SR & SPI_SR_TXE ) == 0 ) { ; }
	while ( ( SPI1->SR & SPI_SR_BSY ) == 1 ) { ; }
	//
	// Disable the peripheral clock - SPI1 (APB2)
	RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
	// Disable SPI and interrupts
	SPI1->CR1 &= ~SPI_CR1_SPE; SPI1->CR2 &= ~(SPI_CR2_TXEIE | SPI_CR2_RXNEIE);
	// Deselect the device
	MCUAXESS_TFTDRVIF__FT800_CS( IO_PIN_SET );

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Functions that allow operating the FT800 SPI in fast/slow mode.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS_TFTDRVIF__FT800_OperateInFastMode(void)
{
	mFT800Device.bOperateInFastMode = true;
	return;
}
void							MCUAXESS_TFTDRVIF__FT800_OperateInSlowMode(void)
{
	mFT800Device.bOperateInFastMode = false;
	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates whether or not the FT800 is currently chip selected.
 * @param:	None.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS_TFTDRVIF__FT800_bDeviceSelected(void)
{
	return mFT800Device.bSelected;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows selection/deselection of the FT800 device.
 * @param:	vIOPin - Flag indicating whether to reset/set the CS pin.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS_TFTDRVIF__FT800_CS(IO_PIN vIOPin)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// CS# line : GPIOC.4 Configuration: - Configure for output PP mode @50MHz
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRL register
	ulTempRegister = GPIOC->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4); ulTempRegister |= (GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0);
	// - Set current state of CRL register
	GPIOC->CRL = ulTempRegister;
	//
	// Now to select/de-select
	if ( vIOPin == IO_PIN_RST )
	{
		GPIOC->ODR &= ~(uint32_t)GPIO_ODR_ODR4;		// Pull lo to select

		mFT800Device.bSelected = true;
	}
	else
	{
		GPIOC->ODR |=  (uint32_t)GPIO_ODR_ODR4;		// Pull hu to de-select

		mFT800Device.bSelected = false;
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Allows power-down/power-up of the FT800 device.
 * @param:	vIOPin - Flag indicating whether to reset/set the PD pin.
 * @retval:	None.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void							MCUAXESS_TFTDRVIF__FT800_PD(IO_PIN vIOPin)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// PD# line : GPIOC.5 Configuration: - Configure for output PP mode @50MHz
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// - Get current state of CRL register
	ulTempRegister = GPIOC->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5); ulTempRegister |= (GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0);
	// - Set current state of CRL register
	GPIOC->CRL = ulTempRegister;
	//
	// Now to select/de-select
	if ( vIOPin == IO_PIN_RST )
	{
		GPIOC->ODR &= ~(uint32_t)GPIO_ODR_ODR5;		// Pull lo to select
	}
	else
	{
		GPIOC->ODR |=  (uint32_t)GPIO_ODR_ODR5;		// Pull hu to de-select
	}

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * @brief:	Indicates when the INTn pin from FT800 is high.
 * @param:	None.
 * @retval:	bool - true when hi.
 *-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool							MCUAXESS_TFTDRVIF__FT800_bINTPinHi(void)
{
	uint32_t	ulTempRegister;
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// FT800 INT line - GPIOA.4
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// a) Configure for input mode (no pullup as externally provided)
	// - Get current state of CRL register
	ulTempRegister = GPIOA->CRL;
	// - Clear then set corresponding bits of temporary register
	ulTempRegister &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4); ulTempRegister |= GPIO_CRL_CNF4_0;
	// - Set current state of CRL register
	GPIOA->CRL = ulTempRegister;

	if ( ( GPIOA->IDR & GPIO_IDR_IDR4 ) != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
//*********************************************************************************************************************************************************************************

