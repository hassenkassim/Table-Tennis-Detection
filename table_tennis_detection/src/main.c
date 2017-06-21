//#define ARM_MATH_CM3

/* C Libraries */
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* emf Libraries */
#include "em_cmu.h"
#include "em_adc.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_timer.h"
#include "em_int.h"
#include "em_device.h"
#include "em_chip.h"

/* Custom Libraries */
#include "data.h"
#include "fix16.h"
#include "main.h"

#include "bsp_trace.h"
#include "math_functions.h"


/* Definitions */


#define ADC_PRS_CHANNEL 	0
#define ADC_DMA_CHANNEL 	0
#define TIMER_ADC_CHANNEL	1
#define ADC_PINGPONG_TRANSFERS 100000
#define WARM_UP_LOOPS 2

#define ADC_TH_A			200
#define ADC_TH_B			200
#define ADC_TH_C			200

#define ADC_CHANNELS	3
#define ADC_SAMPLES		900
uint16_t dma16bitBuffer0[ADC_SAMPLES];
uint16_t dma16bitBuffer1[ADC_SAMPLES];
uint8_t dmaBufferIndex;

volatile bool dmaTransferActive;
volatile uint32_t dmaTransferNumber;

DMA_CB_TypeDef cb;

static volatile uint32_t adcNoiseA;
static volatile uint32_t adcNoiseB;
static volatile uint32_t adcNoiseC;
static volatile uint32_t ah;
static volatile uint32_t al;
static volatile uint32_t bh;
static volatile uint32_t bl;
static volatile uint32_t ch;
static volatile uint32_t cl;
static volatile bool impactA;
static volatile bool impactB;
static volatile bool impactC;
static volatile bool impact;

int _write(int file, const char *ptr, int len)
{
    int x;
    for (x = 0; x < len; x++)
    ITM_SendChar (*ptr++);
    return (len);
}

void ADC0_IRQHandler(void) {
	/* Clear interrupt flag */
	ADC_IntClear(ADC0, ADC_IFC_SCANOF);
	/* Indicate ADC scan overflow using LED on PE2 */
	//GPIO_PinOutSet(gpioPortE, 2);
}

void SWO_SetupForPrint(void) {
    /* Enable GPIO clock. */
    CMU ->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
    /* Enable Serial wire output pin */
    GPIO ->ROUTE |= GPIO_ROUTE_SWOPEN;
    #if defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_LEOPARD_FAMILY) ||         defined(_EFM32_WONDER_FAMILY) || defined(_EFM32_GECKO_FAMILY)
        /* Set location 0 */
        GPIO ->ROUTE = (GPIO ->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) |     GPIO_ROUTE_SWLOCATION_LOC0;
        /* Enable output on pin - GPIO Port F, Pin 2 */
        GPIO ->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
        GPIO ->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;
    #else
        /* Set location 1 */
        GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK))         |GPIO_ROUTE_SWLOCATION_LOC1;
        /* Enable output on pin */
        GPIO->P[2].MODEH &= ~(_GPIO_P_MODEH_MODE15_MASK);
        GPIO->P[2].MODEH |= GPIO_P_MODEH_MODE15_PUSHPULL;
    #endif
    /* Enable debug clock AUXHFRCO */
    CMU ->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;
    /* Wait until clock is ready */
    while (!(CMU ->STATUS & CMU_STATUS_AUXHFRCORDY)) ;
    /* Enable trace in core debug */
    CoreDebug ->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    ITM ->LAR = 0xC5ACCE55;
    ITM ->TER = 0x0;
    ITM ->TCR = 0x0;
    TPI ->SPPR = 2;
    TPI ->ACPR = 0xf;
    ITM ->TPR = 0x0;
    DWT ->CTRL = 0x400003FE;
    ITM ->TCR = 0x0001000D;
    TPI ->FFCR = 0x00000100;
    ITM ->TER = 0x1;
}

