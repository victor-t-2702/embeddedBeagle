// accessRot.c has the implementations of the functions defined in accessRot.h.
// These functions interact via GPIO to initialize, read, and clean up after GPIO utilization (mainly for Rotary Encoder usage).
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gpiod.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "hal/audioMixer.h"
#include "hal/accessRot.h"

static bool is_initialized = false; // Flag to ensure module is initialized
static bool polling_on = false; // Flag to ensure thread is supposed to be running
pthread_t pollThread; // Polling thread


// Initializes the rotary gpio chip and lines (2 lines)
int rotary_init(rotary_t *rot, const char *chip_path, unsigned int pinA, unsigned int pinB)
{
    
    assert(!is_initialized);

    struct gpiod_line_settings *settings = NULL;
    struct gpiod_line_config *config = NULL;

    rot->offsets[0] = pinA;
    rot->offsets[1] = pinB;
    rot->pulses = 0;

    rot->chip = gpiod_chip_open(chip_path);
    if (!rot->chip) {
        perror("gpiod_chip_open");
        return -1;
    }

    settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

    config = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(config, rot->offsets, ROT_LINES, settings);

    rot->request = gpiod_chip_request_lines(rot->chip, NULL, config);
    if (!rot->request) {
        perror("gpiod_chip_request_lines");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(config);
        gpiod_chip_close(rot->chip);
        return -2;
    }

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(config);

    enum gpiod_line_value vals[ROT_LINES];
    if (gpiod_line_request_get_values(rot->request, vals) < 0) {
        perror("gpiod_line_request_get_values");
        return -3;
    }

    rot->lastA = vals[0];
    is_initialized = true;
    return 0;
}

//Function to get the current time in ms, used to implement a timer in the pollForPWM for low frequencies
static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

// Polls the rotary encoder (using basic State Machine logic to ensure that one turn clockwise/counter-clockwise increase/decreases by 1)
static int rotary_poll(rotary_t *rot)
{
    assert(is_initialized);
    enum gpiod_line_value vals[ROT_LINES];
    if (gpiod_line_request_get_values(rot->request, vals) < 0) {
        perror("gpiod_line_request_get_values");
        return rot->pulses;
    }

    
    int A = vals[0];
    int B = vals[1];

    if (A == 1 && rot->lastA == 0) {
        if (B != A)
            rot->pulses += 5;  // Clockwise (increase by 5 BPM)
        else
            rot->pulses -= 5;  // Counter-clockwise (decrease by 5 BPM)
        if (rot->pulses < 1) {
            rot->pulses = 1;
        }
    }


    rot->lastA = A;
    return rot->pulses;
}

// Closes gpio chip and frees memory held by respective structs
void rotary_close(rotary_t *rot)
{
    assert(is_initialized);
    if (rot->request)
        gpiod_line_request_release(rot->request);
    if (rot->chip)
        gpiod_chip_close(rot->chip);
    is_initialized = false;
}

// Global rotary_t variable for use within thread function
static rotary_t rot;



// Thread function that initializes Rotary Encoder and sets PWM duty cycle off polling encoder
static void* pollingThread(void *arg) {
    rot.pulses = 100; // starts off at 10 BPM
    int previousPulses = 0; // to check if encoder was actually turned

    long seconds = 0;
    long nanoseconds = 1000000; 
    struct timespec reqDelay = {seconds, nanoseconds}; // Define a delay of 1ms



    if (rotary_init(&rot, "/dev/gpiochip1", 41, 33) < 0) {
        printf("Failed to initialize rotary encoder");
        return NULL;
    }
    rot.pulses = 100; // start at 100 BPM
    while (polling_on) {
        rotary_poll(&rot);

        //If no change, sleep and continue
        if (rot.pulses == previousPulses) {
            nanosleep(&reqDelay, NULL);
            continue;
        }

        setBPM(rot.pulses);

        previousPulses = rot.pulses;

        // Sleep to avoid CPU hogging
        nanosleep(&reqDelay, NULL);
    }
    rotary_close(&rot);
    return arg;
}


// Start polling thread
void startPolling() {
    assert(!polling_on);
    polling_on = true;
    if (pthread_create(&pollThread, NULL, pollingThread, NULL) != 0) {
        perror("Failed to create polling thread");
        return;
    }

}


// End polling thread
void endPolling() { 
    assert(polling_on);
    polling_on = false;
    pthread_join(pollThread, NULL);
}

