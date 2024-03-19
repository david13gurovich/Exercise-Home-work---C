#include <stdio.h> //stdin, fgets,fprintf
#include <string.h> //stcmp
#include <stdlib.h> // exit

#include "logger.hpp"
#include "consol.h"

#define SIZE 5

extern int g_EXIT_FROM_PROG;

void* InitConsol(void *param)
{
	ilrd::Logger *log = (ilrd::Logger *)param;
    char toGet[SIZE] = {0};

    while (0 != strncmp(toGet, "exit", SIZE))
    {
        fgets(toGet, SIZE, stdin);
        if (NULL == toGet)
        {
            *log << ilrd::Logger::ERROR << "Error fgets" << std::endl;
            return (NULL);
        }

        if(0 == strncmp(toGet, "ping", SIZE))
        {
            printf("pong\n");
        }
        else if ('+' == toGet[0])
        {
            log->IncSeverity();
        }
        else if ('-' == toGet[0])
        {
            log->DecSeverity();
        }
    }
    __sync_add_and_fetch((size_t *)&g_EXIT_FROM_PROG, 1);
    
    return (NULL);
}