// lightdips.c has the implementation of the lightDips.h header file
// Uses hysterisis and a photoresistor to detect dips in ambient light
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "hal/lightDips.h"
#include "hal/sampling.h"


static pthread_t lightDips_thread; // sampling thread
static pthread_mutex_t light_dips_mutex = PTHREAD_MUTEX_INITIALIZER; // mutex to protect global dipCounter variable
static bool detect = false; // flag to ensure thread should be running
static int dipCounter = 0; // global variable that keeps track of the dips in the previous second
static int local_dipCounter = 0; // temp variable that keeps track of the dips in the previous second before they are copied over to dipCounter

// Thread function that uses hysterisis to detect light dips of ambient light "recorded" by a photoresistor
static void* lightDipsDetect(void *arg) {
    historyValues values;
    values.size = 0;
    bool in_dip = false;
    while (detect) {
        in_dip = false;
        values.samples = getSamplerHistory(&(values.size));
        values.average = getSampleAverage();
        local_dipCounter = 0;
        if (values.samples != NULL && values.size > 0) {
            for(int i = 0; i < values.size; i++) {  
                if (values.samples[i] <= (values.average - 0.1) && in_dip == false) {
                    in_dip = true;
                    local_dipCounter++;
                }
                else if (values.samples[i] >= (values.average - 0.07) && in_dip == true) {
                    in_dip = false;
                }
            }
            free(values.samples); 
        } else {
            continue;
        }
        pthread_mutex_lock(&light_dips_mutex);
        dipCounter = local_dipCounter;
        pthread_mutex_unlock(&light_dips_mutex);
        sleep(1);
    }
    return arg;
}

// Exposes static dipCounter variable
int getDips() {
    int temp = 0;
    pthread_mutex_lock(&light_dips_mutex);
    temp = dipCounter;
    pthread_mutex_unlock(&light_dips_mutex);
    return temp;
}

// Starts dip detecting thread
void startLightDipsDetect() {
    assert(!detect);
    detect = true;
    if (pthread_create(&lightDips_thread, NULL, lightDipsDetect, NULL) != 0) {
        perror("Failed to create light dips detect thread");
        return;
    }
}

// Ends dip detecting thread
void endLightDipsDetect() {
    assert(detect);
    detect = false;
    pthread_join(lightDips_thread, NULL);
}


