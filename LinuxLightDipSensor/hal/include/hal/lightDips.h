// lightDips.h defines the Light Detecting HAL module
// Uses hysterisis and a photoresistor to detect dips in ambient light
#ifndef _LIGHTDIPS_H_
#define _LIGHTDIPS_H_

#include <stdbool.h>

// Struct that holds all necessary values necessary when calling the getSamplerHistory() function from the Sampling HAL module
typedef struct{
    double* samples;
    double average;
    int size;
} historyValues;

// Starts dip detecting thread
void startLightDipsDetect();

// Ends dip detecting thread
void endLightDipsDetect();

// Exposes static dipCounter variable
int getDips();


#endif