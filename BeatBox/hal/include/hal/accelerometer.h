//Module to read the movement of accelerometer, playing a sound when accelerated in a direction

#ifndef ACCESS_ACCELEROMETER_H
#define ACCESS_ACCELEROMETER_H

#include <stdint.h>
#include <gpiod.h>
#include <stdbool.h>

//Initialize
void accelerometer_init(void);

//Clean up the thread 
void accelerometer_cleanup(void);

#endif