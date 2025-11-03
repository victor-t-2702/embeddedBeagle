#ifndef _LIGHTDIPS_H_
#define _LIGHTDIPS_H_

#include <stdbool.h>


typedef struct{
    double* samples;
    double average;
    int size;
} historyValues;

void startLightDipsDetect();
void endLightDipsDetect();
int getDips();


#endif