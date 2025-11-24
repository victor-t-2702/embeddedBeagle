#include <stdio.h>      // for printf, fprintf
#include <stdlib.h>     // for malloc, free, exit
#include <assert.h>     // for assert()
#include <string.h>     // for memset, strcmp, etc.
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "hal/audioMixer.h"
#include "hal/accessSPI.h"

#define BASELINE 2048
#define DEBOUNCE_TIME 300

static bool is_initialized = false;
static bool reading = false;


static pthread_t accelThread;

static void* detectMovement(void* arg);

typedef struct{
    int magnitude;
    long long pauseStartTime;
    bool paused;
}direction;



static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

void accelerometer_init(void){
    assert(!is_initialized);
    is_initialized = true;
    reading = true;
    if (pthread_create(&accelThread, NULL, detectMovement, NULL) != 0) {
        perror("Failed to create accelerometer thread");
        return;
    }
}


static void* detectMovement(void* arg){
    assert(is_initialized);
    direction x = {0};
    direction y = {0};
    direction z = {0};
    while(reading){
        x.magnitude = read_ch(1, 500);
        y.magnitude = read_ch(2, 500);
        z.magnitude = read_ch(3, 500);

        printf("%d, %d, %d\n", x.magnitude, y.magnitude, z.magnitude);
        if(x.magnitude > BASELINE*1.01){
            if(!x.paused){
                playSnare();
                x.paused = true;
                x.pauseStartTime = getTimeInMs();
            }
        }
        else if(x.magnitude <= 0.8*BASELINE){
            if(x.paused){
                x.paused = false;
            }
        }


        if(y.magnitude > BASELINE*1.01){
            if(!y.paused){
                playHiHat();
                y.paused = true;
                y.pauseStartTime = getTimeInMs();
            }
        }
        else if(y.magnitude <= 0.8*BASELINE){
            if(y.paused){
                y.paused = false;
            }
        }



        if(z.magnitude > BASELINE*1.3){
            if(!z.paused){
                playBase();
                z.paused = true;
                z.pauseStartTime = getTimeInMs();
            }
        }
        else if(z.magnitude <= 0.8*BASELINE){
            if(z.paused){
                z.paused = false;
            }
        }

        long long x_elapsedTime = getTimeInMs() - x.pauseStartTime;
        long long y_elaspedTime = getTimeInMs() - y.pauseStartTime;
        long long z_elapsedTime = getTimeInMs() - z.pauseStartTime;

        if(x_elapsedTime >= DEBOUNCE_TIME){
            if(x.paused){
                x.paused = false;
            }
        }

        if(y_elaspedTime >= DEBOUNCE_TIME){
            if(y.paused){
                y.paused = false;
            }
        }

        if(z_elapsedTime >= DEBOUNCE_TIME){
            if(z.paused){
                z.paused = false;
            }
        }
        usleep(100000);
    }

    return arg;
}


void accelerometer_cleanup(void){
    assert(is_initialized);
    is_initialized = false;
    pthread_join(accelThread, NULL);
    reading = false;
    
}
