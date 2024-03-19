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
#define EINAT_IP "10.1.0.209"
#define GIL_IP "10.1.0.17"
#define MY_IP "10.1.0.223"
#define RAN_IP "10.1.0.125"
#define LIOR_IP "10.1.0.188"
#define LOOP_BACK "127.0.0.1"
#define BROADCAST "10.1.255.255"

int main()
{
    int sockfd = 0;
    int n = 0;
    socklen_t len = {0};
    char buffer[MAXLINE] = {0};
    const char *ping = "maayan - clinet";
    struct sockaddr_in servaddr;
    int broadcastEnable = 1;
    int ret;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

/*     servaddr.sin_addr.s_addr = INADDR_ANY; 
 */
    if (inet_pton(AF_INET, MY_IP, &servaddr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        perror("inet_pton");
        return(EXIT_FAILURE);
    }

    ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, 
                                                sizeof(broadcastEnable));
    if (ret) 
    {
        close(sockfd);
        return(EXIT_FAILURE);
    }

    while (1)
    {
        sendto(sockfd, ping, strlen(ping), MSG_CONFIRM,
               (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
        printf("Client Sent : %s\n", ping);
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';
        printf("from Server : %s\n", buffer);
        sleep(1);
    }

    close(sockfd);

    return (EXIT_SUCCESS);
}