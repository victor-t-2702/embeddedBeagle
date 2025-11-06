// terminal.c has the implementations of the functions defined in terminal.h.
// These functions continuously print output of important data using printf()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include <pthread.h>
#include "hal/terminal.h"
#include "hal/lightDips.h"
#include "hal/sampling.h"
#include "hal/accessRot.h"
#include "hal/periodTimer.h"

static bool terminalRunning = false; // flag to ensure terminal output thread should be running
static pthread_t terminal_thread; // thread for terminal output

// Thread function for terminal output using periodTimer.c and other HAL modules
static void* terminalAgent(void* arg) {
    int numSamples = 0;
    int dips = 0;
    double avg = 0.0;
    int PWM_freq = 0;
    Period_statistics_t pStats = {0};
    while(terminalRunning) {
        double *sampleHistory = getSamplerHistory(&numSamples);
        avg = getSampleAverage();
        dips = getDips();
        PWM_freq = freqExpose();
        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &pStats);
        printf("#Smpl/s = %3d     Flash @ %3dHz     avg = %5.3fV     dips = %3d     Smpl ms[%5.3f, %5.3f] avg %5.3f/%3d\n", numSamples, PWM_freq, avg, dips, pStats.minPeriodInMs, pStats.maxPeriodInMs, pStats.avgPeriodInMs, pStats.numSamples);  // 7.2 and 8.9 ARE PLACEHOLDERS FOR TIMING JITTER STUFF
        int increment = (numSamples / 10);
        if (increment <= 0) {
            increment = 1;
        }
        int max = 0;
        for (int i = 0; i < numSamples; i += increment) {
            if (max >= 10) {
                break;
            }
            printf("%3d:%5.3f    ", i, sampleHistory[i]);
            max++;
        }
        printf("\n");
        free(sampleHistory);
        sleep(1);
    }
    return arg;
}

// Start terminal output thread
void terminal_start(void) {
    assert(!terminalRunning);
    terminalRunning = true;
    pthread_create(&terminal_thread, NULL, terminalAgent, NULL);
}

// End terminal output thread
void terminal_stop(void) {
    assert(terminalRunning);
    terminalRunning = false;
}