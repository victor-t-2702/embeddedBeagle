#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "hal/audioMixer.h"
#include "hal/accessRot.h"
#include "hal/accessSPI.h"
#include "hal/accelerometer.h"
#include "hal/terminal.h"
#include "hal/joyStick.h"
#include "hal/periodTimer.h"

int main(void)
{
    printf("Initializing audio mixer...\n");
    Period_init();
    AudioMixer_init(); // start beat sequencing and playback thread
    spi_init("/dev/spidev0.0", 500);
    accelerometer_init();
    startPolling(); // Start rotary encoder polling thread
    joystick_init();
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
    Period_cleanup();
    //endPolling(); // end rotary encoder thread
    spi_close();
    AudioMixer_cleanup(); // Cleans up and stops playback and beat sequencing threads   
    accelerometer_cleanup();
    joystickStop();

    return 0;
}