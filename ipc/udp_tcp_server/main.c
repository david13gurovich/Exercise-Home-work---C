#include <iostream> //std::cout
#include <stdlib.h>//exit_status
#include <assert.h> //assert
#include <errno.h> 
#include <sys/socket.h> //socket
#include <string.h> // streror, strncmp, strlen
#include <stdio.h> //stdin, print, perror
#include <unistd.h> //STDIN_FILENO, read

#include <arpa/inet.h>  
#include <pthread.h> 
#include <signal.h> 

#include "logger.hpp"
#include "console.h"
#include "udp_server.h"
#include "tcp_server.h"

#define SIZE 5
#define SIZE_ARR_CLIENTS 1

static const int TO_EXIT = 1;
static const int TO_CONTINUE = 0;

static void SetSocketDescriptorIntoSelect(int *client_socket, fd_set *readfds,
                                        int *max_socket_descriptor)
{
    int socket_descriptor = 0;

    for (int i = 0 ; i < SIZE_ARR_CLIENTS ; i++)   
    {   
        socket_descriptor = client_socket[i];   
        if(socket_descriptor > 0) 
        {
            FD_SET( socket_descriptor , readfds);   
        }  
        
        if( socket_descriptor > *max_socket_descriptor)   
        {
            *max_socket_descriptor = socket_descriptor;   
        }   
    }  
}

int main()
{
    std::ostream& destType(std::cout); 
	ilrd::Logger log(3, &destType);

    int udp_socket = 0;
    if(EXIT_FAILURE == CreateUdpServerAndBind(&udp_socket, &log))
    {
        return (EXIT_FAILURE);
    }
    
    int tcp_socket = 0;
    struct sockaddr_in server_addr;
    if (EXIT_FAILURE == CreateTcpServerAndBind(&log, &tcp_socket, &server_addr))
    {
        return (EXIT_FAILURE);
    }
    
    int status = TO_CONTINUE;
    int client_socket[SIZE_ARR_CLIENTS] = {0}; 
    
    while(TO_CONTINUE == status)
    {  
        int highest_numbered_fd = 0;
        fd_set readfds; 
        FD_ZERO(&readfds);   
        FD_SET(STDIN_FILENO, &readfds);  
        FD_SET(udp_socket, &readfds);  
        FD_SET(tcp_socket, &readfds);  
        
        highest_numbered_fd = (udp_socket > tcp_socket ? udp_socket : tcp_socket);  
        SetSocketDescriptorIntoSelect(client_socket, &readfds ,&highest_numbered_fd);
        
        int select_status = select(highest_numbered_fd + 1 , &readfds , NULL , NULL , NULL);         
        if ((select_status) < 0)   
        {   
            log << ilrd::Logger::ERROR << "select failed\n" << strerror(errno);
            return (EXIT_FAILURE);  
        } 

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            status = Console(&log);
        }
        
        if (FD_ISSET(udp_socket, &readfds))
        {
            SendAndRecieveUDP(udp_socket, &log);
        }      
        
        if (FD_ISSET(tcp_socket, &readfds))
        {
            HandleNewConnection(&server_addr, client_socket, tcp_socket, 
                                                        &readfds, &log);
        }      
        RecieveAndSend(client_socket, &log, &readfds);
    }     

    return (EXIT_SUCCESS);
}