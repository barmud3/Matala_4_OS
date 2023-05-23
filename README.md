# Reactor Chat

This project is an implementation of a chat application using the Reactor design pattern. It supports an unlimited number of customers and utilizes the Reactor structure along with the `poll` or `select` mechanism.

## Prerequisites

- C++17 compiler (such as clang-14)
- pthread library

## Getting Started

1. Clone the repository or download the source code files.

2. Build the project by running the following command:

   ```
   make
   ```

   This will compile the `react_server` executable and the `libst_reactor.so` library.

3. Run the server by executing the following command:

   ```
   ./react_server
   ```
   
4. Specifiy the path to the library :
   ```
   export LD_LIBRARY_PATH="."
   ```

   This will start the Reactor and listen for incoming connections.

## Reactor Library

The core of the design pattern is implemented in the `libst_reactor.so` library, which provides the necessary functionality for the Reactor structure.
the Reactor will be in charge of handling clients and listener , once one of them become active he will use the corresponding handle function.

 - The Reactor uses the `poll` mechanism , in order to maintain tracking which of the clients/listen is active.

### API Functions

The Reactor library provides the following API functions:

- `void* createReactor()`: Creates a new Reactor instance and returns a pointer to it.

- `void stopReactor(void* thisPtr)`: Stops the Reactor if it is active.

- `void startReactor(void* thisPtr)`: Starts the Reactor, which begins listening for incoming events on registered file descriptors.

- `void addFd(void* thisPtr, int fd, handler_t handler)`: Registers a file descriptor `fd` with the Reactor and associates it with the provided handler function `handler_t`.

- `void WaitFor(void* thisPtr)`: Waits for the Reactor thread to finish execution.

### Usage

To use the Reactor library, follow these steps:

1. Create a Reactor instance by calling `createReactor()`, which returns a pointer to the Reactor structure.

2. Register file descriptors and their corresponding handler functions using `addFd(thisPtr, fd, handler)`. The Reactor will listen for events on these file descriptors.

3. Start the Reactor by calling `startReactor(thisPtr)`. The Reactor will enter a loop and begin handling events on the registered file descriptors.

4. To stop the Reactor, call `stopReactor(thisPtr)`. This will halt the Reactor's execution.

5. Finally, use `WaitFor(thisPtr)` to wait for the Reactor thread to finish before exiting the program.

## react_server.cpp

The `react_server.cpp` file contains the main function for the chat server. It sets up a listening socket, creates a Reactor instance, and registers the listening socket with the Reactor. It then starts the Reactor and waits for it to finish execution.

## makefile

The `makefile` contains the build instructions for the project. It compiles the `react_server.cpp` file along with the `libst_reactor.so` library and generates the `react_server` executable.

## Additional Notes

- The Reactor only supports reading events and does not handle writing or exception events.

- The Reactor structure and related functions are defined in the `st_reactor.hpp` and `st_reactor.cpp` files.

- The chat functionality and message handling are implemented in the `handle_listen` and `handle_clients` functions in `react_server.cpp`.

Feel free to explore the code and modify it according to your needs. Enjoy using the Reactor Chat application!
