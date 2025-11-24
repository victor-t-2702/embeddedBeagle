// accessSPI.h defines the functions and enum type implementated in accessSPI.c and exposes them to main.c. 
// These functions interact via SPI to initialize, read, and clean up after SPI utilization.
#ifndef JOY_STICK_H
#define JOY_STICK_H

#include <stdint.h>

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTER,
    UNDEFINED
} JoyDir;



void joystick_init();



// Pool SPI to get direction joystick is pushed
JoyDir getJoyDir(uint32_t speed_hz);

// Stop joystick thread and clean up SPI
void joystickStop();

#endif