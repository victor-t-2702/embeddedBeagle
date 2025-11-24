// terminal.c has the implementations of the functions defined in terminal.h.
// These functions continuously print output of important data using printf()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include <stdbool.h>
#include <pthread.h>
#include "hal/periodTimer.h"
#include "hal/audioMixer.h"

static bool terminalRunning = false; // flag to ensure terminal output thread should be running
static pthread_t terminal_thread; // thread for terminal output

// Thread function for terminal output using periodTimer.c and other HAL modules
static void* terminalAgent(void* arg) {
    sleep(1);

    while(terminalRunning) {
        Period_statistics_t audioStats;
        Period_statistics_t accelStats;
        Period_getStatisticsAndClear(PERIOD_EVENT_FILL_AUDIO_BUFFER, &audioStats);
        Period_getStatisticsAndClear(PERIOD_ACCELRATION, &accelStats);
        printf("M%d %dBPM vol:%d  Audio[%.3f, %.3f] avg %.3f/%d  Accel[%.3f, %.3f] avg %.3f/%d\n", getBeatType(), getBPM(),AudioMixer_getVolume(), audioStats.minPeriodInMs, audioStats.maxPeriodInMs, audioStats.avgPeriodInMs, audioStats.numSamples, accelStats.minPeriodInMs, accelStats.maxPeriodInMs, accelStats.avgPeriodInMs, accelStats.numSamples);
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
    pthread_join(terminal_thread, NULL);
}