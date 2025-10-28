#ifndef _PWM_H_
#define _PWM_H_

#include <stdbool.h>

void PWM_init(void);
void set_period(int period);
void set_duty_cycle(int duty_cycle);
void PWM_cleanup(void);

#endif