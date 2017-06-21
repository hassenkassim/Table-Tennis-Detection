#ifndef SPI_H_
#define SPI_H_

#include <stdbool.h>
#include <stdint.h>

#include "em_cmu.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_prs.h"


#define SPI_TH_A			100
#define SPI_TH_B			100
#define SPI_TH_C			100

/* CSa = PA1, CSb = PA0, CSc = PC0, SCLK = PE13, DATAa = PA1, DATAb = PB11, DATAc = PC1 */
#define SPI_CLK_HIGH (GPIO_PinOutSet(gpioPortE, 13))
#define SPI_CLK_LOW  (GPIO_PinOutClear(gpioPortE, 13))
#define SPI_CSa_HIGH (GPIO_PinOutSet(gpioPortB, 11))
#define SPI_CSa_LOW  (GPIO_PinOutClear(gpioPortB, 11))
#define SPI_CSb_HIGH (GPIO_PinOutSet(gpioPortC, 2))
#define SPI_CSb_LOW  (GPIO_PinOutClear(gpioPortC, 2))
#define SPI_CSc_HIGH (GPIO_PinOutSet(gpioPortC, 0))
#define SPI_CSc_LOW  (GPIO_PinOutClear(gpioPortC, 0))
#define SPI_DATAa_IN (GPIO_PinInGet(gpioPortA, 1))
#define SPI_DATAb_IN (GPIO_PinInGet(gpioPortA, 0))
#define SPI_DATAc_IN (GPIO_PinInGet(gpioPortC, 1))

//static void SPI_Config(void);
//static void SPI_GetSensorData(void);
//static void SPI_GetNoise(void);
//static void SPI_ScanSignals(void);


#endif /* SPI_H_ */
