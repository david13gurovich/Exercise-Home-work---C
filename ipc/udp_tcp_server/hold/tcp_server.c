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

#define PORT 8090
#define MY_IP "127.0.0.1"
#define MAXLINE 1024
#define SIZE_ARR_CLIENTS 30

const int FAIL = -1;

static int CreateMasterSocket(ilrd::Logger *log);
static void InitServerAddr(struct sockaddr_in *server_addr);
static void AddNewSocketToClientsFdArr(int *client_socket, int new_socket);
static void CommunicateWithMultipleClients(struct sockaddr_in server_addr,
                                    int master_socket, ilrd::Logger *log);
static void SetSocketDescriptor(int *client_socket, fd_set *readfds ,
                                        int *max_socket_descriptor);
static void HandleNewConnection(struct sockaddr_in *server_addr, 
                        int *client_socket, int master_socket, 
                        fd_set *readfds, ilrd::Logger *log);
static void RecieveAndSend(int *client_sockets, ilrd::Logger *log, 
                                                fd_set *readfds);

const int g_SIZE_CLIENTS_FD_ARR = 30;

void *CreateTcpServer(void *param)
{
    ilrd::Logger *log = (ilrd::Logger *)param;
    *log << ilrd::Logger::INFO << "open TCP server" << std::endl;
    
    int master_socket = CreateMasterSocket(log);
    if (FAIL == master_socket)
    {
        return (NULL);
    }

    struct sockaddr_in server_addr;    
    InitServerAddr(&server_addr);
   
    if (FAIL == bind(master_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        *log << ilrd::Logger::ERROR << "bind failed";
        return (NULL);
    }

    if (FAIL == listen(master_socket, 3))
    {
        *log << ilrd::Logger::ERROR << "listen failed";
        return (NULL);
    }
          
    CommunicateWithMultipleClients(server_addr, master_socket, log); 

    return (NULL); 
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

static void InitServerAddr(struct sockaddr_in *server_addr)
{
    memset(server_addr, 0, sizeof(*server_addr));

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
}

static void CommunicateWithMultipleClients(struct sockaddr_in server_addr,
                                    int master_socket, ilrd::Logger *log)
{
    int highest_numbered_fd = 0;
    int client_socket[g_SIZE_CLIENTS_FD_ARR] = {0}; 
    fd_set readfds;      

    while(1)   
    {   
        FD_ZERO(&readfds);   
        FD_SET(master_socket, &readfds);   
        highest_numbered_fd = master_socket;  
        SetSocketDescriptor(client_socket, &readfds ,&highest_numbered_fd);
   
        int status = select(highest_numbered_fd + 1 , &readfds , NULL , NULL , NULL);         
        if ((status) < 0)   
        {   
            *log << ilrd::Logger::ERROR << "select failes" << strerror(errno);
            return ;  
        }   
        
        HandleNewConnection(&server_addr,client_socket , master_socket, &readfds, log);
        RecieveAndSend(client_socket, log, &readfds);
    }
}

static void SetSocketDescriptor(int *client_socket, fd_set *readfds ,
                                        int *max_socket_descriptor)
{
    int socket_descriptor = 0;

    for (int i = 0 ; i < g_SIZE_CLIENTS_FD_ARR ; i++)   
    {   
        socket_descriptor = client_socket[i];   
        if( socket_descriptor > 0) 
        {
            FD_SET( socket_descriptor , readfds);   
        }  
        
        if( socket_descriptor > *max_socket_descriptor)   
        {
            *max_socket_descriptor = socket_descriptor;   
        }   
    }  
}

void HandleNewConnection(struct sockaddr_in *server_addr,
                        int *client_socket, int master_socket,
                        fd_set *readfds, ilrd::Logger *log)
{
    int new_socket;
    socklen_t addrlen = {0}; 

    if (FD_ISSET(master_socket, readfds))   
    {   
        new_socket = accept(master_socket, (struct sockaddr *)server_addr,
                                                                    &addrlen);
        if (FAIL == new_socket)
        {
            *log << ilrd::Logger::ERROR << "accept failed" << strerror(errno);
            return;
        }

        *log << ilrd::Logger::INFO << "accept client request\n";
         
        AddNewSocketToClientsFdArr(client_socket, new_socket);
    }   
}

static void RecieveAndSend(int *client_sockets, ilrd::Logger *log, fd_set *readfds)
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
                return;
            }

            buffer_to_read[valread] = '\0';
            printf("%s\n", buffer_to_read);
            
            valread = send(socket_descriptor, msg, strlen(msg) + 1, MSG_NOSIGNAL);
            if (FAIL == valread)
            {
                perror("");
                *log << ilrd::Logger::ERROR << "send failed" << std::endl;
                return;
            }
        }
    }
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





