#ifndef ASSIGNMENT4_ST_REACTOR_HPP
#define ASSIGNMENT4_ST_REACTOR_HPP

#include <poll.h>
#include <vector>
#include <pthread.h>
#include <iostream>
using namespace std;

typedef void (*handler_t)(int);

typedef struct Reactor{
    std::vector<std::pair<int, handler_t>> fd_handlers;
    std::vector<pollfd> poll_fds;
    bool active;
    pthread_t* reactor_thread;
}Reactor ,*pReactor;


void *createReactor();

void stopReactor(void *thisPtr);

void startReactor(void *thisPtr);

void addFd (void * thisPtr,int fd, handler_t handler);

void WaitFor(void * thisPtr);

void* reactorLoop(void* arg);

#endif //ASSIGNMENT4_ST_REACTOR_HPP