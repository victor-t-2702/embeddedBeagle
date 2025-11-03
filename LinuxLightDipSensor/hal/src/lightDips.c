#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "hal/lightDips.h"

#define HYSTERSIS_VALUE 0.03

static pthread_t lightDips_thread;
static bool is_initialized = false;
static bool analyzing = false;
static int lightDipsPipe_fd = -1;


historyValues history;

static bool dipped = false;
static int dipCount = 0;

void lightDips_init(int pipeReadfd)
{
    assert(!is_initialized);
    lightDipsPipe_fd = pipeReadfd;
    int thread_result = pthread_create(&lightDips_thread, NULL, analyzeDips, NULL);
    

    if (thread_result != 0) {
        perror("Error when creating light dip thread");
        //exit(EXIT_FAILURE);
        return;
    }

    is_initialized = true;
    analyzing = true;
    
}

void* analyzeDips(void* arg){
    assert(is_initialized);
    while(analyzing){
        //printf("=== READER ===\n");
        //printf("New array allocated at: %p\n", (void*)history.samples);
        //printf("Size: %d, Average: %f\n", history.size, history.average);


        if(read(lightDipsPipe_fd, &history, sizeof(history)) != sizeof(historyValues)){
            printf("Reading error");
        }

        // read(lightDipsPipe_fd, &history.size, sizeof(int));
        // read(lightDipsPipe_fd, &history.average, sizeof(float));
        // history.samples = malloc(history.size * sizeof(float));
        // read(lightDipsPipe_fd, history.samples, history.size * sizeof(float));

        for(int i = 0; i < history.size; i++){
            //Lower voltage means higher light, because photoresistor is connected to VCC
            //Higher votage thus means a dip in light
            if (!dipped && (history.samples[i] > (history.average + 0.1))) {
                dipped = true;   // Enter dip region
                dipCount++;
            } 
            else if (dipped && (history.samples[i] < (history.average + 0.1 - HYSTERSIS_VALUE))) {
                dipped = false;  // Exit dip region
            }
        }
        //printf("Dip Count: %d", dipCount);
        free(history.samples);
    }
    return arg;
    
}

void lightDips_cleanup(void)
{
    assert(is_initialized);
    is_initialized = false;

    if (lightDipsPipe_fd != -1) {
        close(lightDipsPipe_fd);
    }

    analyzing = false;
    pthread_join(lightDips_thread, NULL);
}

