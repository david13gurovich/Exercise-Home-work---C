#include <iostream> //std::cout
#include <assert.h> //assert
#include <pthread.h> //pthread_create, pthread_join 

#include "consol.h"
#include "udp_server.h"
#include "tcp_server.h"
#include "logger.hpp"

int g_EXIT_FROM_PROG = 0;
typedef void* (*ptr_func)(void *);
const int size = 2;

int main()
{
	std::ostream& destType(std::cout); 
	ilrd::Logger log(3, &destType);    
    
    pthread_t threads_servers[2];
    pthread_t thread_consol;

    ptr_func function_to_pass[] = {StartUDPServer, CreateTcpServer};

    if (EXIT_SUCCESS != pthread_create(&thread_consol, NULL, InitConsol, 
                                                         (void *)&log)) 
    {
        log << ilrd::Logger::ERROR << "Error creating thread" << std::endl;
        return (EXIT_FAILURE);
    }

    for(int i = 0; i < size; ++i)
    {
        if (EXIT_SUCCESS != pthread_create(threads_servers + i, NULL, 
                                    function_to_pass[i], (void *)&log)) 
        {
            log << ilrd::Logger::ERROR << "Error creating thread" << std::endl;
            return (EXIT_FAILURE);
        }
    }

    assert(EXIT_SUCCESS == pthread_detach(threads_servers[0]));
    assert(EXIT_SUCCESS == pthread_detach(threads_servers[1]));

    assert (EXIT_SUCCESS == pthread_join(thread_consol, NULL));
        
    return (EXIT_SUCCESS);
}
