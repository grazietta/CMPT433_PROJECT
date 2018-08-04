/*
sends a string message to a local NodeJs server using UDP
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

#define SERVER "192.168.7.1"
#define PORT 12345

void *sendMessage(char message[])
{
    struct sockaddr_in si_other;

    int s, i, slen = sizeof(si_other);
    //create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("error: could not establish connection\n");
    }
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_aton(SERVER, &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    //bind socket to port
    if (bind(s, (struct sockaddr *)&si_other, sizeof(si_other)) == -1)
    {
        printf("error: could not bind socket to port\n");
    }
    else
    {
        printf("Success!\n");
    }
    if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == -1)
    {
         printf("error: could not send message\n");
    }
    else
    {
        printf("Success!\n");
    }

    close(s);
}