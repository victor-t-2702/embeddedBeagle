// Main program to build the application
// Has main(); does initialization and cleanup and basic game logic.
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "hal/accessRot.h"
#include "hal/PWM.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/accessRot.h"

bool polling_on = false;
pthread_t pollThread;

// Thread function that initializes PWM and Rotary Encoder, and then sets PWM duty cycle off polling encoder
void* pollForPWM(void *arg) {
    PWM_init();
    set_period(100000000); // start off at 10Hz
    set_duty_cycle(0);
    set_duty_cycle(800000);
    rotary_t rot;
    rot.pulses = 10; // starts off at 10Hz
    int previousPulses = 10; // to check if encoder was actually turned
    int pwmPeriod = (int)((double)1000000000*((double)1/(double)rot.pulses)); // convert between frequency and period
    long seconds = 0;
    long nanoseconds = 1000000; 
    struct timespec reqDelay = {seconds, nanoseconds}; // Define a delay of 1ms
    if (rotary_init(&rot, "/dev/gpiochip1", 41, 33) < 0) {
        return NULL;
    }

    while (polling_on) { // poll until main() sets polling_on to false
        rotary_poll(&rot);
        if (rot.pulses == previousPulses) {
            continue;
        }
        else if (rot.pulses <= 0) { // can't have negative frequency
            rot.pulses = 0;
            set_duty_cycle(0); // simply turn off LED
        }
        else if (rot.pulses > 500) { // max of 500Hz
            rot.pulses = 500;
            pwmPeriod = (int)((double)1000000000*((double)1/(double)rot.pulses));
        }
        else {
            pwmPeriod = (int)((double)1000000000*((double)1/(double)rot.pulses));
        }

        set_period(pwmPeriod);
        nanosleep(&reqDelay, NULL); // 1 kHz poll rate (reqDelay should be 1ms)
    }

    rotary_close(&rot);
    PWM_cleanup();
}

int main() {
    
    //start Rotary Encoder polling thread
    polling_on = true;
    if (pthread_create(&pollThread, NULL, pollForPWM, NULL) != 0) {
        perror("Failed to create polling thread");
        return 1;
    }
     

    sleep(15);


   // Clean up modules
   polling_on = false;
   return 0;
}


