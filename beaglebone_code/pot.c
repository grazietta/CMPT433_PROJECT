/* Reads analog input voltage 0 on the BeagleBone
Converts reading to different brightness levels which
change the brightness of the LEDs. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "message.h"

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static int previous_reading = 0;

static pthread_t pot_id;

int getVoltage0Reading()
{
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
    if (!f)
    {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf("try:   echo BB_ADC > /sys/devices/platform/bone_capemgr/slots\n");
        exit(-1);
    }

    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0)
    {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }

    // Close file
    fclose(f);

    return a2dReading;
}

int convertReading()
{
    int voltageReading = getVoltage0Reading();

    if (0 <= voltageReading && voltageReading < 820)
    {
        return 1;
    }
    if (820 <= voltageReading && voltageReading < 1640)
    {
        return 2;
    }
    else if (1640 <= voltageReading && voltageReading < 2460)
    {
        return 3;
    }
    else if (2460 <= voltageReading && voltageReading < 3280)
    {
        return 4;
    }
    else if (3280 <= voltageReading && voltageReading < 4100)
    {
        return 5;
    }
    return 0;
}

static void *pot_thread(void *arg)
{
    while (true)
    {
        int reading = convertReading();
        if (reading == 1)
        {
            if (previous_reading != 1)
            {
                sendMessage("bright1");
                printf("bright1\n");
                //set brightness to 51
            }
            previous_reading = 1;
        }
        if (reading == 2)
        {
            if (previous_reading != 2)
            {
                printf("bright2\n");
                sendMessage("bright2");
                //set brightness to 102
            }
            previous_reading = 2;
        }
        if (reading == 3)
        {
            if (previous_reading != 3)
            {
                printf("bright3\n");
                sendMessage("bright3");
                //set brightness to 153
            }
            previous_reading = 3;
        }
        if (reading == 4)
        {
            if (previous_reading != 4)
            {
                printf("bright4\n");
                sendMessage("bright4");
                //set brightness to 204
            }
            previous_reading = 4;
        }
        if (reading == 5)
        {
            if (previous_reading != 5)
            {
                printf("bright5\n");
                sendMessage("bright5");
                //set brightness to 255
            }
            previous_reading = 5;
        }
    }
    pthread_exit(0);
}

void pot_init(void)
{
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_create(&pot_id, &attributes, pot_thread, NULL);
}

void pot_cleanup(void)
{
    pthread_join(pot_id, NULL);
}