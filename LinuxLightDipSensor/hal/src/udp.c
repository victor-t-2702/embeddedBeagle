#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "hal/udp.h"



// static bool running = false;
// static pthread_t udp_thread;
// static int sockfd = -1; // network socket file descriptor (socket() returns it)

// void* udp_listener(void* arg) {
//     struct sockaddr_in server_addr, client_addr; // server_addr = where we'll listen, client_addr = where incoming packets come from
//     char buffer[64]; // memory to store incoming commands
//     socklen_t len = sizeof(client_addr);

//     sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Create a new socket of type SOCK_DGRAM (UDP) in domain AF_INET (ipv4) with antomatically chosen protocol
//     if (sockfd < 0) {
//         perror("socket");
//         pthread_exit(NULL);
//     }

//     memset(&server_addr, 0, sizeof(server_addr)); // zeroes out all bytes of server_addr struct
//     server_addr.sin_family = AF_INET; // ipv4
//     server_addr.sin_addr.s_addr = INADDR_ANY; // listen to all network interfaces (ex. eth0, usb0)
//     server_addr.sin_port = htons(UDP_PORT); // convert port number between host and network byte order (ie. little/big endian)

//     if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { // Give the socket FD the local address ADDR (which is LEN bytes long)
//         perror("bind");
//         close(sockfd);
//         pthread_exit(NULL);
//     }

//     printf("[UDP] Listening on port %d...\n", UDP_PORT);

//     while (running) {
//         ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, // waits (blocking) for an incoming UDP packet
//                              (struct sockaddr*)&client_addr, &len); // fills buffer with the message, client_addr with sender's address
//         if (n < 0) continue;

//         buffer[n] = '\0'; // null terminate to treat as C string
//         printf("[UDP] Received: %s\n", buffer); // print received message

//         // handle "help" command
//         if (strcasecmp(buffer, "help") == 0 || strcasecmp(buffer, "?") == 0) { // use strcasecmp() to compare buffer with known commands
//             const char* reply =
//                 "Supported commands:\n"
//                 "count - get the total number of samples taken\n"
//                 "length - get the number of samples taken in the previously completed second\n"
//                 "dips - get the number of dips in the previously completed second\n"
//                 "history - get all the samples in the previously completed second\n"
//                 "stop - cause the server program to end\n"
//                 "<enter> - repeat last command\n";
//             sendto(sockfd, reply, strlen(reply), 0,
//                    (struct sockaddr*)&client_addr, len);
//         }
//         else if (strcasecmp(buffer, "count") == 0) { 
//             char reply[64];
//             snprintf(reply, sizeof(reply), "Samples taken total: %d\n", sample_count); // PLACEHOLDER FOR TAYLORS SAMPLE COUNT
//             sendto(sockfd, reply, strlen(reply), 0,
//                    (struct sockaddr*)&client_addr, len);
//         }
//         else {
//             const char* reply = "Unknown command. Type 'help'.\n";
//             sendto(sockfd, reply, strlen(reply), 0,
//                    (struct sockaddr*)&client_addr, len);
//         }
//     }

//     close(sockfd);
//     pthread_exit(NULL);
// }

// void udp_start(void) {
//     running = true;
//     pthread_create(&udp_thread, NULL, udp_listener, NULL);
// }

// void udp_stop(void) {
//     running = false;
//     if (sockfd > 0) {
//         close(sockfd);
//     }
//     pthread_join(udp_thread, NULL);
// }

// static bool running = false;
// static pthread_t udp_thread;
// static int sockfd = -1;