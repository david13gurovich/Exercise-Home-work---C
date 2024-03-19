#include <arpa/inet.h>  //htons
#include <stdio.h>  // print, perror
#include <stdlib.h> // exit status 
#include <string.h> // strlen 
#include <sys/socket.h> //socket
#include <unistd.h> // read, sleep
#include <pthread.h> //pthread create, pthread_join
#include <signal.h> //sigaction
#include <assert.h> //assert
#include <errno.h> 

#include "logger.hpp"

#define PORT 8070
#define MY_IP "127.0.0.1"
#define MAXLINE 1024
#define SIZE_ARR_CLIENTS 1

const int FAIL = -1;

const int g_SIZE_CLIENTS_FD_ARR = 30;

int RecieveAndSend(int *client_sockets, ilrd::Logger *log, fd_set *readfds)
{
    char buffer_to_read[MAXLINE];
    const char *msg = "from server TCP";

    for(int i = 0; i < SIZE_ARR_CLIENTS; ++i)
    {
        int socket_descriptor = client_sockets[i];
        if (FD_ISSET(socket_descriptor ,readfds))   
        {
            int valread = recv(socket_descriptor, buffer_to_read, MAXLINE, 0);
            if (FAIL == valread)
            {
                close(socket_descriptor);   
                client_sockets[i] = 0;   
                *log << ilrd::Logger::ERROR << "recv failed" << std::endl;
                return (EXIT_FAILURE);
            }
            
            if (0 == valread)
            {
                close(socket_descriptor);   
                client_sockets[i] = 0; 
                return (EXIT_FAILURE);
            }

            buffer_to_read[valread] = '\0';
            printf("%s\n", buffer_to_read);
            
            valread = send(socket_descriptor, msg, strlen(msg) + 1, MSG_NOSIGNAL);
            if (FAIL == valread)
            {
                perror("");
                *log << ilrd::Logger::ERROR << "send failed" << std::endl;
                return (EXIT_FAILURE);
            }
        }
    }
    return (EXIT_SUCCESS);
}

static void AddNewSocketToClientsFdArr(int *client_socket, int new_socket)
{
    for (int i = 0; i < SIZE_ARR_CLIENTS; ++i)   
    {   
        if(0 == client_socket[i])   
        {  
            client_socket[i] = new_socket;                            
            break;   
        }   
    }
}

int HandleNewConnection(struct sockaddr_in *server_addr,
                        int *client_socket, int master_socket,
                        fd_set *readfds, ilrd::Logger *log)
{
    int new_socket;
    socklen_t addrlen = {0}; 
    
    new_socket = accept(master_socket, (struct sockaddr *)server_addr,
                                                                    &addrlen);
    if (FAIL == new_socket)
    {
        *log << ilrd::Logger::ERROR << "accept failed" << strerror(errno);
        return EXIT_FAILURE;
    }

    *log << ilrd::Logger::INFO << "accept client request\n";
        
    AddNewSocketToClientsFdArr(client_socket, new_socket);

    return (EXIT_SUCCESS);   
}

static void InitTCPServerAddr(struct sockaddr_in *server_addr)
{
    memset(server_addr, 0, sizeof(*server_addr));

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
}

static int CreateMasterSocket(ilrd::Logger *log)
{
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (FAIL == master_socket)
    {
        *log << ilrd::Logger::ERROR << "socket failed";
        return (FAIL);
    }

    int opt = 1;
    if (FAIL == setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                            &opt, sizeof(opt)))
    {
        *log << ilrd::Logger::ERROR << "setsockopt";
        return (FAIL);
    }

    return (master_socket);
}

int CreateTcpServerAndBind(ilrd::Logger *log, int *tcp_socket, struct sockaddr_in *server_addr)
{  
    printf("CreateTCP\n"); 
    *tcp_socket = CreateMasterSocket(log);
    if (FAIL == *tcp_socket)
    {
        return (EXIT_FAILURE);
    }

    InitTCPServerAddr(server_addr);
   
    if (FAIL == bind(*tcp_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)))
    {
        *log << ilrd::Logger::ERROR << "bind failed";
        return (EXIT_FAILURE);
    }

    if (FAIL == listen(*tcp_socket, 3))
    {
        *log << ilrd::Logger::ERROR << "listen failed";
        return (EXIT_FAILURE);
    }
          
    return (EXIT_SUCCESS); 
} 