#include "spi.h"
#include "uart.h"


//static uint32_t spiNoiseA;
//static uint32_t spiNoiseB;
//static uint32_t spiNoiseC;
//
//uint32_t data[3];


/***************************************************************************//**
* @brief SPI_Config(void)
*    Configures SPI usage.
*******************************************************************************/
//static void SPI_Config(void) {
//
//	uint32_t spiSpeed = 14000000;
//
//	CMU_ClockEnable(cmuClock_USART0, true);
//	CMU_ClockEnable(cmuClock_USART1, true);
//	CMU_ClockEnable(cmuClock_USART2, true);
//	CMU_ClockEnable(cmuClock_GPIO, true);
//	CMU_ClockEnable(cmuClock_PRS, true);
//
//    GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 0);  	 /* DataIn  */
//    GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull, 0); /* Cs     */
//    GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull, 0); /* Clock   */
//
//	GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);  	 /* DataIn  */
//	GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0);  /* Cs      */
//	GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0);  /* Clock   */
//
//	GPIO_PinModeSet(gpioPortC, 3, gpioModeInput, 0);  	 /* DataIn  */
//	GPIO_PinModeSet(gpioPortC, 5, gpioModePushPull, 0);  /* Cs      */
//	GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 0);  /* Clock   */
//
//	USART_InitSync_TypeDef init0 = USART_INITSYNC_DEFAULT;
//	init0.enable = usartDisable;
//	init0.refFreq = 0;
//	init0.baudrate = spiSpeed;
//	init0.databits = usartDatabits14;
//	init0.master = true;
//	init0.msbf = true;
//	init0.clockMode = usartClockMode2;
//	USART_InitSync(USART0, &init0);
//    USART_TypeDef *spi0 = USART0;
//    spi0->CTRL |= USART_CTRL_AUTOCS;
//	spi0->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_LOCATION_LOC0;
//	USART_PrsTriggerInit_TypeDef prs0 = USART_INITPRSTRIGGER_DEFAULT;
//	prs0.txTriggerEnable = true;
//	prs0.prsTriggerChannel = usartPrsTriggerCh7;
//	USART_InitPrsTrigger(spi0, &prs0);
//	USART_Enable(spi0, usartEnable);
//
//
//	USART_InitSync_TypeDef init1 = USART_INITSYNC_DEFAULT;
//	init1.enable = usartDisable;
//	init1.refFreq = 0;
//	init1.baudrate = spiSpeed;
//	init1.databits = usartDatabits14;
//	init1.master = true;
//	init1.msbf = true;
//	init1.clockMode = usartClockMode2;
//	USART_InitSync(USART1, &init1);
//    USART_TypeDef *spi1 = USART1;
//    spi1->CTRL |= USART_CTRL_AUTOCS;
//	spi1->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_LOCATION_LOC1;
//	USART_PrsTriggerInit_TypeDef prs1 = USART_INITPRSTRIGGER_DEFAULT;
//	prs1.txTriggerEnable = true;
//	prs1.prsTriggerChannel = usartPrsTriggerCh7;
//	USART_InitPrsTrigger(spi1, &prs1);
//	USART_Enable(spi1, usartEnable);
//
//
//	USART_InitSync_TypeDef init2 = USART_INITSYNC_DEFAULT;
//	init2.enable = usartDisable;
//	init2.refFreq = 0;
//	init2.baudrate = spiSpeed;
//	init2.databits = usartDatabits14;
//	init2.master = true;
//	init2.msbf = true;
//	init2.clockMode = usartClockMode2;
//	USART_InitSync(USART2, &init2);
//    USART_TypeDef *spi2 = USART2;
//    spi2->CTRL |= USART_CTRL_AUTOCS;
//	spi2->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_LOCATION_LOC0;
//	USART_PrsTriggerInit_TypeDef prs2 = USART_INITPRSTRIGGER_DEFAULT;
//	prs2.txTriggerEnable = true;
//	prs2.prsTriggerChannel = usartPrsTriggerCh7;
//	USART_InitPrsTrigger(spi2, &prs2);
//	USART_Enable(spi2, usartEnable);
//
//}


