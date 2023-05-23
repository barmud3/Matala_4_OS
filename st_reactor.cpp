#include "st_reactor.hpp"

void *createReactor(){
    pReactor ourReactor = (pReactor)malloc(sizeof(Reactor) * 1);
    ourReactor->active = false;
    ourReactor->fd_handlers.reserve(5);
    ourReactor->poll_fds.reserve(5);
    ourReactor->reactor_thread = (pthread_t*)malloc(sizeof(pthread_t));
    return ourReactor;
}

void addFd(void* thisPtr, int fd, handler_t handler) {

    // Push the pair to the pairs vector for (fd,func)
    ((pReactor)thisPtr)->fd_handlers.push_back({fd, handler});
    // Push only the fd to the Fds vector
    ((pReactor)thisPtr)->poll_fds.push_back({fd, POLLIN, 0 });
    
}

void startReactor(void *thisPtr){
    if (!(((pReactor)thisPtr)->active)) {
            ((pReactor)thisPtr)->active = true;
            // Start the reactor thread within reactorLoop function
            pthread_create(((pReactor)thisPtr)->reactor_thread, nullptr, &reactorLoop, thisPtr);
    }   
}

void stopReactor(void *thisPtr){
    pthread_cancel(*(((pReactor)thisPtr)->reactor_thread));
}

void WaitFor(void * thisPtr){
    if (((pReactor)thisPtr)->active) {
        pthread_join((*((pReactor)thisPtr)->reactor_thread), nullptr);
    }
}

void* reactorLoop(void* arg) {
    
    Reactor* reactor = static_cast<Reactor*>(arg);

    while (reactor->active) {
        
        int num_ready_fds = poll(reactor->poll_fds.data(), reactor->poll_fds.size(), -1);
        if (num_ready_fds < 0) {
            std::cerr << "Error in poll\n";
            break;
        }

        // Running thorugh the fds data strcture to find an "HOT" fd. if  found, execute the function belongs to the fd
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
