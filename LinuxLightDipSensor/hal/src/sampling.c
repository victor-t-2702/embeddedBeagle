
#include "hal/sampling.h"
#include "hal/accessSPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

// Allow module to ensure it has been initialized (once!)
static bool is_initialized = false;
static bool sampling = false;

static struct samples_struct
{
    double currentData[1000];
    double historyData[1000];

    double *current;
    double *history;

    int currentDataSize;
    int historyDataSize;
} samples;


static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t sampling_thread;

static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

void currentDataToHistory(void);
void* sample(void*);

//Intializes the sampling thread
void sampling_init(void) 
{
    //printf("Sampling - Initializing\n");
    assert(!is_initialized);
    is_initialized = true;
    sampling = true;
    samples.current = samples.currentData;
    samples.history = samples.historyData;
    samples.currentDataSize = 0;
    samples.historyDataSize = 0;
    
    int thread_result = pthread_create(&sampling_thread, NULL, sample, NULL);

    
    if (thread_result != 0) {
        perror("Error when creating thread");
        exit(EXIT_FAILURE);
    }

}

void currentDataToHistory(void)
{
    pthread_mutex_lock(&data_mutex);
    double *temp = samples.current;
    samples.current = samples.history;
    samples.history = temp;
    for(int i = 0; i < samples.currentDataSize; i++)
    {
        samples.current[i] = 0;
    }
    samples.historyDataSize = samples.currentDataSize;
    samples.currentDataSize = 0;
    pthread_mutex_unlock(&data_mutex);

}


void* sample(void* arg)
{
    assert(is_initialized);
    while(sampling){
        long long start_time = getTimeInMs();

        while(true){
            long long current_time = getTimeInMs();
            long long elapsed_time = current_time - start_time;
            if(elapsed_time < 1000){
                samples.currentData[samples.currentDataSize] = read_ch(0, 500);
                //Implementing the spi here currentData[index] = read;
                samples.currentDataSize++;
            }
            if(elapsed_time >= 1000){
                break;
            }   
        }    
        usleep(1000);
        currentDataToHistory();

    }

    return arg;
}


double* getSamplerHistory(int* size)
{
    assert(is_initialized);
    pthread_mutex_lock(&data_mutex);
    double* output = malloc(samples.historyDataSize * sizeof(double));
    if (output == NULL) {
        pthread_mutex_unlock(&data_mutex);
        perror("Error allocating output buffer");
        exit(EXIT_FAILURE);
    }
    *size = samples.historyDataSize;
    for(int i = 0; i < samples.historyDataSize; i++){
        output[i] = samples.history[i];
    }
    pthread_mutex_unlock(&data_mutex);

    return output;
}

void sampling_cleanup(void)
{
    assert(is_initialized);
    sampling = false;

    pthread_join(sampling_thread, NULL);
    is_initialized = false;

}