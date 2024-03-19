#include <arpa/inet.h> /* inet_pton */
#include <netinet/in.h>
#include <stdio.h>  /* print, perror */
#include <stdlib.h> /* exit status */
#include <string.h> /* strlen */
#include <sys/socket.h>
#include <unistd.h> /* read, sleep */

#define PORT 8080
#define EINAT_IP "10.1.0.209"
#define MY_IP "10.1.0.223"
#define LOOP_BACK "127.0.0.1"
#define MAXLINE 1024

const int FAIL = -1;

void InitServerAddr(struct sockaddr_in *server_addr)
{
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
}

int ReceiveAndSend(int new_socket)
{
    int valread = 0;
    char buffer[MAXLINE] = {0};
    char *msg = "from server - Pong";
    
    while (1)
    {
        valread = recv(new_socket, buffer, sizeof(buffer), 0);
        if (FAIL == valread)
        {
            perror("recv");
            return (EXIT_FAILURE);
        }

        printf("server received: %s\n", buffer);
        valread = send(new_socket, msg, strlen(msg), 0);
        if (FAIL == valread)
        {
            perror("send");
            return (EXIT_FAILURE);
        }
        printf("server sending: %s\n", msg);
        sleep(1);
    }

    return (EXIT_SUCCESS);
}

int main()
{
    int server_fd, new_socket, opt = 1;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (FAIL == server_fd)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (FAIL == setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                           &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    InitServerAddr(&server_addr);

    if (FAIL == bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        perror("bind failed");
        return (EXIT_FAILURE);
    }

    if (FAIL == listen(server_fd, 3))
    {
        perror("listen");
        return (EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&server_addr,
                                                         &addrlen)) < 0)
    {
        perror("accept");
        return (EXIT_FAILURE);
    }

    ReceiveAndSend(new_socket);

    return (EXIT_SUCCESS);
}



    /* convert (the IP) from string to binary form */
/*     if (inet_pton(AF_INET, MY_IP, &server_addr.sin_addr) <= 0)
    {
        printf("\nInvalid server_addr/ server_addr not supported \n");
        perror("inet_pton");
        return (EXIT_FAILURE);
    } */