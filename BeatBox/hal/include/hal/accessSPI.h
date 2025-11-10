// accessSPI.h defines the functions and enum type implementated in accessSPI.c and exposes them to main.c. 
// These functions interact via SPI to initialize, read, and clean up after SPI utilization.
#ifndef ACCESS_SPI_H
#define ACCESS_SPI_H

#include <stdint.h>



typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTER,
    UNDEFINED
} JoyDir;

int spi_init(const char* dev, uint32_t speed_hz);

void spi_close(int fd);

JoyDir getJoyDir(int fd, uint32_t speed_hz);

#endif