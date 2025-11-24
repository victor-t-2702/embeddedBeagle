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
static int fd = 0;

JoyDir getJoyDir(int fd, uint32_t speed_hz) { // We always want to check both channels (i.e. x and y directions)
    assert(is_initialized); // check if joystick has been properly initialized
    
    int joyVal_X = read_ch(fd, 0, speed_hz, is_initialized); // check channel 0 (X direction)
    //printf("X = %d", joyVal_X);
    int joyVal_Y = read_ch(fd, 1, speed_hz, is_initialized); // check channel 1 (Y direction)
    //printf("Y = %d", joyVal_Y);

    if (joyVal_Y <= 2600 && joyVal_Y >= 1500 && joyVal_X <= 2600 && joyVal_X >= 1500) {
        return CENTER;
    }
    else if (joyVal_Y > 2600) {
        return UP;
    }
    else if (joyVal_Y < 1500) {
        return DOWN;
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
    const char* dev = "/dev/spidev0.0"; // point to the SPI 0 device (joystick)
    fd = spi_init(dev, speed_hz, &is_initialized); // Initialize joystick over SPI
    JoyDir playerDir;

    while (joystick_on) {
        playerDir = getJoyDir(fd, speed_hz); // record what direction player pressed
        if (playerDir == CENTER || playerDir == RIGHT || playerDir == LEFT) { 
            usleep(5000); // if no input or invalid input, restart loop
        }
        else if (playerDir == UP) {
            AudioMixer_setVolume((AudioMixer_getVolume()+5));
            usleep(50000); // 50 ms
        }
        else if (playerDir == DOWN) {
            AudioMixer_setVolume((AudioMixer_getVolume()-5));
            usleep(50000); // 50 ms
        }
    }
    return arg;
}

static pthread_t joystickThreadID;

void joystick_init() {
    assert(!joystick_on);
    joystick_on = true;
    // Launch joystick polling thread:
    if (pthread_create(&joystickThreadID, NULL, JoyStick_thread, NULL) != 0) {
        perror("Failed to create joystick thread");
        return;
    }
}

static void joystickCleanup() {
    spi_close(fd, &is_initialized);
}

void joystickStop() {
    assert(joystick_on);
    joystick_on = false;
    pthread_join(joystickThreadID, NULL);
    joystickCleanup();
}

