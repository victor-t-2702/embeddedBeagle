#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include "hal/udp.h"
#include "hal/lightDips.h"
#include "hal/sampling.h"
//#include "/home/victor/embeddedBeagle/work/LinuxLightDipSensor/hal/include/hal/udp.h"


volatile bool programActive = true;
static bool running = false;
static pthread_t udp_thread;
static int sockfd = -1; // network socket file descriptor (socket() returns it)

static void* udp_listener(void* arg) {
    struct sockaddr_in server_addr, client_addr; // server_addr = where we'll listen, client_addr = where incoming packets come from
    char buffer[64]; // memory to store incoming commands
    socklen_t len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Create a new socket of type SOCK_DGRAM (UDP) in domain AF_INET (ipv4) with antomatically chosen protocol
    if (sockfd < 0) {
        perror("socket");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr)); // zeroes out all bytes of server_addr struct
    server_addr.sin_family = AF_INET; // ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // listen to all network interfaces (ex. eth0, usb0)
    server_addr.sin_port = htons(UDP_PORT); // convert port number between host and network byte order (ie. little/big endian)

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { // Give the socket FD the local address ADDR (which is LEN bytes long)
        perror("bind");
        close(sockfd);
        pthread_exit(NULL);
    }

    printf("[UDP] Listening on port %d...\n", UDP_PORT);
    char previousCommand[30] = "";
    while (running) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, // waits (blocking) for an incoming UDP packet
                             (struct sockaddr*)&client_addr, &len); // fills buffer with the message, client_addr with sender's address
        if (n < 0) continue;

        buffer[n] = '\0'; // null terminate to treat as C string
        printf("[UDP] Received: %s\n", buffer); // print received message

        buffer[strcspn(buffer, "\r\n")] = 0; // remove trailing newline return from UDP buffer

        if (strcasecmp(buffer, "") == 0) {
            strcpy(buffer, previousCommand); // buffer becomes last command if user enters <enter>
            const char* enterReply = "<enter> pressed. Repeating last command.\n";
            sendto(sockfd, enterReply, strlen(enterReply), 0,
                   (struct sockaddr*)&client_addr, len);
        }

        if (strcasecmp(buffer, "help") == 0 || strcasecmp(buffer, "?") == 0) { // use strcasecmp() to compare buffer with known commands
            const char* reply =
                "Supported commands:\n"
                "count - get the total number of samples taken\n"
                "length - get the number of samples taken in the previously completed second\n"
                "dips - get the number of dips in the previously completed second\n"
                "history - get all the samples in the previously completed second\n"
                "stop - cause the server program to end\n"
                "<enter> - repeat last command\n\n";
            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, len);
            strcpy(previousCommand, buffer);
        }
        else if (strcasecmp(buffer, "count") == 0) { 
            char reply[64];
            long long sampleCount = getTotalSample();
            snprintf(reply, sizeof(reply), "Samples taken total: %lld\n\n", sampleCount);
            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, len);
            strcpy(previousCommand, buffer);
        }
        else if (strcasecmp(buffer, "length") == 0) { 
            char reply[64];
            int size = 0;
            double* sampleHistory = getSamplerHistory(&(size));
            snprintf(reply, sizeof(reply), "Samples taken in the previously completed second: %d\n\n", size);
            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, len);
            free(sampleHistory);
            strcpy(previousCommand, buffer);
        }      
        else if (strcasecmp(buffer, "dips") == 0) { 
            char reply[64];
            int numDips = getDips();
            snprintf(reply, sizeof(reply), "Number of dips in the previously completed second: %d\n\n", numDips);
            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, len);
            strcpy(previousCommand, buffer);
        }       
        else if (strcasecmp(buffer, "history") == 0) { 
            int size = 0;
            double* sampleHistory = getSamplerHistory(&(size));            
            char reply[(10*size)]; // assume each sample will use up 10 chars
            int offset = 0;
            int newLine = 0;
            for (int i = 0; i < size; i++) {
                if (newLine == 9) {
                    offset += snprintf(reply + offset, sizeof(reply) - offset, "\n"); // new line every 10 samples (makes output look good)
                    newLine = 0;
                }
                offset += snprintf(reply + offset, sizeof(reply) - offset, "%.3f, ", sampleHistory[i]); // ensure that buffer does not overflow
                newLine++;
            }  
            offset += snprintf(reply + offset, sizeof(reply) - offset, "\n\n");          
            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, len);
            free(sampleHistory);
            strcpy(previousCommand, buffer);
        }   
        else if (strcasecmp(buffer, "stop") == 0) { 
            char reply[64];
            snprintf(reply, sizeof(reply), "Program terminating.\n");
            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, len);
            programActive = false;
        }    
        else {
            const char* reply = "Unknown command. Type 'help'.\n\n";
            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, len);
        }
    }

    close(sockfd);
    pthread_exit(NULL);
    return arg;
}

void udp_start(void) {
    assert(!running);
    running = true;
    pthread_create(&udp_thread, NULL, udp_listener, NULL);
}

void udp_stop(void) {
    assert(running);
    running = false;
    if (sockfd > 0) {
        close(sockfd);
    }
    //pthread_join(udp_thread, NULL);
}