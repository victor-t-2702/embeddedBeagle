#define duty_cycle_setting "/dev/hat/pwm/GPIO12/duty_cycle"
#define period_setting "/dev/hat/pwm/GPIO12/period"
#define ENABLE_SETTING "/dev/hat/pwm/GPIO12/enable"

#include "hal/PWM.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Allow module to ensure it has been initialized (once!)
static bool is_initialized = false;

//Intializes the LED
void PWM_init(void) 
{
    //printf("PWM - Initializing\n");
    assert(!is_initialized);
    is_initialized = true;

    FILE *pEnableFile = fopen(ENABLE_SETTING, "w");
    if (pEnableFile == NULL) {
        perror("Error opening enable file");
        exit(EXIT_FAILURE);
    }

    int enable = fprintf(pEnableFile, "1");

    if(enable <= 0){
        perror("Error writing to enable file");
        exit(EXIT_FAILURE);
    }
    fclose(pEnableFile);
}

void set_period(int period)
{
    assert(is_initialized);
    FILE *pPeriodFile = fopen(period_setting, "w");
    if (pPeriodFile == NULL) {
        perror("Error opening period file");
        exit(EXIT_FAILURE);
    }
        
    int setting = fprintf(pPeriodFile, "%d", period);
    if (setting <= 0) {
        perror("Error writing data to period file");
        exit(EXIT_FAILURE);
    }

    fclose(pPeriodFile);

}

void set_duty_cycle(int duty_cycle)
{
    assert(is_initialized);
    FILE *pDutyCycleFile = fopen(duty_cycle_setting, "w");
    if (pDutyCycleFile == NULL) {
        perror("Error opening duty cycle file");
        exit(EXIT_FAILURE);
    }
        
    int setting = fprintf(pDutyCycleFile, "%d", duty_cycle);
    if (setting <= 0) {
        perror("Error writing data to duty_cycle file");
        exit(EXIT_FAILURE);
    }

    fclose(pDutyCycleFile);

}

void PWM_cleanup(void)
{
    assert(is_initialized);
    is_initialized = false;
    FILE *pEnableFile = fopen(ENABLE_SETTING, "w");
    if (pEnableFile == NULL) {
        perror("Error opening enable file");
        exit(EXIT_FAILURE);
    }

    int enable = fprintf(pEnableFile, "0");

    if(enable <= 0){
        perror("Error writing to enable file");
        exit(EXIT_FAILURE);
    }

    fclose(pEnableFile);
 
}