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
#include "hal/lightDips.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
//#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/accessRot.h"

bool polling_on = false;
pthread_t pollThread;

bool writing = false;
pthread_t writeToLightDipThread;


int lightDipsPipe[2];


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

void* writeToLightDips(void* arg){
    lightDips_init(lightDipsPipe[0]);

    // if(pipe(lightDipsPipe) == -1){
    //     perror("Failed to initiliaze light dips analysis pipe");
    //     exit(EXIT_FAILURE);
    // }
    
    while(writing){
        sleep(1);
        historyValues history;

        history.samples = getSamplerHistory(&history.size);  
        history.average = getSampleAverage();

        printf("=== WRITER ===\n");
        printf("New array allocated at: %p\n", (void*)history.samples);
        printf("Size: %d, Average: %f\n", history.size, history.average);

        if (write(lightDipsPipe[1], &history, sizeof(historyValues)) != sizeof(historyValues)) {
            perror("Write to light dip pipe failed");
        }
        // write(lightDipsPipe[1], &history.size, sizeof(int));
        // write(lightDipsPipe[1], &history.average, sizeof(float));
        // write(lightDipsPipe[1], history.samples, history.size * sizeof(float));
    }

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

    //start writing to light dips analysis thread
     writing = true;
     if (pthread_create(&writeToLightDipThread, NULL, writeToLightDips, NULL) != 0) {
         perror("Failed to create write to light dips thread");
         return 1;
     }


    sleep(5);


    int total = getTotalSample();
    printf("%d total samples here\n", total);
    int size = 0; 
    double* data = getSamplerHistory(&size);  

    if (data != NULL && size > 0) {
        for(int i = 0; i < size && i < 20; i++) {  
        printf("%f\n", data[i]);
        }
        free(data); 
    } else {
        printf("No data available\n");
    }

    printf("There are %d samples in the history buffer\n", size);

    double average = getSampleAverage();

    printf("This is the average %f\n", average);



    polling_on = false;
    writing = false;

    lightDips_cleanup();
    close(lightDipsPipe[1]);
    spi_close();
    sampling_cleanup();
    return 0;


}


