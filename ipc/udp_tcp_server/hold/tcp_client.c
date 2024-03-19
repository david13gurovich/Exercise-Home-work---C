#include <arpa/inet.h>
#include <netdb.h> 
#include <strings.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#define MAX 30 
#define PORT 8070 
#define SA struct sockaddr 

void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 

    bzero(buff, sizeof(buff)); 
    strcpy(buff, "TCP client");
    write(sockfd, buff, sizeof(buff)); 
    bzero(buff, sizeof(buff)); 
    read(sockfd, buff, sizeof(buff)); 
    printf("From Server : %s", buff); 
} 
  
int main() 
{ 
    int sockfd; 
    struct sockaddr_in servaddr; 

/*     while(1)
 */    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0); 
        if (sockfd == -1) { 
            printf("socket creation failed...\n"); 
            exit(0); 
        } 
        else
            printf("Socket successfully created..\n"); 
        bzero(&servaddr, sizeof(servaddr)); 
    
        servaddr.sin_family = AF_INET; 
        servaddr.sin_port = htons(PORT); 
        if(inet_pton(AF_INET, "10.1.0.223", &servaddr.sin_addr)<=0) 
        { 
            printf("\nInvalid address/ Address not supported \n"); 
            return -1; 
        } 

        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
            printf("connection with the server failed...\n"); 
            exit(0); 
        } 
        else
            printf("connected to the server..\n"); 

        printf("before function SEND!\n");
    
        func(sockfd); 
    
        close(sockfd);
    }

    return (1); 
} 