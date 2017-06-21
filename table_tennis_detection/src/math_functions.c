/***********************
 * Math C-File
 * For all math operations
 ***********************/
#include "math_functions.h"

#include "fix16.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Calculates the differences between each values in the array
 * dataNew[a] = data[a+1] - data[a]
 * then first one = data[a]
 */
int16_t* diff(int16_t data[], int16_t count){
	int16_t* dataret = (int16_t *) malloc(count * sizeof(int16_t));
	if(dataret == NULL) return NULL;

	volatile int16_t i;

	for(i = count; i>0; i--){
		dataret [i] = data[i] - data[i-1];
	}
	dataret[0] = dataret[1];
	return dataret;
}

/* Swaps the array at the highest Value */
void move(int16_t data[], int16_t count){
	int16_t* diffdata = diff(data, count);
	absolute(diffdata, count);
	int16_t maxVal = maximum(diffdata, count);
	int16_t x = valIndex(diffdata, count, maxVal);
	swapData(data, count, x);
}

/* Calculates Time difference */
int16_t time(int16_t data[], int16_t count){
	volatile int16_t th_factor = 20;
	volatile fix16_t th_factorfix = fix16_from_int(th_factor);

	volatile int16_t n = 50;

	//TODO: noise not accurate, probably threshold must be decreased..

	volatile fix16_t noise = meanfix32(data,n);
	volatile int16_t max = maximum(data,n);
	volatile int16_t min = minimum(data,n);

	volatile fix16_t maxfix = fix16_from_int(max);
	volatile fix16_t minfix = fix16_from_int(min);

	volatile fix16_t athmaxfix = fix16_add(fix16_mul(fix16_sub(maxfix,noise), th_factorfix), noise);
	volatile fix16_t athminfix = fix16_sub(noise, fix16_mul(fix16_sub(noise, minfix), th_factorfix));

	int16_t ind = indDataThresh(data, count, athmaxfix, athminfix);

	/*volatile int16_t noise = mean(data, n);

	volatile int16_t max = maximum(data,n);
	volatile int16_t min = minimum(data,n);

	volatile int16_t athmax = ((max-noise) * th_factor) + noise;
	volatile int16_t athmin = noise - ((noise-min) * th_factor);

	//find index
	int16_t ind = indDataThresh(data, count, athmax, athmin);*/

	return ind;
}

/* Index of value which exceeds maxThresh or falls under minThresh */
int16_t indDataThresh(int16_t data[], int16_t count, fix16_t maxThresh, fix16_t minThresh){
	volatile int16_t i;
	for(i = 0; i< count; i++){
		if(fix16_largerThan(data[i],maxThresh) || fix16_smallerThan(data[i], minThresh)) return i;

		//if(data[i]>maxThresh || data[i]<minThresh) return i;
	}
	return -1;
}

/* Calculate difference of two values multiplied by the duration of chip cycle */
fix16_t delta(int16_t a, int16_t b){
	fix16_t T = fix16_from_str("3.27");
	fix16_t result = fix16_mul(fix16_sub(fix16_from_int(a), fix16_from_int(b)), T);
	return result;
}

/* Get absolute values */
void absolute(int16_t* data, int16_t count){
	volatile int16_t i;

	for(i = 0; i< count; i++){
		data[i] = abs(data[i]);
	}
}

/* Get maximum value */
int16_t maximum(int16_t* data, int16_t count){
	volatile int16_t ret;
	ret = data[0];

	volatile int16_t i;
	for(i = 1; i< count; i++){
		if(data[i] > ret) {
			ret = data[i];
		}
	}
	return ret;
}

/* Get minimum value */
int16_t minimum(int16_t* data, int16_t count){
	volatile int16_t ret;
	ret = data[0];

	volatile int16_t i;
	for(i = 1; i< count; i++){
		if(data[i] < ret) {
			ret = data[i];
		}
	}
	return ret;
}

/* Get index of a value */
int16_t valIndex(int16_t data[], int16_t count, int16_t val){
	volatile int16_t i;

	for(i = 0; i< count; i++){
		if(data[i] == val){
			return i;
		}
	}
	return -1;
}

/* swaps data at specific index */
void swapData(int16_t data[], int16_t count, int16_t maxIndex){
	volatile int16_t i;
	int16_t dataTmp[maxIndex];
	for(i=0; i< maxIndex; i++){
		dataTmp[i] = data[i];
	}

	for(i = maxIndex; i<count; i++){
		data[-maxIndex + i] = data[i];
	}

	for(i = 0; i < maxIndex; i++){
		data[count-maxIndex+i] = dataTmp[i];
	}
}

/* Calculates mean value and cuts of decimal places */
int16_t mean(int16_t data[], int16_t count){
	volatile int32_t sum = 0;
	volatile int16_t i;
	for(i=0; i<count;i++){
		sum = sum + data[i];
	}

	volatile int16_t res = (int16_t)(sum/count);

	return res;
}

/* Calculates mean value and doesn't cut off decimal places,
 * Problem: fix16 maps 32 bit int value to 16 bit number with 16 bit for decimal playces
 * -> sum only can reach -32,768 .. 32,767. Otherwise overflow occures */
fix16_t meanfix(int16_t data[], int16_t count){
	volatile int16_t sum = 0;
	volatile int16_t i;
	for(i=0; i<count;i++){
		sum = sum + data[i];
	}

	volatile fix16_t sumfix = fix16_from_int(sum);
	volatile fix16_t countfix = fix16_from_int(count);

	volatile fix16_t res = fix16_div(sumfix,countfix);

	return res;
}


/* Calculates the mean value and doesn't cut off decimal places
 * Workaround over fixedptc.h: fixed point arithmetic with 64 bits */
fix16_t meanfix32(int16_t data[], int16_t count){
		volatile int32_t sum = 0;
		volatile int16_t i;

		for(i=0; i<count;i++){
			sum = sum + data[i];
		}

		volatile fixedpt sumfix = fixedpt_fromint(sum);
		volatile fixedpt countfix = fixedpt_fromint(count);
		volatile fixedpt res = fixedpt_div(sumfix, countfix);

		/* Convert to fix16 */
		char mean[20];
		fixedpt_str(res, mean, 2);
		volatile fix16_t resfix16 = fix16_from_str(mean);

		return resfix16;
}


