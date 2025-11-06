//This module is used to create a sampling thread that constantly reads value from an SPI input
//Every second, the accumulated samples will be put into a buffer 
//Use getSamplerHistory to get both the samples in the history buffer and the amount of samples in the buffer
//Use getSampleAverage to get the weighted average of all samples
//Use getTotalSample to get the total amount of samples taked since intiailziation

#ifndef _SAMPLING_H_
#define _SAMPLING_H_

#include <stdbool.h>

//Intializes the sampling thread, sets all the variables to zero, must be called first before any functions in this module is used
void sampling_init(void);

//This gets the values from the history buffer, pass in size, size gets modified directly in the function to be the history size
//Returns a pointer to a double array, this double array is created inside the function, needs to be freed by caller!
//So this function returns a pointer to a newly made double array with the contents of history and modifies the int that comes in 
//to be the size of the history array, basically providing two values
double* getSamplerHistory(int* size);

//Get the weighted sampling average of every sample taken since intialization
double getSampleAverage(void);

//Gets the total number of samples taken sinze intialization
long long getTotalSample(void);

//Cleanup function that closes the thread
void sampling_cleanup(void);

#endif