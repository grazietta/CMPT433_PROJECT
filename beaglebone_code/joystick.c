/*When the joystick is moved up, down, righ left,
or pushed down, it selects one of 5 modes which
represent a different animation on the LEDs*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "message.h"

#define JOYSTICK_UP_FILE "/sys/class/gpio/gpio26/value"
#define JOYSTICK_DOWN_FILE "/sys/class/gpio/gpio46/value"
#define JOYSTICK_RIGHT_FILE "/sys/class/gpio/gpio47/value"
#define JOYSTICK_LEFT_FILE "/sys/class/gpio/gpio65/value"
#define JOYSTICK_PUSH_FILE "/sys/class/gpio/gpio27/value"

#define JOYSTICK_UP_VALUE 26
#define JOYSTICK_DOWN_VALUE 46
#define JOYSTICK_LEFT_VALUE 65
#define JOYSTICK_RIGHT_VALUE 47
#define JOYSTICK_PUSH_VALUE 27
#define max_length 1024

static pthread_t joystick_id;

static void writeToExport(int GPIO_VALUE)
{
    FILE *pfile = fopen("/sys/class/gpio/export", "w");
    if (pfile == NULL)
    {
        printf("ERROR: Unable to open the file.\n");
        exit(1);
    }
    fprintf(pfile, "%d", GPIO_VALUE);
    fclose(pfile);
}

static char getJoyStickValue(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("ERROR: Unable to open %s for read\n", fileName);
        exit(-1);
    }
    char buffer[max_length];
    fgets(buffer, max_length, file);
    fclose(file);
    return buffer[0];
}

static char getJoystickUpValue(void)
{
    return getJoyStickValue(JOYSTICK_UP_FILE);
}

static char getJoystickDownValue(void)
{
    return getJoyStickValue(JOYSTICK_DOWN_FILE);
}

static char getJoystickRightValue(void)
{
    return getJoyStickValue(JOYSTICK_RIGHT_FILE);
}

static char getJoystickLeftValue(void)
{
    return getJoyStickValue(JOYSTICK_LEFT_FILE);
}

static char getJoystickPushValue(void)
{
    return getJoyStickValue(JOYSTICK_PUSH_FILE);
}

static void delay(void)
{
    double seconds = 0.5;
    long nanoseconds = seconds * 1000000000L;
    struct timespec reqDelay = {0, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}

static void *joystick_thread(void *arg)
{
    writeToExport(JOYSTICK_UP_VALUE);
    writeToExport(JOYSTICK_DOWN_VALUE);
    writeToExport(JOYSTICK_RIGHT_VALUE);
    writeToExport(JOYSTICK_LEFT_VALUE);
    writeToExport(JOYSTICK_PUSH_VALUE);
    while (1)
    {
        if (getJoystickUpValue() == '0')
        {
            printf("mode1\n");
            sendMessage("mode1");
            delay();
        }
        if (getJoystickDownValue() == '0')
        {
            printf("mode2\n");
            sendMessage("mode2");
            delay();
        }
        if (getJoystickRightValue() == '0')
        {
            printf("mode3\n");
            sendMessage("mode3");
            delay();
        }
        if (getJoystickLeftValue() == '0')
        {
            printf("mode4\n");
            sendMessage("mode4");
            delay();
        }
        if (getJoystickPushValue() == '0')
        {
            printf("mode5\n");
            sendMessage("mode5");
            delay();
        }
    }
    pthread_exit(0);
}

void joystick_init(void)
{
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_create(&joystick_id, &attributes, joystick_thread, NULL);
}

void joystick_cleanup(void)
{
    pthread_join(joystick_id, NULL);
}
