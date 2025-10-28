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



int main() {
    // long seconds = 0;
    // long nanoseconds = 1000000; 
    // struct timespec reqDelay = {seconds, nanoseconds}; // Define a delay of 250ms
    // rotary_t rot;
    // if (rotary_init(&rot, "/dev/gpiochip1", 41, 33) < 0)
    //     return 1;

    // while (1) {
    //     rotary_poll(&rot);
    //     nanosleep(&reqDelay, NULL); // 1 kHz poll rate
    // }

    // rotary_close(&rot);
    // return 0;

    PWM_init();
    set_period(1000000);
    set_duty_cycle(0);
    set_duty_cycle(800000);
}


