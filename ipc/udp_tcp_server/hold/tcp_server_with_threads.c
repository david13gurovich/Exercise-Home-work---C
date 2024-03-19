#include <arpa/inet.h>  //htons
#include <stdio.h>  // print, perror
#include <stdlib.h> // exit status 
#include <string.h> // strlen 
#include <sys/socket.h> //socket
#include <unistd.h> // read, sleep
#include <pthread.h> //pthread create, pthread_join
#include <signal.h> //sigaction
#include <assert.h> //assert

#include "logger.hpp"

#define PORT 8090
#define MY_IP "10.1.0.223"
#define MAXLINE 1024

extern int g_EXIT_FROM_PROG;
const int FAIL = -1;

typedef struct param_threads_s
{
    int server_fd;
    struct sockaddr_in server_addr;
    ilrd::Logger *m_logger;
} param_threads_t;

void SigHand(int sig)
{
	(void)sig;
}

void InitSigAction()
{
	struct sigaction sig_pipe;
    memset (&sig_pipe, 0, sizeof (sig_pipe)); 

    sig_pipe.sa_handler = SigHand;

    assert (FAIL != sigaction(SIGPIPE, &sig_pipe, NULL));
}	

static void InitServerAddr(struct sockaddr_in *server_addr)
{
    memset(server_addr, 0, sizeof(*server_addr));

    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
}

static void RecieveAndSend(int new_socket, char *buffer, ilrd::Logger *logger)
{
    int valread = 0;
    const char *msg = "from server TCP";
    
    while ((0 ==__sync_add_and_fetch((size_t *)&g_EXIT_FROM_PROG, 0)))
    {
        valread = recv(new_socket, buffer, MAXLINE, 0);
        if (FAIL == valread)
        {
            *logger << ilrd::Logger::ERROR << "recv failed" << std::endl;
            return;
        }

        valread = send(new_socket, msg, strlen(msg) + 1, MSG_NOSIGNAL);
        if (FAIL == valread)
        {
            *logger << ilrd::Logger::ERROR << "send failed" << std::endl;
            return;
        } 
    }
}

static void *CreateNewSocketAndThread(void *params)
{
    int new_socket;
    char buffer[MAXLINE];
    memset(buffer, 0, MAXLINE);    

    param_threads_t *server_params = (param_threads_t *)params;
    socklen_t addrlen = sizeof(server_params->server_addr);
    
    new_socket = accept(server_params->server_fd, (struct sockaddr *)&server_params->server_addr,
                                                                                        &addrlen);
    if (FAIL == new_socket)
    {
        *server_params->m_logger << ilrd::Logger::ERROR << "accept failed";
        return (NULL);
    }

    *server_params->m_logger << ilrd::Logger::INFO << "accept client request" << std::endl;
    
    pthread_t thread_id;
    if (EXIT_SUCCESS != pthread_create(&thread_id, NULL, CreateNewSocketAndThread, 
                                                            (void *)server_params)) 
    {
        *server_params->m_logger << ilrd::Logger::ERROR << "Error creating thread";
        return (NULL);
    }

    *server_params->m_logger << ilrd::Logger::INFO << "thread create" << std::endl;
    RecieveAndSend(new_socket, buffer, server_params->m_logger); 
    assert (EXIT_SUCCESS == pthread_join(thread_id, NULL));
    
    return (NULL);
}

void InitParams(param_threads_t *param_to_pass, struct sockaddr_in *server_addr,
                                                int server_fd, ilrd::Logger *log)
{
    param_to_pass->server_fd = server_fd;
    param_to_pass->server_addr = *server_addr;
    param_to_pass->m_logger = log;
}

void* CreateTcpServer(void *param)
{
    param_threads_t param_to_pass;
    int server_fd, opt = 1;
    struct sockaddr_in server_addr;    

	ilrd::Logger *log = (ilrd::Logger *)param;
    *log << ilrd::Logger::INFO << "open TCP server" << std::endl;

    InitSigAction();

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (FAIL == server_fd)
    {
        *log << ilrd::Logger::ERROR << "socket failed";
        return (NULL);
    }

    if (FAIL == setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                         &opt, sizeof(opt)))
    {
        *log << ilrd::Logger::ERROR << "setsockopt";
        return (NULL);
    }
    
    InitServerAddr(&server_addr);

    if (FAIL == bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        *log << ilrd::Logger::ERROR << "bind failed";
        return (NULL);
    }

    if (FAIL == listen(server_fd, 3))
    {
        *log << ilrd::Logger::ERROR << "listen failed";
        return (NULL);
    }

    InitParams(&param_to_pass, &server_addr, server_fd, log);
    CreateNewSocketAndThread(&param_to_pass);

    return (NULL);
}

