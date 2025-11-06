//This HAL module is used to control the flashing of an LED using PWM
//1. Call PWM_init to intiailize the module so allow all other functions to be used
//2. Call set_period to set the period of the PWM
//3. Call set_duty_cycle to set the duty cycle of the PWM
//4. Call PWM_cleanup to stop using the module and prevent further modifications to the PWM
#ifndef _PWM_H_
#define _PWM_H_


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