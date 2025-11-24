// udp.h has the definitions of the functions implemented in udp.c.
// These functions interact via UDP to incorporate communication between the host and the target.
#ifndef UDP_H
#define UDP_H

#include <pthread.h>
#include <stdbool.h>

#define UDP_PORT 12345

extern volatile bool programActive; // to end program execution through UDP

// Create UDP thread
void udp_start(void); 

// Close and clean up UDP thread
void udp_stop(void); 

#endif
