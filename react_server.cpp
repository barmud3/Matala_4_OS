#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <vector>
#include "st_reactor.hpp"
#include <dlfcn.h>
#include <iostream>
using namespace std;

#define PORT "9034"   // Port we're listening on
void handle_listen(int listener);
void handle_clients(int fd);


static pReactor ourReactor = static_cast<pReactor>(createReactor());

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

// Main
int main(void)
{
    int listener;     // Listening socket descriptor

    // Set up and get a listening socket
    listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }
    
    startReactor(ourReactor);
    addFd(ourReactor,listener,handle_listen); //adding listener fd.
    WaitFor(ourReactor);
    
    
return 0;
}



void handle_listen(int listener){

    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    int newfd; 
    char remoteIP[INET6_ADDRSTRLEN];     

    addrlen = sizeof(remoteaddr);
    newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
    if (newfd == -1) {
    perror("accept");
    } else {
        addFd(ourReactor,newfd,handle_clients);
    printf("New connection from %s on ""socket %d\n",
    inet_ntop(remoteaddr.ss_family,
    get_in_addr((struct sockaddr *)&remoteaddr),remoteIP, INET6_ADDRSTRLEN),newfd);
    }
}

void handle_clients(int fd)
{
    char buf[256];
    int nbytes = recv(fd, buf, sizeof(buf), 0);
                    
    int sender_fd = fd;

    if (nbytes <= 0) {
        // Got error or connection closed by client
        if (nbytes == 0) {
            // Connection closed
            printf("pollserver: socket %d hung up\n", sender_fd);
        } else {
            perror("recv");
        }

    } else {
        // We got some good data from a client
        for (size_t j = 0; j < ourReactor->fd_handlers.size(); j++) {
            // Send to everyone!
            int dest_fd = ourReactor->fd_handlers[j].first;

            // Except the listener and ourselves
            int listener = ourReactor->fd_handlers[0].first;
            if (dest_fd != listener && dest_fd != sender_fd) {
                if (send(dest_fd, buf, nbytes, 0) == -1) {
                    perror("send");
                }
            }
        }
    }
}