// accessSPI.h defines the functions and enum type implementated in accessSPI.c and exposes them to main.c. 
// These functions interact via SPI to initialize, read, and clean up after SPI utilization.
#ifndef ACCESS_SPI_H
#define ACCESS_SPI_H

#include <stdint.h>
#include <stdbool.h>



typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTER,
    UNDEFINED
} JoyDir;

bool spi_is_ready(void);

int spi_init(const char* dev, uint32_t speed_hz);

void spi_close(void);

int read_ch(int ch, uint32_t speed_hz);

JoyDir getJoyDir(int fd, uint32_t speed_hz);

#endif