/***************************************************************************//**
* @brief ADC_ConfigScanConversion(uint8_t res)
*    Configures ADC usage.
*******************************************************************************/
void ADC_ConfigScanConversion(uint8_t res) {

	/* Initialize necessary clocks */
	CMU_ClockEnable(cmuClock_ADC0, true);
	CMU_ClockEnable(cmuClock_PRS, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_DMA, true);

	DMA_Reset();

	/* DMA callback function */
	cb.cbFunc = dmaTransferComplete;
	cb.userPtr = NULL;

	/* DMA initialization structure */
	DMA_Init_TypeDef dmaInit;
	dmaInit.hprot = 0;
	dmaInit.controlBlock = dmaControlBlock;
	DMA_Init(&dmaInit);

	/* DMA channel configuration structure */
	DMA_CfgChannel_TypeDef chnlCfg;
	chnlCfg.highPri = false;
	chnlCfg.enableInt = true;
	chnlCfg.select = DMAREQ_ADC0_SCAN;
	chnlCfg.cb = &cb;
	DMA_CfgChannel(ADC_DMA_CHANNEL, &chnlCfg);

	/* DMA description structure */
	DMA_CfgDescr_TypeDef descrCfg;
	descrCfg.srcInc = dmaDataIncNone;
	descrCfg.arbRate = dmaArbitrate1;
	descrCfg.dstInc = dmaDataInc2;
	descrCfg.size = dmaDataSize2;
	descrCfg.hprot = 0;
	DMA_CfgDescr(ADC_DMA_CHANNEL, true, &descrCfg);
	DMA_CfgDescr(ADC_DMA_CHANNEL, false, &descrCfg);

	/* Start DMA transfer */
	DMA_ActivatePingPong(ADC_DMA_CHANNEL,
					  	 false,
					  	 (void *)dma16bitBuffer0,
					  	 (void *)&(ADC0->SCANDATA),
					  	 ADC_SAMPLES - 1,
					  	 (void *)dma16bitBuffer1,
					  	 (void *)&(ADC0->SCANDATA),
					  	 ADC_SAMPLES - 1);

	/* Initialize general ADC */
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	init.lpfMode = adcLPFilterBypass;
	init.warmUpMode = adcWarmupKeepADCWarm;
	init.timebase = ADC_TimebaseCalc(0);
	init.prescale = ADC_PrescaleCalc(13000000UL, 0);
	ADC_Init(ADC0, &init);

	/* Initialize ADC scan mode */
	ADC_InitScan_TypeDef scanInit = ADC_INITSCAN_DEFAULT;
	scanInit.acqTime = adcAcqTime1;
	scanInit.reference = adcRefVDD;
	//scanInit.prsEnable = true;
	scanInit.rep = true;
	//scanInit.prsSel = adcPRSSELCh0;
	if (res == 8) {
		scanInit.resolution = adcRes8Bit;
	} else if (res == 6) {
		scanInit.resolution = adcRes6Bit;
	} else {
		scanInit.resolution = adcRes12Bit;
	}
	scanInit.input = ADC_SCANCTRL_INPUTMASK_CH4 |
					 ADC_SCANCTRL_INPUTMASK_CH5 |
					 ADC_SCANCTRL_INPUTMASK_CH6;
	ADC_InitScan(ADC0, &scanInit);

	/* Enable ADC scan overflow interrupt to indicate lost samples */
	GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
	GPIO_PinOutClear(gpioPortE, 2);
	GPIO_PinOutClear(gpioPortE, 3);
	ADC_IntEnable(ADC0, ADC_IEN_SCANOF);
	NVIC_EnableIRQ(ADC0_IRQn);

	/* PRS pulse configuration */
	PRS_SourceSignalSet(2, PRS_CH_CTRL_SOURCESEL_ADC0, PRS_CH_CTRL_SIGSEL_ADC0SCAN, prsEdgeOff);
	GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);
	PRS->ROUTE = PRS_ROUTE_CH2PEN | PRS_ROUTE_LOCATION_LOC0;

	/* Reset variables */
	adcNoiseA = 2050;
	adcNoiseB = 2050;
	adcNoiseC = 2050;
	ah = adcNoiseA + ADC_TH_A;
	al = adcNoiseA - ADC_TH_A;
	bh = adcNoiseB + ADC_TH_B;
	bl = adcNoiseB - ADC_TH_B;
	ch = adcNoiseC + ADC_TH_C;
	cl = adcNoiseC - ADC_TH_C;
	impactA = false;
	impactB = false;
	impactC = false;
	impact = false;
	dmaTransferActive = true;
	dmaBufferIndex = 0;
	dmaTransferNumber = 0;
	uint32_t j;
	for(j = 0; j < ADC_SAMPLES; j++) {
		dma16bitBuffer0[j] = 0;
		dma16bitBuffer1[j] = 0;
	}

	/* Start ADC scan */
	ADC_Start(ADC0, adcStartScan);

}

/**************************************************************************//**
 * @brief
 *    Callbacks.
 *****************************************************************************/
