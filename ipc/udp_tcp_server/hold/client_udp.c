#include <netinet/in.h> /* sockaddr , socklen_t*/
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit, EXIT_FAILURE */  
#include <string.h> /* memset */
#include <unistd.h>  /* close */
#include <arpa/inet.h> /* inet_pron*/

#define PORT 8070 
#define MAXLINE 1024 
#define BROADCAST "10.1.255.255"
#define MY_IP "10.1.0.188"
#define MAAYAN "10.1.0.223"
#define GIL "10.1.0.17"
  
int main()
{ 
    int sockfd = 0; 
    char buffer[MAXLINE]= {0}; 
    char *client_message = "Client :lior - Ping"; 
    struct sockaddr_in servaddr; 
    int n = 0;
    int broadcastPermission = 1;
    socklen_t len = {0}; 

    while(1)
    {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        { 
            perror("socket creation failed"); 
            exit(EXIT_FAILURE); 
        } 

        memset(&servaddr, 0, sizeof(servaddr)); 
        servaddr.sin_family = AF_INET; 
        servaddr.sin_port = htons(PORT); 
    /*     servaddr.sin_addr.s_addr = INADDR_ANY; */

        if (inet_pton(AF_INET, MAAYAN, &servaddr.sin_addr) <= 0)
        {
            printf("\nInvalid address/ Address not supported \n");
            perror("inet_pton");
            return(EXIT_FAILURE);
        }

        /* set socket options enable broadcast*/
        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission,sizeof(broadcastPermission)) < 0)
        {
        fprintf(stderr, "setsockopt error");
        exit(1);
        }   

        if (-1 == sendto(sockfd, client_message, strlen(client_message), 
                    MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
        {
            perror("sent failed"); 

            exit(EXIT_FAILURE); 
        }           

        printf("after send \n");        
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,   
                                    (struct sockaddr *)&servaddr,  &len);
        if (-1 == n)
        {
            perror("read creation failed"); 

            exit(EXIT_FAILURE); 
        }
        buffer[n] = '\0'; 
        printf("Client : %s\n", buffer); 
        sleep(1);  
        
        close(sockfd); 
    }

    return 0; 
} 
