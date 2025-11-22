// accessRot.h has the definitions of the functions implemented in accessRot.c.
// These functions interact via GPIO to initialize, read, and clean up after GPIO utilization (mainly for Rotary Encoder usage).
#ifndef ACCESS_ROT_H
#define ACCESS_ROT_H

#include <stdint.h>
#include <gpiod.h>
#include <stdbool.h>

// Rotary encoder needs 2 GPIO lines
#define ROT_LINES 2

// Struct that holds all necessary info about the currently initialized rotary encoder
typedef struct {
    struct gpiod_chip *chip;
    struct gpiod_line_request *request;
    unsigned int offsets[ROT_LINES];
    int lastA;
    int pulses;
} rotary_t;

// Start polling thread
void startPolling(); 

// End polling thread
void endPolling(); 

// Initialize rotary encoder
int rotary_init(rotary_t *rot, const char *chip_path, unsigned int pinA, unsigned int pinB);

// Clean up
void rotary_close(rotary_t *rot);

#endif

