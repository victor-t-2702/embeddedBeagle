#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include "hal/terminal.h"
#include "hal/lightDips.h"
#include "hal/sampling.h"
#include "hal/accessRot.h"

static bool terminalRunning = false;
static pthread_t terminal_thread;


static void* terminalAgent(void* arg) {
    int numSamples = 0;
    int dips = 0;
    double avg = 0.0;
    int PWM_freq = 0;
    while(terminalRunning) {
        double *sampleHistory = getSamplerHistory(&numSamples);
        avg = getSampleAverage();
        dips = getDips();
        PWM_freq = freqExpose();
        printf("#Smpl/s = %3d     Flash @ %3dHz     avg = %5.3fV     dips = %3d     Smpl ms[%5.3f, %5.3f]\n", numSamples, PWM_freq, avg, dips, 7.2, 8.9);  // 7.2 and 8.9 ARE PLACEHOLDERS FOR TIMING JITTER STUFF
        int increment = (numSamples / 10);
        if (increment <= 0) {
            increment = 1;
        }
        for (int i = 0; i < numSamples; i += increment) {
            printf("%3d:%5.3f    ", i, sampleHistory[i]);
        }
        printf("\n");
        free(sampleHistory);
        sleep(1);
    }
    return arg;
}

void terminal_start(void) {
    assert(!terminalRunning);
    terminalRunning = true;
    pthread_create(&terminal_thread, NULL, terminalAgent, NULL);
}

void terminal_stop(void) {
    assert(terminalRunning);
    terminalRunning = false;
}