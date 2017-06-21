/*
 * main.h
 *
 *  Created on: 09.12.2016
 *      Author: HassenKassim
 */

//#ifndef SRC_MAIN_H_
//#define SRC_MAIN_H_

/*****************************************************************************
 *  Write function: Writes string to the standard output
 * 					Is used for printf operation
 *****************************************************************************/
//int _write(int file, const char *ptr, int len);


/*****************************************************************************
 * Programs a single word into flash.
 *****************************************************************************/
void FLASH_writeWord(uint32_t adr, uint32_t data);

/**************************************************************************//**
 * @brief
 *    Prototypes.
 *****************************************************************************/
void ADC_ConfigScanConversion(uint8_t res);

/**************************************************************************//**
 * @brief
 *    Interrupts.
 *****************************************************************************/
void ADC0_IRQHandler(void);


/**************************************************************************//**
 * @brief
 *    Callbacks.
 *****************************************************************************/
void dmaTransferComplete(unsigned int channel, bool primary, void *user);

/*****************************************************************************
 * Sets up new printf function
 *****************************************************************************/
void SWO_SetupForPrint(void);

/*****************************************************************************
 * Enable Oscillator
 *****************************************************************************/
void enableOscillator(void);

/*****************************************************************************
 * Enable Orange (Start) LED
 *****************************************************************************/
void enableOrangeLEDs(void);

/*****************************************************************************
 * Disable Orange (Start) LED
 *****************************************************************************/
void disbaleOrangeLEDs(void);

/*****************************************************************************
 * Enable Blue (End) LED
 *****************************************************************************/
void enableBlueLEDs(void);

/*****************************************************************************
 * Disable Blue (End) LED
 *****************************************************************************/
void disableBlueLEDs(void);

//#endif /* SRC_MAIN_H_ */