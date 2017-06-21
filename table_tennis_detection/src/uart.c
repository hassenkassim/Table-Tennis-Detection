#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>

#include "em_leuart.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "uart.h"

#undef errno
extern int errno;

/******************************************************************************/
void LEUART_Setup(void) {
	//CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_HFLE, true);
	CMU_ClockEnable(cmuClock_LEUART0, true);
	GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 1);
	LEUART_Init_TypeDef initLEUART = LEUART_INIT;
	initLEUART.baudrate = 115200;
	LEUART_Init(LEUART0, &initLEUART);
	LEUART_Enable(LEUART0, leuartEnable);
	LEUART0->ROUTE |= LEUART_ROUTE_TXPEN | LEUART_ROUTE_LOCATION_LOC4;
}

/******************************************************************************/
void LEUART_Printf(char *data) {
	while(*data) {
		LEUART_Tx(LEUART0, *data);
		data++;
	}
}

/******************************************************************************/
void LEUART_Write32bit(uint32_t data) {
	LEUART_Tx(LEUART0, (uint8_t)((data >> 24) & 0xFF));
	LEUART_Tx(LEUART0, (uint8_t)((data >> 16) & 0xFF));
	LEUART_Tx(LEUART0, (uint8_t)((data >>  8) & 0xFF));
	LEUART_Tx(LEUART0, (uint8_t)((data >>  0) & 0xFF));
}

/******************************************************************************/
void LEUART_Write16bit(uint16_t data) {
	LEUART_Tx(LEUART0, (uint8_t)((data >>  8) & 0xFF));
	LEUART_Tx(LEUART0, (uint8_t)((data >>  0) & 0xFF));
}

/******************************************************************************/
void LEUART_Write8bit(uint8_t data) {
	LEUART_Tx(LEUART0, data);
}

/******************************************************************************/
/*int _write(int file, char *ptr, int len) {
	(void) file;
	(void) (*ptr);
	int n;
	for (n = 0; n < len; n++) {
		//LEUART_Tx(LEUART0, ptr[n]);
	    ITM_SendChar (ptr[n]);
	}
	return len;
}*/

/******************************************************************************/
int _read(int file, char *ptr, int len) {
	(void) (*ptr);
	(void) len;
	switch (file) {
	case STDIN_FILENO:
		return 1;
		break;
	default:
		errno = EBADF;
		return -1;
	}
}

/******************************************************************************/
int _lseek(int file, int ptr, int dir) {
	(void) file;
	(void) ptr;
	(void) dir;
	return 0;
}

/******************************************************************************/
int _isatty(int file) {
	switch (file) {
	case STDOUT_FILENO:
	case STDERR_FILENO:
	case STDIN_FILENO:
		return 1;
	default:
		errno = EBADF;
		return 0;
	}
}

/******************************************************************************/
int _fstat(int file, struct stat *st) {
	(void) file;
	st->st_mode = S_IFCHR;
	return 0;
}

/******************************************************************************/
int _close(int file) {
	(void) file;
	return -1;
}
