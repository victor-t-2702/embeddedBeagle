#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "hal/audioMixer.h"
//#include "/home/victor/embeddedBeagle/work/BeatBox/hal/include/hal/audioMixer.h"



int main(void)
{
    printf("Initializing audio mixer...\n");
    AudioMixer_init();
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
    AudioMixer_cleanup(); // Cleans up and stops playback and beat sequencing threads

    return 0;
}