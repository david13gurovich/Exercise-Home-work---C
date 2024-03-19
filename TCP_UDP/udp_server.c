#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024
#define MY_IP "10.1.0.223"
#define LOOP_BACK "127.0.0.1"
#define BRODCAST "10.1.255.255"

void InitServerAddr(struct sockaddr_in *server_addr)
{
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
}

int main()
{
    int sockfd;
    int n;
    socklen_t len;
    char buffer[MAXLINE];
    const char *pong = "maayan - server";
    struct sockaddr_in servaddr, cliaddr;
    int opt = 1;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    if (inet_pton(AF_INET, MY_IP, &servaddr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        perror("inet_pton");
        return(EXIT_FAILURE);
    } 
    
    InitServerAddr(&servaddr);
    
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        return(EXIT_FAILURE);
    }
    
    while (1)
    {
        n = recvfrom(sockfd, buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        buffer[n] = '\0';
        printf("from Client : %s\n", buffer);
        sendto(sockfd, pong, strlen(pong), MSG_CONFIRM,
               (const struct sockaddr *)&cliaddr, len);
        printf("Server Sent : %s\n", pong);
        sleep(2);
    }

    return (EXIT_SUCCESS);
}