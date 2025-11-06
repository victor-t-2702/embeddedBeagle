#ifndef _PWM_H_
#define _PWM_H_

/*This HAL module is used to control the flashing of an LED using PWM*/
#include <stdbool.h>

//Initializes the PWM to be used, needs to be called before any function is used in this module
void PWM_init(void);

//Sets the period of the LED flash
void set_period(int period);

//Sets the duty cycle of the LED flash, the portion of the period that the LED is on for
void set_duty_cycle(int duty_cycle);

//A clean up function that is called after this module is no longer needed
void PWM_cleanup(void);

#endif