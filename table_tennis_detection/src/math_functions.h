/***********************
 * Math Header
 * For all math operations
 ***********************/

#include <stdint.h>
#include "fix16.h"
#include "fixedptc.h"

/* Calculates the differences between each values in the array
 * dataNew[a] = data[a+1] - data[a]
 * then first one = data[a]
 */
int16_t* diff(int16_t data[], int16_t count);

/* Swaps the array at the highest Value */
void move(int16_t data[], int16_t count);

/* Calculates Time difference */
int16_t time(int16_t data[], int16_t count);

/* Index of value which exceeds maxThresh or falls under minThresh */
int16_t indDataThresh(int16_t data[], int16_t count, fix16_t maxThresh, fix16_t minThresh);

/* Calculate difference of two values multiplied by the duration of chip cycle */
fix16_t delta(int16_t a, int16_t b);

/* Get absolute values */
void absolute(int16_t* data, int16_t count);

/* Get maximum value */
int16_t maximum(int16_t* data, int16_t count);

/* Get minimum value */
int16_t minimum(int16_t* data, int16_t count);

/* Get index of a value */
int16_t valIndex(int16_t data[], int16_t count, int16_t val);

/* swaps data at specific index */
void swapData(int16_t data[], int16_t count, int16_t maxIndex);

/* Calculates mean value and cuts of decimal places */
int16_t mean(int16_t data[], int16_t count);

/* Calculates mean value and doesn't cuts of decimal places,
 * Problem: fix16 maps 32 bit int value to 16 bit number with 16 bit for decimal playces
 * -> sum only can reach -32,768 .. 32,767. Otherwise overflow occures */
fix16_t meanfix(int16_t data[], int16_t count);

/* Calculates the mean value and doesn't cut off decimal places
 * Workaround over fixedptc.h: fixed point arithmetic with 64 bits */
fix16_t meanfix32(int16_t data[], int16_t count);
