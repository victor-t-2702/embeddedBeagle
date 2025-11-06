//Implementation of sampling.h
//Every second, the accumulated samples will be put into a buffer 
//Use getSamplerHistory to get both the samples in the history buffer and the amount of samples in the buffer
//Use getSampleAverage to get the weighted average of all samples
//Use getTotalSample to get the total amount of samples taked since intiailziation
#include "hal/sampling.h"
#include "hal/accessSPI.h"
#include "hal/periodTimer.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

// Allow module to ensure it has been initialized (once!)
static bool is_initialized = false;

//Bool that controls the main sampling loop, ie the function ran by the thread
static bool sampling = false;

//The struct that holds all the sample variables, currentData accumulates for 1 sec before getting moved to historyData
static struct samples_struct
{
    double currentData[1000];
    double historyData[1000];

    double *current;
    double *history;

    //Sizes of the buffer
    int currentDataSize;
    int historyDataSize;

    //Total samples and samples average
    long long totalSamples;
    double sampleAverage;

} samples;


//The sampling thread and the mutex objects
static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t sampling_thread;

//A function to get current time in ms, allowing for a timer to be used to measure 1s
static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

//Function prototypes
static void currentDataToHistory(void);
void* sample(void*);

//Intializes the sampling thread, sets all the variables to zero 
void sampling_init(void) 
{
    //Set all the flags to allow functions to run
    assert(!is_initialized);
    is_initialized = true;
    sampling = true;

    //Clear variables
    samples.totalSamples = 0;

    for(int i = 0; i < 1000; i++){
        samples.currentData[i] = 0;
        samples.historyData[i] = 0;
    }
    samples.current = samples.currentData;
    samples.history = samples.historyData;
    samples.currentDataSize = 0;
    samples.historyDataSize = 0;

    //Create the thread
    int thread_result = pthread_create(&sampling_thread, NULL, sample, NULL);

    //Check if a thread is created
    if (thread_result != 0) {
        perror("Error when creating thread");
        exit(EXIT_FAILURE);
    }

}

//The function that moves the current data into the history buffer, an internal method
static void currentDataToHistory(void)
{
    //Lock before changing values
    pthread_mutex_lock(&data_mutex);
    for (int i = 0; i < samples.currentDataSize; i++) {
        samples.history[i] = samples.current[i];
    }

    // Zero out current
    for (int i = 0; i < 1000; i++) {
        samples.current[i] = 0.0;
    }
    samples.historyDataSize = samples.currentDataSize;
    samples.currentDataSize = 0;

    //Unlock
    pthread_mutex_unlock(&data_mutex);

}

//This function runs continously, with sampling being the flag that keeps the loop going
void* sample(void* arg)
{
    assert(is_initialized);
    while (!spi_is_ready()) {          // wait for SPI HAL
        usleep(1000);                  // 1 ms
    }
    //Outer loop is the whole program: 1 sec read, then transfer then 1 sec read then transfer ...
    while(sampling){

        //Start a timer for 1 second
        long long start_time = getTimeInMs();

        //The inner loop is the part that runs every seoncd
        while(true){

            //Get the elapsed time to check if a second has passed
            long long current_time = getTimeInMs();
            long long elapsed_time = current_time - start_time;

            //Read from the spi and convert values to voltages, putting them in the currentData buffer
            //This occurs until 1s has elapsed
            if((elapsed_time < 1000)){
                samples.currentData[samples.currentDataSize] = (double)((read_ch(0, 500))/4095.0)*3.3; // convert to voltages from SPI output

                //Also calculate the weighted average everytime a sample is taken, first one is base case for 1st sample
                if(samples.totalSamples == 0){
                    samples.sampleAverage = samples.currentData[samples.currentDataSize];
                }
                //Calculate the weighted average
                else{
                    samples.sampleAverage = samples.sampleAverage*0.999 + samples.currentData[samples.currentDataSize]*0.001;
                }

                //Increment the currentData buffer size and the number of total samples
                Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);
                samples.currentDataSize++;
                samples.totalSamples++;

                //Sleep for 1ms to prevent calling read_ch too fast
                usleep(1000);
            }

            //After 1 second has passed, break the while true loop
            if(elapsed_time >= 1000){
                break;
            }   
        }    

        //Move the buffer contents, then sleep for 1ms
        currentDataToHistory();
        usleep(1000);


    }

    return arg;
}


//IMPORTANT
//This gets the values from the history buffer, pass in size, size gets modified directly in the function to be the history size
//Returns a pointer to a double array, this double array is created inside the function, needs to be freed by caller!
//So this function returns a pointer to a newly made double array with the contents of history and modifies the int that comes in 
//to be the size of the history array, basically providing two values
double* getSamplerHistory(int* size)
{
    assert(is_initialized);

    //Lock before reading the values of history buffer
    pthread_mutex_lock(&data_mutex);
        if (samples.historyDataSize <= 0) {
        pthread_mutex_unlock(&data_mutex);
        *size = 0;
        return NULL;
    }

    //Create a new array, needs to be freed in main!
    double* output = malloc(samples.historyDataSize * sizeof(double));
    if (output == NULL) {
        pthread_mutex_unlock(&data_mutex);
        free(output);
        perror("Error allocating output buffer");
        exit(EXIT_FAILURE);
    }

    //Set input to the size of the history buffer
    *size = samples.historyDataSize;

    //Fill the new array
    for(int i = 0; i < samples.historyDataSize; i++){
        output[i] = samples.history[i];
    }

    //Unlock
    pthread_mutex_unlock(&data_mutex);

    return output;
}

//Returns the average that is calculated everytime a sample is taken
double getSampleAverage(void)
{
    assert(is_initialized);
    pthread_mutex_lock(&data_mutex);
    double average = samples.sampleAverage;
    pthread_mutex_unlock(&data_mutex);
    return average;
}

//Returns the total amount of samples taken since the thread started
long long getTotalSample(void)
{
    assert(is_initialized);
    pthread_mutex_lock(&data_mutex);
    long long total = samples.totalSamples;
    pthread_mutex_unlock(&data_mutex);
    return total;
    
}


//Breaks the sampling function ran by the thread and and closes the thread
void sampling_cleanup(void)
{
    assert(is_initialized);
    sampling = false;

    pthread_join(sampling_thread, NULL);
    is_initialized = false;

}