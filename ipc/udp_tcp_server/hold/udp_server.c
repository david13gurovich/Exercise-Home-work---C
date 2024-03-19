#include <sys/types.h> //bind, recvfrom, sendto
#include <sys/socket.h> //socket, AF_INET, bind, recvfrom, sendto
#include <string.h> //memset
#include <arpa/inet.h> //htons
#include <stdlib.h> //exit_status
#include <unistd.h> //sleep

#include "udp_server.h"
#include "logger.hpp"

#define PORT 8090
#define MAXLINE 1024

extern int g_EXIT_FROM_PROG;
const int FAIL = -1;

void InitServerAddr(struct sockaddr_in *server_addr)
{
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

    while ((0 ==__sync_add_and_fetch((size_t *)&g_EXIT_FROM_PROG, 0)))
    {
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
}

void *StartUDPServer(void *params)
{
    int sockfd = 0;
    struct sockaddr_in servaddr;

    ilrd::Logger *log = (ilrd::Logger *)params;
    *log << ilrd::Logger::INFO << "open UDP server" << std::endl;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (FAIL == sockfd)
    {
        *log << ilrd::Logger::ERROR << "socket failed";
        return (NULL);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    InitServerAddr(&servaddr);

    if (FAIL == bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        *log << ilrd::Logger::ERROR << "bind failed";
        return (NULL);
    }

    SendAndRecieveUDP(sockfd, log);

    return (NULL);
}