#ifndef _LIGHTDIPS_H_
#define _LIGHTDIPS_H_

#include <stdbool.h>


typedef struct{
    double* samples;
    double average;
    int size;
} historyValues;

void lightDips_init(int pipeReadfd);
void* analyzeDips(void* arg);
void lightDips_cleanup(void);

#endif