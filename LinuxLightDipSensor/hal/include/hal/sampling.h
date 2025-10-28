#ifndef _SAMPLING_H_
#define _SAMPLING_H_

#include <stdbool.h>


void sampling_init(void);
double* getSamplerHistory(int* size);
void sampling_cleanup(void);

#endif