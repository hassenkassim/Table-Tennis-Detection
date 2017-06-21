//#include "adc.h"
//#include "uart.h"
//
//
////static uint32_t adcNoiseA;
////static uint32_t adcNoiseB;
////static uint32_t adcNoiseC;
//
//
//#define ADC_CHANNELS	3
//#define ADC_SAMPLES		200
//uint32_t DMA_32bit_Channel_Buffer[90];
////volatile uint32_t DMA_32bit_Channel_Buffer[ADC_SAMPLES];
//volatile bool dmaTransferActive;
//
//DMA_CB_TypeDef cb;
//
////#define SAMPLES 100
////static uint32_t bufferA[SAMPLES];
////static uint32_t bufferB[SAMPLES];
////static uint32_t bufferC[SAMPLES];
////
////static uint32_t tab;
////static uint32_t tac;
////static uint32_t tbc;
//
//
///**************************************************************************//**
// * @brief
// *    Prototypes.
// *****************************************************************************/
////static void Transmit_Noise(void);
////static void Transmit_Signals(void);
////static void Transmit_StartSignal(void);
////
////static void GetTimeDifferences(void);
//
//
///**************************************************************************//**
// * @brief
// *    Interrupts.
// *****************************************************************************/
//void ADC0_IRQHandler(void) {
//	/* Clear interrupt flag */
//	ADC_IntClear(ADC0, ADC_IFC_SCANOF);
//	/* Indicate ADC scan overflow using LED on PE2 */
//	GPIO_PinOutSet(gpioPortE, 2);
//}
//
//
///**************************************************************************//**
// * @brief
// *    Callbacks.
// *****************************************************************************/
//void dmaTransferComplete(unsigned int channel, bool primary, void *user) {
//	GPIO_PinOutSet(gpioPortE, 3);
//	(void) channel;
//	(void) primary;
//	(void) user;
//	/* Stopping ADC by stopping TIMER0 */
//	TIMER_Enable(TIMER0, false);
//	/* Clearing flag to indicate that transfer is complete */
//	dmaTransferActive = false;
//}
//
//
///***************************************************************************//**
//* @brief ADC_ConfigScanConversion(uint8_t res)
//*    Configures ADC usage.
//*******************************************************************************/
//void ADC_ConfigScanConversion(uint8_t res, uint32_t freq) {
//
//	/* Initialize necessary clocks */
//	CMU_ClockEnable(cmuClock_ADC0, true);
//	CMU_ClockEnable(cmuClock_PRS, true);
//	CMU_ClockEnable(cmuClock_GPIO, true);
//	CMU_ClockEnable(cmuClock_TIMER0, true);
//	CMU_ClockEnable(cmuClock_DMA, true);
//
//	/* DMA callback function */
//	cb.cbFunc = dmaTransferComplete;
//	cb.userPtr = NULL;
//
//	/* DMA initialization structure */
//	DMA_Init_TypeDef dmaInit;
//	dmaInit.hprot = 0;
//	dmaInit.controlBlock = dmaControlBlock;
//	DMA_Init(&dmaInit);
//
//	/* DMA channel configuration structure */
//	DMA_CfgChannel_TypeDef chnlCfg;
//	chnlCfg.highPri = false;
//	chnlCfg.enableInt = false;
//	chnlCfg.select = DMAREQ_ADC0_SCAN;
//	chnlCfg.cb = &cb;
//	DMA_CfgChannel(ADC_DMA_CHANNEL, &chnlCfg);
//
//	/* DMA description structure */
//	DMA_CfgDescr_TypeDef descrCfg;
//	descrCfg.srcInc = dmaDataIncNone;
//	descrCfg.arbRate = dmaArbitrate1;
//	descrCfg.dstInc = dmaDataInc4;
//	descrCfg.size = dmaDataSize4;
//	descrCfg.hprot = 0;
//	DMA_CfgDescr(ADC_DMA_CHANNEL, true, &descrCfg);
//
//	/* Start DMA transfer */
//	DMA_ActivateBasic(ADC_DMA_CHANNEL,
//					  true,
//					  false,
//					  DMA_32bit_Channel_Buffer,
//					  (void *)((uint32_t)&(ADC0->SCANDATA)),
//					  (90 - 1));
//	dmaTransferActive = true;
//
//	/* Initialize general ADC */
//	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
//	init.lpfMode = adcLPFilterBypass;
//	init.warmUpMode = adcWarmupKeepADCWarm;
//	init.timebase = ADC_TimebaseCalc(0);
//	init.prescale = ADC_PrescaleCalc(7000000UL, 0);
//	ADC_Init(ADC0, &init);
//
//	/* Initialize ADC scan mode */
//	ADC_InitScan_TypeDef scanInit = ADC_INITSCAN_DEFAULT;
//	scanInit.acqTime = adcAcqTime1;
//	scanInit.reference = adcRefVDD;
//	scanInit.prsEnable = true;
//	scanInit.prsSel = adcPRSSELCh0;
//	if (res == 8) {
//		scanInit.resolution = adcRes8Bit;
//	} else if (res == 6) {
//		scanInit.resolution = adcRes6Bit;
//	} else {
//		scanInit.resolution = adcRes12Bit;
//	}
//	scanInit.input = ADC_SCANCTRL_INPUTMASK_CH5 |
//					 ADC_SCANCTRL_INPUTMASK_CH6 |
//					 ADC_SCANCTRL_INPUTMASK_CH7;
//	ADC_InitScan(ADC0, &scanInit);
//
//	/* Enable ADC scan overflow interrupt to indicate lost samples */
//	GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);
//	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
//	ADC_IntEnable(ADC0, ADC_IEN_SCANOF);
//	NVIC_EnableIRQ(ADC0_IRQn);
//
//	/* Connect PRS channel 0 to TIMER0 overflow */
//	//PRS_SourceSignalSet(ADC_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgeOff);
//	/* Initialize TIMER0 */
//	TIMER_TopSet(TIMER0,  CMU_ClockFreqGet(cmuClock_TIMER0)/freq);
//	TIMER_Enable(TIMER0, true);
//
//	/* PRS pulse configuration */
//	PRS_SourceSignalSet(2, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgeOff);
//	GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);
//	PRS->ROUTE = PRS_ROUTE_CH2PEN | PRS_ROUTE_LOCATION_LOC0;
//}
//
//
///***************************************************************************//**
//* @brief ADC_GetNoise(void)
//*    Get noise of every channel.
//*******************************************************************************/
////void ADC_GetNoise(void) {
////
////		  uint32_t i;
////	const uint32_t cnt = 50;
////		  uint32_t a = 0;
////		  uint32_t b = 0;
////		  uint32_t c = 0;
////
////	/* Configure ADC */
////	ADC_ConfigScanConversion(12, 200000);
////
////	/* Warm up */
////	for(i = 0; i < cnt; i++) {
////		DMA_ActivateBasic(ADC_DMA_CHANNEL, true, false, DMA_32bit_Channel_Buffer, (void *)((uint32_t) &(ADC0->SCANDATA)), (ADC_CHANNELS - 1));
////		ADC_Start(ADC0, adcStartScan);
////		while (ADC0->STATUS & ADC_STATUS_SCANACT);
////	}
////
////	/* Get data */
////	for(i = 0; i < cnt; i++) {
////		DMA_ActivateBasic(ADC_DMA_CHANNEL, true, false, DMA_32bit_Channel_Buffer, (void *)((uint32_t) &(ADC0->SCANDATA)), (ADC_CHANNELS - 1));
////		ADC_Start(ADC0, adcStartScan);
////		while (ADC0->STATUS & ADC_STATUS_SCANACT);
////		a = a + DMA_32bit_Channel_Buffer[0];
////		b = b + DMA_32bit_Channel_Buffer[1];
////		c = c + DMA_32bit_Channel_Buffer[2];
////	}
////
////	/* Average data */
////	adcNoiseA = a / cnt;
////	adcNoiseB = b / cnt;
////	adcNoiseC = c / cnt;
////}
//
//
///***************************************************************************//**
//* @brief ADC_GetSignals(void)
//*    Get data signal of every channel after any impact.
//*******************************************************************************/
////void ADC_ScanSignals(void) {
////
////		  uint32_t i;
////	const uint32_t cnt = 50;
////		  bool	   impactA = false;
////		  bool	   impactB = false;
////		  bool	   impactC = false;
////
////	/* Configure ADC */
////	ADC_ConfigScanConversion(12, 200000);
////
////	/* Warm up */
////	for(i = 0; i < cnt; i++) {
////		DMA_ActivateBasic(ADC_DMA_CHANNEL, true, false, DMA_32bit_Channel_Buffer, (void *)((uint32_t) &(ADC0->SCANDATA)), (ADC_CHANNELS - 1));
////		ADC_Start(ADC0, adcStartScan);
////		while (ADC0->STATUS & ADC_STATUS_SCANACT);
////	}
////
////	GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);
////
////	uint32_t ah = adcNoiseA + ADC_TH_A;
////	uint32_t al = adcNoiseA - ADC_TH_A;
////	uint32_t bh = adcNoiseB + ADC_TH_B;
////	uint32_t bl = adcNoiseB - ADC_TH_B;
////	uint32_t ch = adcNoiseC + ADC_TH_C;
////	uint32_t cl = adcNoiseC - ADC_TH_C;
////
////	while(1) {
////
////		/* Get samples */
////		DMA_ActivateBasic(ADC_DMA_CHANNEL, true, false, DMA_32bit_Channel_Buffer, (void *)((uint32_t) &(ADC0->SCANDATA)), (ADC_CHANNELS - 1));
////		ADC_Start(ADC0, adcStartScan);
////		while (ADC0->STATUS & ADC_STATUS_SCANACT);
////
//////		/* Toggle pin for debug */
//////		GPIO_PinOutToggle(gpioPortC, 10);
//////
//////		/* Impact thresholding decision */
//////		impactA = (DMA_32bit_Channel_Buffer[0] > ah) || (DMA_32bit_Channel_Buffer[0] < al);
//////		impactB = (DMA_32bit_Channel_Buffer[1] > bh) || (DMA_32bit_Channel_Buffer[1] < bl);
//////		impactC = (DMA_32bit_Channel_Buffer[1] > ch) || (DMA_32bit_Channel_Buffer[1] < cl);
//////
//////		/* Save signal shapes */
//////		if(impactA | impactB | impactC) {
//////
//////			for(i = 0; i < SAMPLES; i++) {
//////				DMA_ActivateBasic(ADC_DMA_CHANNEL, true, false, DMA_32bit_Channel_Buffer, (void *)((uint32_t) &(ADC0->SCANDATA)), (ADC_CHANNELS - 1));
//////				ADC_Start(ADC0, adcStartScan);
//////				while (ADC0->STATUS & ADC_STATUS_SCANACT);
//////				bufferA[i] = DMA_32bit_Channel_Buffer[0];
//////				bufferB[i] = DMA_32bit_Channel_Buffer[1];
//////				bufferC[i] = DMA_32bit_Channel_Buffer[2];
//////			}
//////
//////			GetTimeDifferences();
//////			/* Done */
//////			break;
//////
//////		}
////
////	}
////
////}
//
//
///**************************************************************************//**
// * @brief GetTimeDifferences(void)
// *    Calculate time differences
// *****************************************************************************/
////static void GetTimeDifferences(void) {
////
////	uint32_t ta = 0;
////	uint32_t tb = 0;
////	uint32_t tc = 0;
////	uint32_t i;
////
////	for(i = 0; i < SAMPLES; i++) {
////		if ((bufferA[i] > (adcNoiseA + ADC_TH_A)) || (bufferA[i] < (adcNoiseA - ADC_TH_A))) {
////			break;
////		}
////		ta++;
////	}
////	for(i = 0; i < SAMPLES; i++) {
////		if ((bufferB[i] > (adcNoiseB + ADC_TH_B)) || (bufferB[i] < (adcNoiseB - ADC_TH_B))) {
////			break;
////		}
////		tb++;
////	}
////	for(i = 0; i < SAMPLES; i++) {
////		if ((bufferC[i] > (adcNoiseC + ADC_TH_C)) || (bufferC[i] < (adcNoiseC - ADC_TH_C))) {
////			break;
////		}
////		tc++;
////	}
////
////	tab = ta - tb;
////	tac = ta - tc;
////	tbc = tb - tc;
////}
//
//
///**************************************************************************//**
// * @brief Transmit_Noise(void)
// *    Transmit noise of all ADC channels over UART
// *****************************************************************************/
////static void Transmit_Noise(void) {
////	LEUART_Write32bit(adcNoiseA);
////	LEUART_Write32bit(adcNoiseB);
////	LEUART_Write32bit(adcNoiseC);
////}
//
//
///**************************************************************************//**
// * @brief Transmit_StartSignal(void)
// *    Transmit start signal for communication over UART
// *****************************************************************************/
////static void Transmit_StartSignal(void) {
////	LEUART_Write32bit(0xFFFFFFFF);
////}
//
//
///**************************************************************************//**
// * @brief Transmit_Noise(void)
// *    Transmit noise of all ADC channels over UART
// *****************************************************************************/
////static void Transmit_Signals(void) {
////	uint32_t i;
////	for(i = 0; i < SAMPLES; i++) {
////		LEUART_Write32bit(bufferA[i]);
////		LEUART_Write32bit(bufferB[i]);
////		LEUART_Write32bit(bufferC[i]);
////	}
////}
