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

    //Opens the enable file that would be editted, setting it to 1 to allow the led settings to be modified
    FILE *pEnableFile = fopen(ENABLE_SETTING, "w");
    if (pEnableFile == NULL) {
        perror("Error opening enable file");
        exit(EXIT_FAILURE);
    }

    int enable = fprintf(pEnableFile, "1");

    //Check if file is opened correctly
    if(enable <= 0){
        perror("Error writing to enable file");
        exit(EXIT_FAILURE);
    }
    fclose(pEnableFile);
}

//Function for setting the period
void set_period(int period)
{
    assert(is_initialized);

    //Open the file to be ready to write
    FILE *pPeriodFile = fopen(period_setting, "w");
    if (pPeriodFile == NULL) {
        perror("Error opening period file");
        exit(EXIT_FAILURE);
    }
        
    //Write in the function argument to the file
    int setting = fprintf(pPeriodFile, "%d", period);
    if (setting <= 0) {
        perror("Error writing data to period file");
        exit(EXIT_FAILURE);
    }

    //Close the period file
    fclose(pPeriodFile);

}

//Function for setting the duty cycle
void set_duty_cycle(int duty_cycle)
{
    assert(is_initialized);

    //Open the duty cycle setting file
    FILE *pDutyCycleFile = fopen(duty_cycle_setting, "w");
    if (pDutyCycleFile == NULL) {
        perror("Error opening duty cycle file");
        exit(EXIT_FAILURE);
    }
        
    //Write the function input into the duty cycle file
    int setting = fprintf(pDutyCycleFile, "%d", duty_cycle);
    if (setting <= 0) {
        perror("Error writing data to duty_cycle file");
        exit(EXIT_FAILURE);
    }

    //Close the duty cycle file
    fclose(pDutyCycleFile);

}

//The cleanup function
void PWM_cleanup(void)
{
    //Set our initialized variable to false
    assert(is_initialized);
    is_initialized = false;

    //Open the enable file and write in 0 to prevent modification
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

    //Close the enable file
    fclose(pEnableFile);
 
}