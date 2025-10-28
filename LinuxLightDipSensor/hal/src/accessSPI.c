// accessSPI.c has the implementations of the functions defined in accessSPI.h.
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
#include "hal/accessSPI.h"


static bool is_initialized = false; // bool to easily check if joystick is initialized
static int fd;
// Initialize SPI device and return file descriptor
int spi_init(const char* dev, uint32_t speed_hz) {
    assert(!is_initialized); // check if SPI device is already initialized
    
    //const char* dev = "/dev/spidev0.0"; // point to the SPI 0 device
    uint8_t mode = 0;   // SPI mode 0
    uint8_t bits = 8;
    //uint32_t speed_hz = 250000;

    fd = open(dev, O_RDWR);  // open is a low-level system call that returns a raw file descriptor (which is what ioctl() wants), not a FILE* like fopen() returns

    if (fd < 0) {  // if open() fails, it returns -1
        perror("open");
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) {  // SPI_IOC_WR_MODE = tells the kernel: “set the SPI mode (clock polarity, phase, etc.)”, with &mode being a pointer to the desired SPI mode
        perror("mode");  // If setting SPI mode fails (ie. ioctl() returns -1, throw an error)
        return -1; 
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {  // set bits per SPI word
        perror("bpw");
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz) == -1) {  // set SPI clock speed
        perror("speed");
        return -1;
    }

    is_initialized = true;
    return fd;
}

// Read a channel from MCP3208 ADC chip
int read_ch(int ch, uint32_t speed_hz) {  // marked static because only functions within this file (eg. getJoyDir()) will use it
    usleep(500); // sleep for 500 microseconds before doing a read to avoid garbage SPI data
    
    assert(is_initialized); // check if joystick has been properly initialized
    
    uint8_t tx[3] = { (uint8_t)(0x06 | ((ch & 0x04) >> 2)), (uint8_t)((ch & 0x03) << 6), 0x00 };  // tx and rx arrays are pointers (uint8_t *)
    uint8_t rx[3] = { 0 }; // buffer for the received data

    struct spi_ioc_transfer tr = {  // type of struct defined in <linux/spi/spidev.h> for the Linux SPI driver
        .tx_buf = (unsigned long)tx,  // cast to unsigned long because the kernel API expects integer types (__u64), not pointer types for ioctl
        .rx_buf = (unsigned long)rx,  // store received bytes here
        .len = 3,              // Number of bytes to transfer (3 bytes for the MCP3208)
        .speed_hz = speed_hz,
        .bits_per_word = 8, // Usually 8 bits per SPI word
        .cs_change = 0  // Whether to toggle CS after this transfer (0 = keep CS low until done)
    };

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {  // SPI_IOC_MESSAGE(n) is a macro from <linux/spi/spidev.h> that tells the kernel you want to perform n SPI transfers in one go
        return -1;
    }

    return ((rx[1] & 0x0F) << 8) | rx[2];  // full 12 bit result
}


void spi_close(void) {
    assert(is_initialized);
    is_initialized = false;
    close(fd);
}

// JoyDir getJoyDir(int fd, uint32_t speed_hz) { // We always want to check both channels (i.e. x and y directions)
//     assert(is_initialized); // check if joystick has been properly initialized
    
//     int joyVal_X = read_ch(0, speed_hz); // check channel 0 (X direction)
//     //printf("X = %d", joyVal_X);
//     int joyVal_Y = read_ch(1, speed_hz); // check channel 1 (Y direction)
//     //printf("Y = %d", joyVal_Y);

//     if (joyVal_Y <= 2600 && joyVal_Y >= 1500 && joyVal_X <= 2600 && joyVal_X >= 1500) {
//         return CENTER;
//     }
//     else if (joyVal_Y > 2600) {
//         return UP;
//     }
//     else if (joyVal_Y < 1500) {
//         return DOWN;
//     }
//     else if (joyVal_X > 2600) {
//         return RIGHT;
//     }
//     else if (joyVal_X < 1500) {
//         return LEFT;
//     }
//     else {
//         return UNDEFINED;
//     }
// }

