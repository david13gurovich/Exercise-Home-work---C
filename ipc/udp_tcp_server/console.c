#include <stdio.h> //stdin, fgets,fprintf
#include <string.h> //stcmp
#include <stdlib.h> // exit

#include "logger.hpp"

#define SIZE 5

static const int TO_EXIT = 1;
static const int TO_CONTINUE = 0;

int Console(ilrd::Logger *log)
{
    char toGet[SIZE] = {0};
    fgets(toGet, SIZE, stdin);
    if (NULL == toGet)
    {
        *log << ilrd::Logger::ERROR << "Error fgets" << std::endl;
        return (EXIT_FAILURE);
    }
    
    if (0 != strncmp(toGet, "exit", SIZE))
    {
        return (TO_EXIT);    
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

    return (TO_CONTINUE);
}