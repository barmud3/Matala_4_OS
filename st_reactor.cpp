#include "st_reactor.hpp"
#include <iostream>
#include <vector>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <functional>
#include <algorithm>


void *createReactor(){
    pReactor ourReactor = (pReactor)malloc(sizeof(Reactor) * 1);
    ourReactor->active = false;
    ourReactor->fd_handlers.reserve(5);
    ourReactor->poll_fds.reserve(5);
    ourReactor->reactor_thread = (pthread_t*)malloc(sizeof(pthread_t));
    return ourReactor;
}

void stopReactor(void *thisPtr){
    pthread_cancel(*(((pReactor)thisPtr)->reactor_thread));
}

void* reactorLoop(void* arg) {
    
    Reactor* reactor = static_cast<Reactor*>(arg);
    while (reactor->active) {
        
        int num_ready_fds = poll(reactor->poll_fds.data(), reactor->poll_fds.size(), -1);
        if (num_ready_fds < 0) {
            std::cerr << "Error in poll\n";
            break;
        }
        for (const auto& poll_fd : reactor->poll_fds) {
            if (poll_fd.revents & POLLIN) {
                int fd = poll_fd.fd;
                auto it = std::find_if(reactor->fd_handlers.begin(), reactor->fd_handlers.end(),
                [fd](const std::pair<int, handler_t>& fd_handler) { return fd_handler.first == fd; });
            if (it != reactor->fd_handlers.end()) {
                it->second(fd);
            }

            }
        }
    }

    pthread_exit(nullptr);
}

void startReactor(void *thisPtr){
    if (!(((pReactor)thisPtr)->active)) {
            ((pReactor)thisPtr)->active = true;
            pthread_create(((pReactor)thisPtr)->reactor_thread, nullptr, &reactorLoop, thisPtr);
        
        ///////////////////
    }
    
    
}

void addFd(void* thisPtr, int fd, handler_t handler) {
    ((pReactor)thisPtr)->fd_handlers.push_back({fd, handler});
    ((pReactor)thisPtr)->poll_fds.push_back({fd, POLLIN, 0 });
    
}


void WaitFor(void * thisPtr){
    if (((pReactor)thisPtr)->active) 
    {
        pthread_join((*((pReactor)thisPtr)->reactor_thread), nullptr);
    }
}