// accessSPI.h defines the functions and enum type implementated in accessSPI.c and exposes them to main.c. 
// These functions interact via SPI to initialize, read, and clean up after SPI utilization.
#ifndef ACCESS_SPI_H
#define ACCESS_SPI_H

#include <stdint.h>


int spi_init(const char* dev, uint32_t speed_hz, bool *is_initialized);

int read_ch(int fd, int ch, uint32_t speed_hz, bool is_initialized);

void spi_close(int fd, bool *is_initialized);


#endif