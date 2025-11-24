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

static bool isPaused = false;

static pthread_t accelThread;

static void* detectMovement(void* arg);

typedef struct{
    int x;
    int y;
    int z;
}direction;

direction movementVector;


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
    movementVector.x = 0;
    movementVector.y = 0;
    movementVector.z = 0;
    long long startTime = 0;
    while(reading){
        movementVector.x = read_ch(1, 500);
        movementVector.y = read_ch(2, 500);
        movementVector.z = read_ch(3, 500);

        //printf("%d, %d, %d\n", movementVector.x, movementVector.y, movementVector.z);

        if(movementVector.x > BASELINE*1.01){
            if(!isPaused){
                playBase();
                isPaused = true;
                startTime = getTimeInMs();
            }
        }
        else if(movementVector.x <= BASELINE){
            if(isPaused){
                isPaused = false;
            }
        }


        if(movementVector.y > BASELINE*1.01){
            if(!isPaused){
                playHiHat();
                isPaused = true;
                startTime = getTimeInMs();
            }
        }
        else if(movementVector.y <= BASELINE){
            if(isPaused){
                isPaused = false;
            }
        }



        if(movementVector.z > BASELINE*1.3){
            if(!isPaused){
                playSnare();
                isPaused = true;
                startTime = getTimeInMs();
            }
        }
        else if(movementVector.z <= BASELINE){
            if(isPaused){
                isPaused = false;
            }
        }


        if(getTimeInMs()-startTime >= DEBOUNCE_TIME){
            if(isPaused){
                isPaused = false;
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
