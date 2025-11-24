// udp.c has the implementations of the functions defined in udp.h.
// These functions interact via UDP to incorporate communication between the host and the target.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <strings.h>
#include <sys/socket.h>
#include <assert.h>
#include "hal/udp.h"
#include "hal/audioMixer.h"


volatile bool programActive = true; // flag that is used to end program through UDP
static bool running = false; // flag to ensure thread should be running
static pthread_t udp_thread; // UDP thread
static int sockfd = -1; // network socket file descriptor (socket() returns it)

// Thread function that listens and responds to port 12345 through UDP 
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
        sockfd = -1;
        pthread_exit(NULL);
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000; // 100 ms timeout
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt(SO_RCVTIMEO)");
    }
    
    while (running) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, // waits (blocking) for an incoming UDP packet
                             (struct sockaddr*)&client_addr, &len); // fills buffer with the message, client_addr with sender's address
        if (n < 0) {
            // recvfrom() returns -1 if socket was closed
            if (!running) break;  // exiting cleanly
            continue;
        }

        buffer[n] = '\0'; // null terminate to treat as C string
        printf("[UDP] Received: %s\n", buffer); // print received message

        buffer[strcspn(buffer, "\r\n")] = 0; // remove trailing newline return from UDP buffer

        // strstr returns a pointer to the first occurrence of subString in mainString,
        // or NULL if subString is not found.
        if (strstr(buffer, "volume") != NULL) {
            if (strstr(buffer, "NULL") != NULL) {
                char reply[6];
                sprintf (reply, "%d", AudioMixer_getVolume());
                sendto(sockfd, reply, strlen(reply), 0,
                        (struct sockaddr*)&client_addr, len);
            }
            else {
                int newVol;
                int assigned_items = sscanf(buffer, "volume %d", &newVol);

                if (assigned_items == 1) {
                    AudioMixer_setVolume(newVol);
                    char reply[6];
                    sprintf (reply, "%d", AudioMixer_getVolume());
                    sendto(sockfd, reply, strlen(reply), 0,
                        (struct sockaddr*)&client_addr, len);
                } else {
                    printf("Failed to parse all values.\n");
                }
            }
        } 
        else if (strstr(buffer, "tempo") != NULL) {
            if (strstr(buffer, "NULL") != NULL) {
                char reply[6];
                sprintf (reply, "%d", getBPM());
                sendto(sockfd, reply, strlen(reply), 0,
                        (struct sockaddr*)&client_addr, len);
            }
            else {
                int newBPM;
                int assigned_items = sscanf(buffer, "tempo %d", &newBPM);

                if (assigned_items == 1) {
                    setBPM(newBPM);
                    char reply[6];
                    sprintf (reply, "%d", getBPM());
                    sendto(sockfd, reply, strlen(reply), 0,
                        (struct sockaddr*)&client_addr, len);
                } else {
                    printf("Failed to parse all values.\n");
                }
            }
        } 
        else if (strstr(buffer, "mode") != NULL) {
            if (strstr(buffer, "NULL") != NULL) {
                char reply[6];
                sprintf (reply, "%d", getBeatType());
                sendto(sockfd, reply, strlen(reply), 0,
                        (struct sockaddr*)&client_addr, len);
            }
            else {
                int newBeatType;
                int assigned_items = sscanf(buffer, "mode %d", &newBeatType);
                
                if (assigned_items == 1) {
                    setBeatType(newBeatType);
                    char reply[6];
                    sprintf (reply, "%d", getBeatType());
                    sendto(sockfd, reply, strlen(reply), 0,
                        (struct sockaddr*)&client_addr, len);
                } else {
                    printf("Failed to parse all values.\n");
                }
            }
        }
        else if (strstr(buffer, "play") != NULL) {
            int sound;
            int assigned_items = sscanf(buffer, "play %d", &sound);

            if (assigned_items == 1) {
                if (sound == 0) {
                    playBase();
                }
                else if (sound == 1) {
                    playHiHat();
                }
                else if (sound == 2) {
                    playSnare();
                }
                // char reply[6];
                // sprintf (reply, "%d", getBPM());
                // sendto(sockfd, reply, strlen(reply), 0,
                //     (struct sockaddr*)&client_addr, len);
            } else {
                printf("Failed to parse all values.\n");
            }
        } 
        else if (strstr(buffer, "stop") != NULL) {
            int val;
            int assigned_items = sscanf(buffer, "stop %d", &val);

            if (assigned_items == 1) {
                if (val == 0) {
                    programActive = false;
                // char reply[6];
                // sprintf (reply, "%d", getBPM());
                // sendto(sockfd, reply, strlen(reply), 0,
                //     (struct sockaddr*)&client_addr, len);
            } else {
                printf("Failed to parse all values.\n");
            }
        } 
    }
    sockfd = -1;
    return arg;
}

// Start UDP thread
void udp_start(void) {
    assert(!running);
    running = true;
    pthread_create(&udp_thread, NULL, udp_listener, NULL);
}

// End UDP thread
void udp_stop(void) {
    assert(running);
    running = false;
    if (sockfd > 0) {
        close(sockfd);
        sockfd = -1;
    }
    pthread_join(udp_thread, NULL);
}