void dmaTransferComplete(unsigned int channel, bool primary, void *user) {

//	(void) channel;
//	(void) primary;
//	(void) user;
//	/* Stopping ADC by stopping TIMER0 */
//	TIMER_Enable(TIMER0, false);
//	/* Clearing flag to indicate that transfer is complete */
//	dmaTransferActive = false;
//	(void) user;
//	static uint32_t transferNumber = 0;
//	/* Keep track of transfer number */
//	transferNumber++;
//	/* Repeat the DMA transfer */
//	if (transferNumber < ADC_PINGPONG_TRANSFERS) {
//	/* Reactivate the DMA */
//	DMA_RefreshPingPong(channel,
//						primary,
//						false,
//						NULL,
//						NULL,
//						(ADC_SAMPLES - 1),
//						false);
//	} else {
//		/* Stop ADC */
//		ADC_Reset(ADC0);
//		/* Clear flag */
//	    dmaTransferActive = false;
//	    transferNumber = 0;
//	}





	(void) user;

	GPIO_PinOutSet(gpioPortE, 2);

	static uint32_t i;

//	/* Search after some warm up loops */
//	if (dmaTransferNumber >= WARM_UP_LOOPS) {

		/* dmaBuffer0 */
		if ((dmaTransferNumber % 2) == 0) {
			for(i = 0; i < ADC_SAMPLES; i = (i + (ADC_CHANNELS << 1))) {
				impactA = (dma16bitBuffer0[i+0] > ah) || (dma16bitBuffer0[i+0] < al);
				impactB = (dma16bitBuffer0[i+1] > bh) || (dma16bitBuffer0[i+1] < bl);
				impactC = (dma16bitBuffer0[i+2] > ch) || (dma16bitBuffer0[i+2] < cl);
				if(impactA | impactB | impactC) {
					impact = true;
					dmaBufferIndex = 0;
					break;
				}
			}
		/* dmaBuffer1 */
		} else {
			for(i = 0; i < ADC_SAMPLES; i = (i + (ADC_CHANNELS << 1))) {
				impactA = (dma16bitBuffer1[i+0] > ah) || (dma16bitBuffer1[i+0] < al);
				impactB = (dma16bitBuffer1[i+1] > bh) || (dma16bitBuffer1[i+1] < bl);
				impactC = (dma16bitBuffer1[i+2] > ch) || (dma16bitBuffer1[i+2] < cl);
				if(impactA | impactB | impactC) {
					impact = true;
					dmaBufferIndex = 1;
					break;
				}
			}
		}

		/* Check for any impact */
		if (impact) {
			ADC_Reset(ADC0);
			dmaTransferActive = false;
			GPIO_PinOutSet(gpioPortE, 3);
		} else {
			/* Reactivate the DMA */
			DMA_RefreshPingPong(channel, primary, false, NULL, NULL, (ADC_SAMPLES - 1), false);
			dmaTransferNumber++;
		}


//	/* Do warm up again */
//	} else {
//		/* Reactivate the DMA */
//		DMA_RefreshPingPong(channel, primary, false, NULL, NULL, (ADC_SAMPLES - 1), false);
//	}

	GPIO_PinOutClear(gpioPortE, 2);

}


