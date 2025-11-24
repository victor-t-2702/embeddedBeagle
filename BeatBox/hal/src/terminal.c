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
#include "hal/audioMixer.h"

static bool terminalRunning = false; // flag to ensure terminal output thread should be running
static pthread_t terminal_thread; // thread for terminal output

// Thread function for terminal output using periodTimer.c and other HAL modules
static void* terminalAgent(void* arg) {
    sleep(1);

    while(terminalRunning) {
        printf("M%d, %dBPM\n", getBeatType(), getBPM());
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