/***************************************************************************//**
* @brief SPI_GetNoise(void)
*    Get noise of every channel.
*******************************************************************************/
//static void SPI_GetNoise(void) {
//
//}


/***************************************************************************//**
* @brief SPI_ScanSignals(void)
*    Get data signal of every channel after any impact.
*******************************************************************************/
//static void SPI_ScanSignals(void) {
//	//DMA_ActivateBasic(0, true, false, data, (void *)((uint32_t) &(USART1->RXDOUBLE)), (3 - 1));
//
//	bool impactA = false;
//	bool impactB = false;
//	bool impactC = false;
//
//	uint32_t ah = spiNoiseA + SPI_TH_A;
//	uint32_t al = spiNoiseA - SPI_TH_A;
//	uint32_t bh = spiNoiseB + SPI_TH_B;
//	uint32_t bl = spiNoiseB - SPI_TH_B;
//	uint32_t ch = spiNoiseC + SPI_TH_C;
//	uint32_t cl = spiNoiseC - SPI_TH_C;
//
//	/* Data acquisition */
//	while(true) {
//
//		PRS_PulseTrigger(PRS_SWPULSE_CH7PULSE);
//
////		USART_TxDouble(USART0, 0x00);
////		USART_TxDouble(USART1, 0x00);
////		USART_TxDouble(USART2, 0x00);
//		data[1] = USART_RxDouble(USART0);
//		data[1] = USART_RxDouble(USART1);
//		data[2] = USART_RxDouble(USART2);
//
//		/* Impact thresholding decision */
//		impactA = (data[0] > ah) || (data[0] < al);
//		impactB = (data[1] > bh) || (data[1] < bl);
//		impactC = (data[2] > ch) || (data[2] < cl);
//
//		if(impactA | impactB | impactC) {
//
//		}
//
//	}
//
//
//}


/***************************************************************************//**
* @brief SPI_GetSensorData(void)
*    Perform low level data transmission.
*******************************************************************************/
//static void SPI_GetSensorData(void) {

//	uint32_t dataA = 0;
//	uint32_t dataB = 0;
//	uint32_t dataC = 0;
//	uint32_t  temp = 0;
//
//	/* Initiate transmission */
//	SPI_CSa_LOW;
//	SPI_CSb_LOW;
//	SPI_CSc_LOW;
//	SPI_CLK_LOW;
//	temp = SPI_DATAa_IN;
//	temp = SPI_DATAb_IN;
//	temp = SPI_DATAc_IN;
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	temp = SPI_DATAa_IN;
//	temp = SPI_DATAb_IN;
//	temp = SPI_DATAc_IN;
//	SPI_CLK_HIGH;
//
//	/* Get data */
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 11);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 11);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 11);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 10);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 10);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 10);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 9);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 9);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 9);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 8);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 8);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 8);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 7);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 7);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 7);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 6);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 6);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 6);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 5);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 5);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 5);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 4);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 4);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 4);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 3);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 3);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 3);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 2);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 2);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 2);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 1);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 1);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 1);
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	dataA |= ((uint32_t)SPI_DATAa_IN << 0);
//	dataB |= ((uint32_t)SPI_DATAb_IN << 0);
//	dataC |= ((uint32_t)SPI_DATAc_IN << 0);
//	SPI_CLK_HIGH;
//
//	/* Terminate transmission */
//	SPI_CLK_LOW;
//	temp = SPI_DATAa_IN;
//	temp = SPI_DATAb_IN;
//	temp = SPI_DATAc_IN;
//	SPI_CLK_HIGH;
//	SPI_CLK_LOW;
//	temp = SPI_DATAa_IN;
//	temp = SPI_DATAb_IN;
//	temp = SPI_DATAc_IN;
//	SPI_CLK_HIGH;
//	SPI_CSa_HIGH;
//	SPI_CSb_HIGH;
//	SPI_CSc_HIGH;
//
//	DMA_32bit_Channel_Buffer[0] = dataA;
//	DMA_32bit_Channel_Buffer[1] = dataB;
//	DMA_32bit_Channel_Buffer[2] = dataC;
//
//}