fix16_t* solve_by_tree(fix16_t dab, fix16_t dac, fix16_t dbc){
	if(dab== 0 && dac== 0 && dbc == 0) return NULL;
	fix16_t* res = (fix16_t *) malloc(2 * sizeof(fix16_t));
	if(res == NULL) return NULL;

	volatile int16_t i = (int16_t)0;
	volatile int16_t j = (int16_t)0;

	//anzA
	volatile int16_t anzA = (int16_t)0;
	for(i = (int16_t)0; i < 48; i++){
		if (dab >= 0 && model[0].arr1[i] >= 0){
			anzA++;
		} else if (dab < 0 && model[0].arr1[i] < 0){
			anzA++;
		}
	}

	//printf("%d", anzA);

	int16_t* posA = (int16_t *) malloc(anzA * sizeof(int16_t));
	if(posA == NULL) return NULL;

	volatile int16_t pointer = (int16_t)0;

	for(i = (int16_t)0; i < 48; i++){
		if (dab >= 0 && model[0].arr1[i] >= 0){
			posA[pointer] = i;
			pointer++;
		} else if (dab < 0 && model[0].arr1[i] < 0){
			posA[pointer] = i;
			pointer++;
		}
	}

	//anzB
	volatile int16_t anzB = (int16_t)0;
	for(i = (int16_t)0; i < 48; i++){
		if (dac >= 0 && model[1].arr1[i] >= 0){
			anzB++;
		} else if (dac < 0 && model[1].arr1[i] < 0){
			anzB++;
		}
	}

	//printf("%d", anzB);

	// reserve space
	int16_t* posB = (int16_t *) malloc(anzB * sizeof(int16_t));
	if(posB == NULL) return NULL;

	pointer = (int16_t)0;

	for(i = (int16_t)0; i < 48; i++){
		if (dac >= 0 && model[1].arr1[i] >= 0){
			posB[pointer] = i;
			pointer++;
		} else if (dac < 0 && model[1].arr1[i] < 0){
			posB[pointer] = i;
			pointer++;
		}
	}

	//anzB
	volatile int16_t anzC = (int16_t)0;
	for(i = (int16_t)0; i < 48; i++){
		if (dbc >= 0 && model[2].arr1[i] >= 0){
			anzC++;
		} else if (dbc < 0 && model[2].arr1[i] < 0){
			anzC++;
		}
	}

	//printf("%d", anzC);

	// reserve space
	int16_t* posC = (int16_t *) malloc(anzC * sizeof(int16_t));
	if(posC == NULL) return NULL;

	pointer = (int16_t)0;

	for(i = (int16_t)0; i < 48; i++){
		if (dbc >= 0 && model[2].arr1[i] >= 0){
			posC[pointer] = i;
			pointer++;
		} else if (dbc < 0 && model[2].arr1[i] < 0){
			posC[pointer] = i;
			pointer++;
		}
	}

	//find intersections AB AC (temp1)
	volatile int16_t anzTemp1 = 0;
	for(i = (int16_t)0; i<anzA;i++){
		for(j=(int16_t)0;j<anzB;j++){
			if(posA[i] == posB[j]){
				anzTemp1++;
			}
		}
	}

	//printf("%d", anzTemp1);


	// reserve space
	int16_t* intTemp1 = (int16_t *) malloc(anzTemp1 * sizeof(int16_t));
	if(intTemp1 == NULL) return NULL;

	pointer = (int16_t)0;

	for(i = (int16_t)0; i<anzA;i++){
		for(j=(int16_t)0;j<anzB;j++){
			if(posA[i] == posB[j]){
				intTemp1[pointer] = posA[i];
				pointer++;
			}
		}
	}

	//find intersection of temp1 and BC (temp2)
	volatile int16_t anzTemp2 = 0;
	for(i = (int16_t)0; i<anzTemp1;i++){
		for(j=(int16_t)0;j<anzC;j++){
			if(intTemp1[i] == posC[j]){
				anzTemp2++;
			}
		}
	}

	//printf("%d", anzTemp2);

	// reserve space
	int16_t* intTemp2 = (int16_t *) malloc(anzTemp2 * sizeof(int16_t));
	if(intTemp2 == NULL) return NULL;

	pointer = (int16_t)0;

	for(i = (int16_t)0; i<anzTemp1;i++){
		for(j=(int16_t)0;j<anzC;j++){
			if(intTemp1[i] == posC[j]){
				intTemp2[pointer] = intTemp1[i];
				pointer++;
			}
		}
	}


	int16_t* x = (int16_t *) malloc(anzTemp2 * sizeof(int16_t));
	if(x == NULL) return NULL;
	int16_t* y = (int16_t *) malloc(anzTemp2 * sizeof(int16_t));
	if(y == NULL) return NULL;

	for(i = (int16_t)0; i< anzTemp2; i++){
		x[i] = (int16_t)positions[intTemp2[i]].coordinates[0];
		printf("x[%d] = %d\n", i, x[i]);
		y[i] = (int16_t)positions[intTemp2[i]].coordinates[1];
		printf("y[%d] = %d\n", i, x[i]);
	}

	fix16_t mX = meanfix(x, anzTemp2);
	fix16_t mY = meanfix(y, anzTemp2);

	res[0] = mX;
	res[1] = mY;

	free(posA);
	free(posB);
	free(posC);
	free(intTemp1);
	free(intTemp2);
	free(x);
	free(y);

	return res;
}



