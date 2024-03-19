#include <stdlib.h> //exit_status
#include <sys/types.h> //bind, recvfrom, sendto
#include <sys/socket.h> //socket, AF_INET, bind, recvfrom, sendto
#include <string.h> //memset
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h> //htons
#include <unistd.h> //sleep

#include "logger.hpp"
#include "udp_server.h"

#define PORT 8070
#define MAXLINE 1024

const int FAIL = -1;

static void InitServerAddr(struct sockaddr_in *server_addr)
{
    memset(server_addr, 0, sizeof(*server_addr));

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
}

void SendAndRecieveUDP(int sockfd, ilrd::Logger *log)
{
    char buffer[MAXLINE];
    const char *msg_to_send = "udp server";
    int num_of_bytes = 0;
    int status = 0;
    socklen_t len = MAXLINE;
    struct sockaddr_in cliaddr;

    memset(&cliaddr, 0, sizeof(cliaddr));

    num_of_bytes = (recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, 
                                (struct sockaddr *)&cliaddr, &len));
    if (FAIL == num_of_bytes)
    {
        *log << ilrd::Logger::INFO << "recvfrom failed" << std::endl;
        return;
    }
    buffer[num_of_bytes] = '\0';

    status = (sendto(sockfd, msg_to_send, strlen(msg_to_send)+1, MSG_CONFIRM,
                                        (const struct sockaddr *)&cliaddr, len));
    if (FAIL == status)
    {
        *log << ilrd::Logger::INFO << "sendto failed" << std::endl;
        return;
    }
}

int CreateUdpServerAndBind(int *udp_socket, ilrd::Logger *log)
{
    struct sockaddr_in servaddr; 

    *udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (FAIL == *udp_socket)
    {
        *log << ilrd::Logger::ERROR << "socket failed";
        return (EXIT_FAILURE);
    }
    InitServerAddr(&servaddr);
    
    if (FAIL == bind(*udp_socket, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        *log << ilrd::Logger::ERROR << "bind failed" << strerror(errno) <<std::endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}