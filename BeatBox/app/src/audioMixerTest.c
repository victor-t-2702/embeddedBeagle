#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "hal/audioMixer.h"
#include "hal/accessRot.h"


int main(void)
{
    printf("Initializing audio mixer...\n");
    AudioMixer_init(); // start beat sequencing and playback thread
    startPolling(); // Start rotary encoder polling thread
    extern int beatType;
    extern int BPM;

    while(1) {
        sleep(10);
        beatType = 2;
        sleep(5);
        beatType = 1;
        sleep(10);
        BPM = 200;
        sleep(10);
        
    }

    printf("Cleaning up...\n");
    endPolling(); // end rotary encoder thread
    AudioMixer_cleanup(); // Cleans up and stops playback and beat sequencing threads

    return 0;
}