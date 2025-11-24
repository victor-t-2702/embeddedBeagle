#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "hal/audioMixer.h"
#include "hal/accessRot.h"
#include "hal/accessSPI.h"
#include "hal/accelerometer.h"
#include "hal/terminal.h"


int main(void)
{
    printf("Initializing audio mixer...\n");
    AudioMixer_init(); // start beat sequencing and playback thread
    spi_init("/dev/spidev0.0", 500);
    accelerometer_init();
    terminal_start();
    //startPolling(); // Start rotary encoder polling thread
    //extern int beatType;
    //extern int BPM;

    while(1) {
        setBeatType(2);
        //beatType = 2;
        sleep(5);
        //beatType = 1;
        sleep(10);
        //BPM = 200;
        sleep(10);
        
    }

    printf("Cleaning up...\n");
    terminal_stop();
    //endPolling(); // end rotary encoder thread
    spi_close();
    AudioMixer_cleanup(); // Cleans up and stops playback and beat sequencing threads   
    accelerometer_cleanup();
    

    return 0;
}