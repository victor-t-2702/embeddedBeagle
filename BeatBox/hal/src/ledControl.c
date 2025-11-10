// ledControl.c has the implementations of the functions defined in ledControl.h. 
// These functions interact with the BeagleY-AI's LEDs to open, adjust, and close their files to alter trigger modes and brightness values.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hal/ledControl.h"

#define GREEN_TRIGGER "/sys/class/leds/ACT/trigger"
#define GREEN_BRIGHTNESS "/sys/class/leds/ACT/brightness"
#define RED_TRIGGER "/sys/class/leds/PWR/trigger"
#define RED_BRIGHTNESS "/sys/class/leds/PWR/brightness"



// Change the LED trigger mode
void setLedTrigger(const char* triggerType, const char colour) { // use a pointer to a string literal as parameter (Passing char[] would decay to a pointer anyway, and we don't need to modify it either)
    FILE* pLedTriggerFile = NULL;
    
    if (colour == 'g') {
        pLedTriggerFile = fopen(GREEN_TRIGGER, "w"); // fopen() allocates space for a FILE-type struct and returns a pointer (FILE*) to it
    }
    else if (colour == 'r') {
        pLedTriggerFile = fopen(RED_TRIGGER, "w");
    }
    else {
        perror("Invalid LED trigger mode");
        exit(EXIT_FAILURE);
    }
    
    if (pLedTriggerFile == NULL) { // If the pointer is NULL, then the file wasn't opened
        perror("Failed to open LED trigger file"); // perror() looks at global variable errno (set by the last failed system/library call, like fopen() in this case) to tell what went wrong
        exit(EXIT_FAILURE); // Essentially << return 1; >> from main() (ie. program ended due to an error)
    }

    int triggerWritten = fprintf(pLedTriggerFile, triggerType); // fprintf() writes to a file stream and returns the number of characters successfully written (returns negative if error)

    if (triggerWritten < 0) {
        perror("Failed to write to LED trigger file");
        exit(EXIT_FAILURE);
    }

    fclose(pLedTriggerFile);
}

// Change the LED brightness
void setLedBrightness(const char* value, const char colour) {
    FILE* pLedBrightnessFile = NULL;

    if (colour == 'g') {
        pLedBrightnessFile = fopen(GREEN_BRIGHTNESS, "w");
    }
    else if (colour == 'r') {
        pLedBrightnessFile = fopen(RED_BRIGHTNESS, "w");
    }
    else {
        perror("Invalid LED colour");
        exit(EXIT_FAILURE);
    }
    
    if (pLedBrightnessFile == NULL) {
        perror("Failed to open LED brightness file");
        exit(EXIT_FAILURE);
    }

    int brightnessWritten = fprintf(pLedBrightnessFile, value);

    if (brightnessWritten < 0) {
        perror("Failed to write to LED brightness file");
        exit(EXIT_FAILURE);
    }

    fclose(pLedBrightnessFile);
}
