// accessRot.h has the definitions of the functions implemented in accessRot.c.
// These functions interact via GPIO to initialize, read, and clean up after GPIO utilization (mainly for Rotary Encoder usage).
#ifndef ACCESS_ROT_H
#define ACCESS_ROT_H

#include <stdint.h>
#include <gpiod.h>
#include <stdbool.h>

#define ROT_LINES 2

typedef struct {
    struct gpiod_chip *chip;
    struct gpiod_line_request *request;
    unsigned int offsets[ROT_LINES];
    int lastA;
    int pulses;
} rotary_t;

int rotary_init(rotary_t *rot, const char *chip_path, unsigned int pinA, unsigned int pinB);
int rotary_poll(rotary_t *rot);
void rotary_close(rotary_t *rot);

#endif

