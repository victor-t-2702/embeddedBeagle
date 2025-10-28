
#include "hal/sampling.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

// Allow module to ensure it has been initialized (once!)
static bool is_initialized = false;
static bool sampling = false;

static double currentData[1000];
static double historyData[1000];
static double *current = currentData;
static double *history = historyData;
#define CURRENT_DATA_SIZE 1000
#define HISTORY_DATA_SIZE 1000

static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

//Intializes the sampling thread
void sampling_init(void) 
{
    //printf("Sampling - Initializing\n");
    assert(!is_initialized);
    is_initialized = true;
    sampling = true;
    pthread_t sampling_thread;
    pthread_create(&sampling_thread, NULL, sample, NULL);

}

void sample(sampling)
{
    assert(is_initialized);
    while(sampling){
        long long start_time = getTimeInMs();


        while(true){
            long long current_time = getTimeInMs();
            long long elapsed_time = current_time - start_time;
            if(elapsed_time < 1000){
                //read spi
            }
            if(elapsed_time >= 1000){
                break;
            }   
        }    
        currentDataToHistory()
        usleep(1000)
    }
}


void currentDataToHistory(void)
{
    pthread_mutex_lock(data_mutex, NULL);
    double *temp = current;
    current = history;
    history = temp;
    pthread_mutex_unlock(data_mutex, NULL);

}

double getSamplerHistory(double* storage, int* size)
{
    assert(is_initialized);
    *size = HISTORY_DATA_SIZE;
    pthread_mutex_lock(&data_mutex);
    for(int i = 0; i < HISTORY_DATA_SIZE; i++){
        storage[i] = historyData[i];
    }
    pthread_mutex_unlock(&data_mutex);
}

