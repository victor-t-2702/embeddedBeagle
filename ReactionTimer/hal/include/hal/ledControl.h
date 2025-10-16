// ledControl.h defines the functions implemented in ledControl.c and exposes them to main.c.
// These functions interact with the BeagleY-AI's LEDs to open, adjust, and close their files to alter trigger modes and brightness values.
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>



void setLedTrigger(const char* triggerType, const char colour);

void setLedBrightness(const char* value, const char colour);

#endif