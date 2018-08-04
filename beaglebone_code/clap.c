/* 
reads analog input voltage 4 on the BeagleBone and detects whether there is a likelihood of a clap occuring
Assumes ADC cape already loaded:
root@beaglebone:/#  echo BB_ADC > /sys/devices/platform/bone_capemgr/slots
*/

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "list.h"
#include "message.h"

static pthread_t clap_id;

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage4_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define GPIO49_VALUE "/sys/class/gpio/gpio49/value"
#define SHORT_TERM_SAMPLE_SIZE 20
#define LONG_TERM_SAMPLE_SIZE 400
#define THRESHOLD 1.07
#define COOL_DOWN_DELAY 2

/*function partially copied from class (CMPT 433) guides*/
static void writeToFile(char *filename, char *value)
{
	FILE *file = fopen(filename, "w");
	if (file == NULL)
	{
		printf("ERROR: Unable to open file (%s) for read\n", filename);
		exit(-1);
	}
	fprintf(file, "%s", value);
	fclose(file);
}

static int getVoltageReading()
{
	FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
	if (!f)
	{
		printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
		printf("try:   echo BB-ADC > /sys/devices/platform/bone_capemgr/slots\n");
		exit(-1);
	}

	int a2dReading = 0;
	int itemsRead = fscanf(f, "%d", &a2dReading);
	if (itemsRead <= 0)
	{
		printf("ERROR: Unable to read values from voltage input file.\n");
		exit(-1);
	}
	fclose(f);
	return a2dReading;
}

static void delay()
{
	long seconds = COOL_DOWN_DELAY;
	struct timespec reqDelay = {seconds, 0};
	nanosleep(&reqDelay, (struct timespec *)NULL);
}

static double mean(struct nodeStruct *head, double size)
{
	double sum = 0;
	struct nodeStruct *current = head;
	while (current != NULL)
	{
		sum += current->item;
		current = current->next;
	}
	return sum / size;
}

static void *start_reading(void *args)
{
	struct nodeStruct *short_term_list = List_createNode(getVoltageReading());
	struct nodeStruct *long_term_list = List_createNode(getVoltageReading());

	int i, j;

	for (i = 0; i < LONG_TERM_SAMPLE_SIZE - SHORT_TERM_SAMPLE_SIZE; i++)
	{
		List_insertTail(&long_term_list, List_createNode(getVoltageReading()));
	}
	for (j = 0; j < SHORT_TERM_SAMPLE_SIZE; j++)
	{
		List_insertTail(&long_term_list, List_createNode(getVoltageReading()));
		List_insertTail(&short_term_list, List_createNode(getVoltageReading()));
	}

	while (true)
	{
		double clap_likeness = mean(short_term_list, SHORT_TERM_SAMPLE_SIZE) / mean(long_term_list, LONG_TERM_SAMPLE_SIZE);

		if (clap_likeness > THRESHOLD)
		{
			sendMessage("clap");
			printf("clap detected\n");

			struct nodeStruct *temp = long_term_list;

			while (temp != NULL)
			{
				temp->item = getVoltageReading();
				temp = temp->next;
			}

			temp = short_term_list;
			while (temp != NULL)
			{
				temp->item = getVoltageReading();
				temp = temp->next;
			}
			delay();
		}
		int voltage = getVoltageReading();

		List_insertTail(&short_term_list, List_createNode(voltage));
		List_deleteHead(&short_term_list);

		List_insertTail(&long_term_list, List_createNode(voltage));
		List_deleteHead(&long_term_list);
	}
	return NULL;
}

void clap_init(void)
{
	pthread_create(&clap_id, NULL, start_reading, NULL);
}

void clap_cleanup(void)
{
	pthread_join(clap_id, NULL);
}
