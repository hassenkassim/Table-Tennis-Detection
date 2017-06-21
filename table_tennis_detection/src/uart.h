#ifndef UART_H_
#define UART_H_

#include "em_leuart.h"

#define LEUART_INIT {		\
	leuartDisable,			\
	0,						\
	9600,					\
	leuartDatabits8,		\
	leuartNoParity,			\
	leuartStopbits1,		\
}

void LEUART_Setup(void);
void LEUART_Printf(char *data);

void LEUART_Write32bit(uint32_t data);
void LEUART_Write16bit(uint16_t data);
void LEUART_Write8bit(uint8_t data);

#endif /* UART_H_ */
