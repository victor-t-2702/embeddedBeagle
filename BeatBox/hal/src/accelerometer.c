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

static bool is_initialized = false;
static bool reading = false;

static pthread_t accelThread;

static void* detectMovement(void* arg);

int xMoveCount = 0;
int yMoveCount = 0;
int zMoveCount = 0;



void accelerometer_init(void){
    assert(!is_initialized);
    is_initialized = true;
    reading = true;
    if (pthread_create(&accelThread, NULL, detectMovement, NULL) != 0) {
        perror("Failed to create beat thread");
        return;
    }
}

static void* detectMovement(void* arg){
    assert(is_initialized);
    int x_val = 0;
    int y_val = 0;
    int z_val = 0;
    while(reading){
        x_val = read_ch(1, 500);
        y_val = read_ch(2, 500);
        z_val = read_ch(3, 500);

        printf("%d,%d,%d\n", x_val, y_val, z_val);

        if(x_val > 2200){

            printf("Move x + 1\n");
            xMoveCount++;
        }

        if(y_val > 2200){
            printf("Move y + 1\n");
            yMoveCount++;
        }

        if(z_val > 2000){
            playBase();
            //printf("Move z + 1\n");
            //zMoveCount++;
        }
        usleep(107860);
    }

    return arg;
}

void playSound(void){
    assert(is_initialized);
    if(zMoveCount > 10){
        playBase();
    }
    if(yMoveCount > 10){
        playHiHat();
    }
    if(xMoveCount > 10){
        playSnare();
    }
}

void accelerometer_cleanup(void){
    assert(is_initialized);
    is_initialized = false;
    pthread_join(accelThread, NULL);
    reading = false;
    
}
