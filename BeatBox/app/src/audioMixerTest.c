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
#include "hal/udp.h"


int main(void)
{
    printf("Initializing audio mixer...\n");
    Period_init();
    AudioMixer_init(); // start beat sequencing and playback thread
    spi_init("/dev/spidev0.0", 500);
    accelerometer_init();
    joystick_init();
    startPolling(); // Start rotary encoder polling thread
    terminal_start();
    udp_start();


    while(programActive) {
        sleep(1);
    }

    printf("Cleaning up...\n");
    udp_stop();
    terminal_stop();
    endPolling(); // end rotary encoder thread
    accelerometer_cleanup();
    joystickStop();
    spi_close();
    Period_cleanup();
    AudioMixer_cleanup(); // Cleans up and stops playback and beat sequencing threads   

    return 0;
}