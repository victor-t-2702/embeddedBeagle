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
#include "hal/sampling.h"
#include "hal/accessSPI.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
//#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/accessRot.h"

bool polling_on = false;
pthread_t pollThread;

// Thread function that initializes PWM and Rotary Encoder, and then sets PWM duty cycle off polling encoder
void* pollForPWM(void *arg) {
    PWM_init();
    set_period(1000000000); // start off at 10Hz
    set_duty_cycle(0);
    set_duty_cycle(1000000000);
    rotary_t rot;
    int previousPulses = 10; // to check if encoder was actually turned
    int pwmPeriod = (int)((double)1000000000*((double)1/(double)rot.pulses)); // convert between frequency and period
    long seconds = 0;
    long nanoseconds = 1000000; 
    struct timespec reqDelay = {seconds, nanoseconds}; // Define a delay of 1ms
    if (rotary_init(&rot, "/dev/gpiochip1", 41, 33) < 0) {
        return NULL;
    }
    rot.pulses = 10; // starts off at 10Hz
    while (polling_on) {
    rotary_poll(&rot);
    
    // Skip if no change
    if (rot.pulses == previousPulses) {
        continue;
    }
    
    // Clamp frequency range
    if (rot.pulses > 500) {
        rot.pulses = 500;
    }
    
    // Handle LED and PWM based on frequency
    if (rot.pulses == 0) {
        // Zero frequency - turn off LED completely
        set_duty_cycle(0);
        pwmPeriod = 0;
    } else {
        // Non-zero frequency - enable LED and set period
        set_duty_cycle(100000000);  // Fixed duty cycle
        pwmPeriod = (int)(1000000000.0 / rot.pulses);
    }
    
    set_period(pwmPeriod);
    previousPulses = rot.pulses;
    nanosleep(&reqDelay, NULL);
}

    rotary_close(&rot);
    PWM_cleanup();
    return arg;
}

int main() {
    
     //start Rotary Encoder polling thread
     polling_on = true;
     if (pthread_create(&pollThread, NULL, pollForPWM, NULL) != 0) {
         perror("Failed to create polling thread");
         return 1;
     }

    if (spi_init("/dev/spidev0.0", 500) != 0) {
        fprintf(stderr, "SPI initialization failed\n");
        return 1;
    }

    sampling_init();

    sleep(1);

    // Clean up modules

    int total = getTotalSample();
    printf("%d totla here\n", total);
    int size = 0;  // ✅ Create an actual integer
    double* data = getSamplerHistory(&size);  // ✅ Pass address of the integer

    if (data != NULL && size > 0) {
        for(int i = 0; i < size && i < 20; i++) {  // ✅ Use actual size, with bounds check
        printf("%f\n", data[i]);
        }
        free(data);  // ✅ Don't forget to free!
    } else {
        printf("No data available\n");
    }

    printf("There are %d samples in the history buffer\n", size);

    double average = getSampleAverage();

    printf("This is the average %f\n", average);

    sampling_cleanup();
    spi_close();
    polling_on = false;
    return 0;


}


