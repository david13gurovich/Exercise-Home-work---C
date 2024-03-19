#include <stdio.h> /* print, perror */
#include <sys/socket.h>
#include <stdlib.h> /* exit status */
#include <netinet/in.h>
#include <string.h> /* strlen */
#include <arpa/inet.h> /* inet_pton */
#include <unistd.h> /* read, sleep */

#define PORT 8090
#define MAXLINE 1024
#define EINAT_IP "10.1.0.209"
#define DAN_IP "10.1.0.226"
#define GIL_IP "10.1.0.17"
#define LIOR_IP "10.1.0.188"
#define MY_IP "10.1.0.223"
#define LOOP_BACK "127.0.0.1"
#define BEN "127.0.0.91"

const int FAIL = -1;

int main()
{
    int sock_fd, valread;
    struct sockaddr_in serv_addr;
    const char *client_msg = "from client TCP - Maayan";
    char buffer[MAXLINE] = {0};

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (FAIL == sock_fd)
    {
        perror("Socket creation error ");
        return (EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
/*     serv_addr.sin_addr.s_addr = INADDR_ANY;
 */
    if (inet_pton(AF_INET, LIOR_IP, &serv_addr.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported \n");
        perror("inet_pton");
        return (EXIT_FAILURE);
    } 

    if (FAIL == connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
    {
        perror("Connection Failed ");
        return (EXIT_FAILURE);
    }

    while (1)
    {
        valread = send(sock_fd, client_msg, strlen(client_msg), 0);
        if (FAIL == valread)
        {
           perror("send");
           return (EXIT_FAILURE);
        }
        printf("Client sending: %s \n", client_msg);

       valread = recv(sock_fd, buffer, MAXLINE, 0);
       if (FAIL == valread)
       {
           perror("recv");
           return (EXIT_FAILURE);
       }

        printf("Server sent : %s\n", buffer);
        sleep(1);         
    }

    close(sock_fd);

    return (EXIT_SUCCESS); 
}


