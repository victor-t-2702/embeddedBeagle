// accessSPI.h defines the functions and enum type implementated in accessSPI.c and exposes them to main.c. 
// These functions interact via SPI to initialize, read, and clean up after SPI utilization.
#ifndef ACCESS_SPI_H
#define ACCESS_SPI_H

#include <stdint.h>
#include <stdbool.h>

// Function that exposes static bool is_initialized
bool spi_is_ready(void);

// Initializes the SPI device
int spi_init(const char* dev, uint32_t speed_hz);

// Closes the SPI file and sets the is_initialized flag accordingly
void spi_close(void);

// Reads a channel from the MCP3208 ADC
int read_ch(int ch, uint32_t speed_hz);

#endif