#ifndef UDP_H
#define UDP_H

#include <pthread.h>
#include <stdbool.h>

#define UDP_PORT 12345

void* udp_listener(void* arg); // Thread function to listen to UDP
void udp_start(void); // Create thread
void udp_stop(void); // Close and clean up thread

#endif
