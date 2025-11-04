#ifndef UDP_H
#define UDP_H

#include <pthread.h>
#include <stdbool.h>

#define UDP_PORT 12345

extern volatile bool programActive; // to end program execution through UDP

void udp_start(void); // Create UDP thread
void udp_stop(void); // Close and clean up UDP thread

#endif
