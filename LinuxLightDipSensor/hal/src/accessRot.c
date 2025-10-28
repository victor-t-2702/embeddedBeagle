// accessRot.c has the implementations of the functions defined in accessRot.h.
// These functions interact via GPIO to initialize, read, and clean up after GPIO utilization (mainly for Rotary Encoder usage).
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gpiod.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "hal/accessRot.h"
//#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/accessRot.h"

int rotary_init(rotary_t *rot, const char *chip_path, unsigned int pinA, unsigned int pinB)
{
    struct gpiod_line_settings *settings = NULL;
    struct gpiod_line_config *config = NULL;

    rot->offsets[0] = pinA;
    rot->offsets[1] = pinB;
    rot->pulses = 0;

    rot->chip = gpiod_chip_open(chip_path);
    if (!rot->chip) {
        perror("gpiod_chip_open");
        return -1;
    }

    settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);

    config = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(config, rot->offsets, ROT_LINES, settings);

    rot->request = gpiod_chip_request_lines(rot->chip, NULL, config);
    if (!rot->request) {
        perror("gpiod_chip_request_lines");
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(config);
        gpiod_chip_close(rot->chip);
        return -2;
    }

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(config);

    enum gpiod_line_value vals[ROT_LINES];
    if (gpiod_line_request_get_values(rot->request, vals) < 0) {
        perror("gpiod_line_request_get_values");
        return -3;
    }

    rot->lastA = vals[0];
    printf("Rotary encoder initialized on offsets %u, %u.\n", pinA, pinB);
    return 0;
}

int rotary_poll(rotary_t *rot)
{
    enum gpiod_line_value vals[ROT_LINES];
    if (gpiod_line_request_get_values(rot->request, vals) < 0) {
        perror("gpiod_line_request_get_values");
        return rot->pulses;
    }

    int A = vals[0];
    int B = vals[1];

    if (A != rot->lastA) {
        if (B != A)
            rot->pulses++;  // Clockwise
        else
            rot->pulses--;  // Counter-clockwise

        printf("A=%d B=%d pulses=%d\n", A, B, rot->pulses);
    }

    rot->lastA = A;
    return rot->pulses;
}

void rotary_close(rotary_t *rot)
{
    if (rot->request)
        gpiod_line_request_release(rot->request);
    if (rot->chip)
        gpiod_chip_close(rot->chip);
}
