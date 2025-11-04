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
#include <unistd.h>
#include <time.h>
//#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/accessRot.h"
//#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/udp.h"


int main() {
 

    if (spi_init("/dev/spidev0.0", 500) != 0) {
        fprintf(stderr, "SPI initialization failed\n");
        return 1;
    }

    startPolling(); // Start rotary encoder polling thread

    sampling_init();

    startLightDipsDetect();

    udp_start();

    while(programActive) { // flag set by UDP thread to end program execution when user sends "stop" command

    }
    
    endPolling();
    endLightDipsDetect();
    sampling_cleanup();
    spi_close();
    udp_stop();
    
    return 0;

    // int total = getTotalSample();
    // printf("%d total samples here\n", total);
    // int size = 0; 
    // double* data = getSamplerHistory(&size);  

    // if (data != NULL && size > 0) {
    //     for(int i = 0; i < size && i < 20; i++) {  
    //     printf("%f\n", data[i]);
    //     }
    //     free(data); 
    // } else {
    //     printf("No data available\n");
    // }

    // printf("There are %d samples in the history buffer\n", size);

    // double average = getSampleAverage();

    // printf("This is the average %f\n", average);

    // printf("There have been %d light dips in the past second\n", getDips());
    

}


