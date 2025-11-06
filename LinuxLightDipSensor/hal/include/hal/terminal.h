// terminal.h defines the Terminal Output HAL module.
// These functions continuously print output of important data using printf()
#ifndef TERMINAL_H
#define TERMINAL_H

// Create terminal output thread
void terminal_start(void); 

// Close and clean up terminal output thread
void terminal_stop(void); 

#endif
