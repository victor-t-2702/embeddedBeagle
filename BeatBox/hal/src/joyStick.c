// joyStick.c has the implementations of the functions defined in joyStick.h.
// These functions interact via SPI to initialize, read, and clean up after SPI utilization.
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include "hal/accessSPI.h"
#include "hal/audioMixer.h"
#include "hal/joyStick.h"


static bool is_initialized = false; // bool to easily check if joystick is initialized
static bool joystick_on = false;

JoyDir getJoyDir(uint32_t speed_hz) { // We always want to check both channels (i.e. x and y directions)
    assert(is_initialized); // check if joystick has been properly initialized
    
    int joyVal_X = read_ch(4, speed_hz); // check channel 4 (X direction)

    int joyVal_Y = read_ch(5, speed_hz); // check channel 5 (Y direction)


    static int yLowCount = 0;       // persists between calls
    if (joyVal_Y < 1500) {
        yLowCount++;
        if (yLowCount > 10) {       // ~10 × loop delay (5ms) = 50ms
            return DOWN;
        }
    } else {
        yLowCount = 0;
    }

    if (joyVal_Y <= 2600 && joyVal_Y >= 1500 && joyVal_X <= 2600 && joyVal_X >= 1500) {
        return CENTER;
    }
    else if (joyVal_Y > 2600) {
        return UP;
    }
    else if (joyVal_X > 2600) {
        return RIGHT;
    }
    else if (joyVal_X < 1500) {
        return LEFT;
    }
    else {
        return UNDEFINED;
    }
}

static void* JoyStick_thread(void *arg) {
    uint32_t speed_hz = 250000;
    JoyDir playerDir;

    while (joystick_on) {
        playerDir = getJoyDir(speed_hz); // record what direction player pressed
        if (playerDir == CENTER || playerDir == RIGHT || playerDir == LEFT) { 
            usleep(5000); // if no input or invalid input, restart loop
        }
        else if (playerDir == UP) {
            AudioMixer_setVolume((AudioMixer_getVolume()+5));
            usleep(250000); // 0.25 sec
        }
        else if (playerDir == DOWN) {
            AudioMixer_setVolume((AudioMixer_getVolume()-5));
            usleep(250000); // 0.25 sec
        }
    }
    return arg;
}

static pthread_t joystickThreadID;

void joystick_init() {
    assert(!joystick_on);
    joystick_on = true;
    is_initialized = true;
    // Launch joystick polling thread:
    if (pthread_create(&joystickThreadID, NULL, JoyStick_thread, NULL) != 0) {
        perror("Failed to create joystick thread");
        return;
    }
}

void joystickStop() {
    assert(joystick_on);
    assert(is_initialized);
    joystick_on = false;
    pthread_join(joystickThreadID, NULL);
    is_initialized = false;
}

