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
#include "hal/udp.h"
#include "hal/periodTimer.h"
#include "hal/terminal.h"
#include <unistd.h>
#include <time.h>

int main() {
 
    Period_init();
    if (spi_init("/dev/spidev0.0", 500) != 0) {
        fprintf(stderr, "SPI initialization failed\n");
        return 1;
    }

    startPolling(); // Start rotary encoder polling thread

    sampling_init(); // Start sampling thread

    startLightDipsDetect(); //Start light dips detection thread

    udp_start(); //Start udp thread

    terminal_start(); //Start terminal output thread

    while(programActive) { // flag set by UDP thread to end program execution when user sends "stop" command

    }
    
    //Call the cleanup functions for all the HAL modules used
    terminal_stop();
    endPolling();
    endLightDipsDetect();
    sampling_cleanup();
    spi_close();
    udp_stop();
    Period_cleanup();
    
    return 0;

}