/***************************************************************************//**
* @brief main(void)
*    Starts ADC conversion as fast as possible.
*******************************************************************************/
int main(void) {

	/*
	 *  - load  Model
	 *  - wait for impact
	 *  - calculate time differences
	 *  - solve per tree
	 *  - print position
	 */


	/* Enable Orange LED - Indication that uC is running*/
	enableOrangeLEDs();

	/* Initialize chip */
	CHIP_Init();

	/* Enable highest clock */
	enableOscillator();

	/* Set up printf method*/
    SWO_SetupForPrint();

	/* Alternative Enable UART for sending results to Bluetooth Module */
	//LEUART_Setup();

	/* Configure sensor interface and start measurement */
	ADC_ConfigScanConversion(12);

	/* 1. Init */
	initData();


	while(true) {

		//~3,3  us

		/* 2. Wait for impact */
		while(dmaTransferActive);

		/* Enable Blue LED - Indication that impact is detected*/
		enableBlueLEDs();

		/* Write to buffers */
		uint16_t i;

		for(i = 0; i<300; i++){
			dataInA[i] = (int16_t)dma16bitBuffer0[i*3+0];
			dataInB[i] = (int16_t)dma16bitBuffer0[i*3+1];
			dataInC[i] = (int16_t)dma16bitBuffer0[i*3+2];
		}
		for(i = 300; i<600; i++){
			dataInA[i] = (int16_t)dma16bitBuffer1[(i-300)*3+0];
			dataInB[i] = (int16_t)dma16bitBuffer1[(i-300)*3+1];
			dataInC[i] = (int16_t)dma16bitBuffer1[(i-300)*3+2];
		}


		/* Print Buffer */
		for(i = 0; i<600; i++){
			printf("%i;%i;%i\n", dataInA[i],dataInB[i],dataInC[i]);
		}

		/*
		 * analyze buffer, because impact occured!
		 *
		 */

		/* 3. Calculate Differences */
		volatile int16_t count = 600;

		/* 3.1 Move */
		move(dataInA, count);
		move(dataInB, count);
		move(dataInC, count);

		/* 3.2 Time */
		int16_t a = time(dataInA, count);
		int16_t b = time(dataInB, count);
		int16_t c = time(dataInC, count);

		//printf("a: %d b: %d c: %d\n", a,b,c);

		/* 3.3 Delta */
		fix16_t dab = delta(a,b);
		fix16_t dac = delta(a,c);
		fix16_t dbc = delta(b,c);

		/* Print difference */
		char dabstr[10];
		char dacstr[10];
		char dbcstr[10];
		fix16_to_str(dab, dabstr, 2);
		fix16_to_str(dac, dacstr, 2);
		fix16_to_str(dbc, dbcstr, 2);
		printf("dab: %s, dac: %s, dbc: %s \n", dabstr, dacstr, dbcstr);

		/*fix16_t dab = fix16_from_str("237.34");
		fix16_t dac = fix16_from_str("156.43");
		fix16_t dbc = fix16_from_str("-219.09");*/

		/* Solve by tree */
		fix16_t* resPosI = solve_by_tree(dab, dac, dbc);
		if(resPosI == NULL) {
			printf("ERROR");
		}else {
			volatile fix16_t posX = resPosI[0];
			volatile fix16_t posY = resPosI[1];

			char posXstr[20];
			char posYstr[20];
			fix16_to_str(posX, posXstr, 2);
			fix16_to_str(posY, posYstr, 2);

			/* Print position */
			printf("Position: x: %s y: %s \n", posXstr, posYstr);
		}

		/* Disable Blue LED - Indication that impact processed*/
		disableBlueLEDs();

		/* Configure sensor interface and start measurement */
		ADC_ConfigScanConversion(12);

	}
}


void FLASH_writeWord(uint32_t adr, uint32_t data)
{
  /* Enable Write feature and write feature */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Load address */
  MSC->ADDRB    = adr;
  MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

  /* Load data */
  MSC->WDATA = data;

  /* Trigger write once */
  MSC->WRITECMD = MSC_WRITECMD_WRITEONCE;
}


void enableOscillator(void){
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true); // HMM
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_1);
	CMU_ClockEnable(cmuClock_HFPER, true);
}

void enableOrangeLEDs(void){
	CMU_ClockEnable(cmuClock_GPIO, true);                      // Enable GPIO peripheral clock
	GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 1);      // Configure LED0 pin as digital output (push-pull)
}

void disableOrangeLEDs(void){
	CMU_ClockEnable(cmuClock_GPIO, true);                      // Enable GPIO peripheral clock
	GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 0);      // Configure LED0 pin as digital output (push-pull)
}

void enableBlueLEDs(void){
	GPIO_PinModeSet(gpioPortA, 3, gpioModePushPull, 1);      // Configure LED0 pin as digital output (push-pull)
}

void disableBlueLEDs(void){
	GPIO_PinModeSet(gpioPortA, 3, gpioModePushPull, 0);      // Configure LED0 pin as digital output (push-pull)
}
