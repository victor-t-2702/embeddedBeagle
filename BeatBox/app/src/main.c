// Main program to build the application
// Has main(); does initialization and cleanup and basic game logic.
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "hal/ledControl.h"
#include "hal/accessSPI.h"



long randDel(long min, long max) { // Generate a random delay between min and max values
    if (min >= max) {
        return min;
    }
    double scale = (double)rand() / RAND_MAX;  // Create a scaling value bewteen 0 and 1
    return min + (long)((max - min) * scale); // Scale the range defined by the min and max inputs and then add min value to compute finalized random value
}

long randNum(long min, long max) { // Generate a random number between min and max values
    if (min >= max) {
        return min;
    }
    return min + (long)rand() % (max - min + 1);
}


void ledOff() { // turn off LEDs
    setLedBrightness("0", 'r'); 
    setLedBrightness("0", 'g');
}

void closeAll(int fd) { // Function called before program is quit
    ledOff();
    spi_close(fd); // clean up SPI
}





int main() {
    srand(time(NULL)); // Ensure a unique seed for rand()
    
    long seconds = 0;
    long nanoseconds = 250000000; 
    struct timespec reqDelay = {seconds, nanoseconds}; // Define a delay of 250ms
    struct timespec start, end; // Define 2 struct timespecs, one for start and one for end
    struct timespec flashDelay = {(long)0, (long)90000000}; // Define 0.1 sec delay for flashing lights on correct/incorrect joystick press (in reality, sleep 0.09 sec to compensate for I/O lag)
    int elapsed; // define a variable to store how much time it takes the player to press the joystick
    int bestTime = 1000000000; // Pick a large bestTime, so first round is always best time
    JoyDir correctDir;
    int dirNum; // Direction respresented by a 1 or 0

    // Initialize all modules; HAL modules first
    uint32_t speed_hz = 250000;
    const char* dev = "/dev/spidev0.0"; // point to the SPI 0 device (joystick)
    const int fd = spi_init(dev, speed_hz); // Initialize joystick over SPI

    // Turn off any LED triggers
    setLedTrigger("none", 'r');
    setLedTrigger("none", 'g');

    // Turn off the LEDs
    ledOff();

    // Intro Message
    printf("Are you ready to test your reaction time? No? TOO BAD!! When the LEDs light up, press the joystick in that direction\n\n");

    // Main program logic:
    while (true) {
        printf("Get ready...\n");
        for (int i = 0; i < 4; i++) {
            setLedBrightness("1", 'g');
            nanosleep(&reqDelay, NULL); // sleep for 250ms
            setLedBrightness("0", 'g');
            setLedBrightness("1", 'r');
            nanosleep(&reqDelay, NULL);
            setLedBrightness("0", 'r');
        }

        if (getJoyDir(fd, speed_hz) != CENTER) {
            printf("Please let go of joystick\n\n");
            while (getJoyDir(fd, speed_hz) != CENTER) { // Wait until player is not pressing the joystick
                nanosleep(&reqDelay, NULL);
            }
        }

        long nanoDelay = randDel(500000000, 3000000000); // Define a random delay between 0.5 and 3 seconds (in nanoseconds)
        struct timespec randDelay = {nanoDelay / 1000000000, nanoDelay % 1000000000}; // Split delay into seconds and nanoseconds (if .tv_nsec > 1,000,000,000, then nanosleep() will fail silently)
        nanosleep(&randDelay, NULL); // Wait a random amount of time between 0.5 and 3 seconds
        JoyDir playerDir = getJoyDir(fd, speed_hz); // record what direction player pressed

        if (playerDir != CENTER) {
            if(playerDir == LEFT || playerDir == RIGHT) {
                printf("User selected to quit.\n");
                closeAll(fd); // clean up
                return 0;
            }
            printf("Too soon\n\n");
            continue; // Restart game if player moves joystick too early
        }
      
        dirNum = randNum(0, 1); // Pick a random direction (up or down), represented by a 0 or 1
        
        switch (dirNum) {
        case 0:
            correctDir = UP;
            printf("Press UP!\n\n");
            setLedBrightness("1", 'g');
            break;

        case 1:
            correctDir = DOWN;
            printf("Press DOWN!\n\n");
            setLedBrightness("1", 'r');
            break;
        
        default:
            printf("Correct joystick direction is invalid\n\n");
            closeAll(fd); // clean up
            return -1;
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &start); // Get the current value of the monotonic clock (only moves forward) and store it in the "start" struct timespec

        ledOff(); // turn off LEDs
        while (true) {
            playerDir = getJoyDir(fd, speed_hz); // record what direction player pressed
            clock_gettime(CLOCK_MONOTONIC, &end);
            elapsed = (end.tv_sec - start.tv_sec)*1000 + (end.tv_nsec - start.tv_nsec) / 1000000; // Calculate elapsed time in ms (divide nanoseconds member variable by 10^6)
            if (playerDir == CENTER) { 
                if (elapsed > 5000) {
                    printf("Took too long!\n\n"); // If player does not make a move in 5 seconds, end program
                    closeAll(fd); // clean up
                    return 0;
                }
                continue; // if no input, restart loop
            }
            else if (playerDir == correctDir) {
                printf("Correct!\n");
                if (elapsed < bestTime) {
                    printf("New best time!\n");
                    bestTime = elapsed; // new best time set
                }
                printf("Your reaction time was %dms; best so far is %dms\n\n", elapsed, bestTime);
                for (int j = 0; j < 5; j++) { // flash green LED on/off five times in one second
                    setLedBrightness("1", 'g');
                    nanosleep(&flashDelay, NULL); // sleep for 0.1 sec
                    setLedBrightness("0", 'g');
                    nanosleep(&flashDelay, NULL); // sleep for 0.1 sec
                }
                break; // move back to main game loop
            }
            else if (playerDir != LEFT && playerDir != RIGHT) {
                printf("Incorrect...\n\n");
                for (int j = 0; j < 5; j++) { // flash red LED on/off five times in one second
                    setLedBrightness("1", 'r');
                    nanosleep(&flashDelay, NULL); // sleep for 0.1 sec
                    setLedBrightness("0", 'r');
                    nanosleep(&flashDelay, NULL); // sleep for 0.1 sec
                }
                break; // move back to main game loop
            }
            else { // quit program if left or right are pressed
                printf("User selected to quit.\n");
                closeAll(fd); // clean up
                return 0;
            }
        }
           
    }
    // Cleanup all modules (HAL modules last)
    closeAll(fd); // clean up
    

}