#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "hal/lightDips.h"
#include "hal/sampling.h"


static pthread_t lightDips_thread;
static pthread_mutex_t light_dips_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool detect = false;
static int dipCounter = 0;
static int local_dipCounter = 0;

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

int getDips() {
    int temp = 0;
    pthread_mutex_lock(&light_dips_mutex);
    temp = dipCounter;
    pthread_mutex_unlock(&light_dips_mutex);
    return temp;
}

void startLightDipsDetect() {
    assert(!detect);
    detect = true;
    if (pthread_create(&lightDips_thread, NULL, lightDipsDetect, NULL) != 0) {
        perror("Failed to create light dips detect thread");
        return;
    }
}

void endLightDipsDetect() {
    assert(detect);
    detect = false;
}


