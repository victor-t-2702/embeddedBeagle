#ifndef ACCESS_ACCELEROMETER_H
#define ACCESS_ACCELEROMETER_H

#include <stdint.h>
#include <gpiod.h>
#include <stdbool.h>

void accelerometer_init(void);
void playSound(void);
void accelerometer_cleanup(void);

#endif