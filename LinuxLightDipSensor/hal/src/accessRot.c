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
#include "hal/accessRot.h"
#include "hal/PWM.h"
//#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/accessRot.h"

static bool is_initialized = false;
static bool polling_on = false;
pthread_t pollThread;


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
    printf("Rotary encoder initialized on offsets %u, %u.\n", pinA, pinB);
    is_initialized = true;
    return 0;
}

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
            rot->pulses++;  // Clockwise
        else
            rot->pulses--;  // Counter-clockwise
        if (rot->pulses < 0) {
            rot->pulses = 0;
        }
        printf("A=%d B=%d pulses=%d\n", A, B, rot->pulses);
    }


    rot->lastA = A;
    return rot->pulses;
}

void rotary_close(rotary_t *rot)
{
    assert(is_initialized);
    if (rot->request)
        gpiod_line_request_release(rot->request);
    if (rot->chip)
        gpiod_chip_close(rot->chip);
    is_initialized = false;
}

static rotary_t rot;

// Thread function that initializes Rotary Encoder and sets PWM duty cycle off polling encoder
static void* pollForPWM(void *arg) {
    PWM_init();
    set_period(100000000); // start off at 10Hz
    set_duty_cycle(0);
    set_duty_cycle(1000000000);
    rot.pulses = 10; // starts off at 10Hz
    int previousPulses = 0; // to check if encoder was actually turned
    int pwmPeriod = (int)(1000000000.0 / rot.pulses); // convert between frequency and period
    long seconds = 0;
    long nanoseconds = 1000000; 
    struct timespec reqDelay = {seconds, nanoseconds}; // Define a delay of 1ms
    if (rotary_init(&rot, "/dev/gpiochip1", 41, 33) < 0) {
        return NULL;
    }
    rot.pulses = 10; // starts off at 10Hz
    while (polling_on) {
        rotary_poll(&rot);

        // If no change, sleep and continue
        if (rot.pulses == previousPulses) {
            nanosleep(&reqDelay, NULL);
            continue;
        }

        // Clamp frequency range
        if (rot.pulses > 500) {
            rot.pulses = 500;
        }

        if (rot.pulses <= 0) {
            // Zero frequency - turn off LED
            rot.pulses = 0;
            set_duty_cycle(0);
            // Do NOT set_period here; leave previous frequency
        } else {
            // Compute new period
            pwmPeriod = (int)(1000000000.0 / rot.pulses);

            // 50% duty cycle based on the new period
            set_duty_cycle(pwmPeriod / 2);

            // Apply period
            set_period(pwmPeriod);
        }

        // Track the new state
        previousPulses = rot.pulses;

        // Sleep to avoid CPU hogging
        nanosleep(&reqDelay, NULL);
}


    rotary_close(&rot);
    return arg;
}

void startPolling() { // Start polling thread
    assert(!polling_on);
    polling_on = true;
    if (pthread_create(&pollThread, NULL, pollForPWM, NULL) != 0) {
        perror("Failed to create polling thread");
        return;
    }
}

void endPolling() { // End polling thread
    assert(polling_on);
    polling_on = false;
}

int freqExpose() {
    assert(polling_on);
    int freq = rot.pulses;
    return freq;
}
