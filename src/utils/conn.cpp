/**
 * @file conn.cpp
 *
 * @brief Implements functions to establish connections between servers and clients
 * over the network. Currently, server & client connections over the local network
 * are implemented.
 *
 * @cite referred code examples available on student website of `Computer Systems: A Programmer's
 * Perspective, 3/E (CS:APP3e)` (https://csapp.cs.cmu.edu/3e/ics3/code/src/csapp.c)
 * @cite `sockaddr_in` for localhost addresses adopted from https://www.geeksforgeeks.org/socket-programming-cc/
 */

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "conn.hpp"

#define LOCALHOST "127.0.0.1"
#define CONN_BUFFER 2048

/**
 * Open a socket where the server can listen for connections
 *
 * @param[in] port The port to start server on
 *
 * @return listener descriptor if no error was encountered, else, -1
 */
int start_listener(int port)
{
    int listenfd, opt = 1; // server listener descriptor, option for setsockopt

    // address to bind to, AF_INET for IPv4
    struct sockaddr_in addr = {.sin_family = AF_INET,
                               .sin_addr.s_addr = inet_addr(LOCALHOST),
                               .sin_port = htons(port)};

    // The socket where server should listen for new connection requests
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[server] couldn't create listenfd");
        return -1;
    }

    // Options to avoid errors during bind
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    // Associate `addr` to the socket `listenfd`
    if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[server] couldn't bind socket to address");
        return -1;
    }

    // Turn this into socket that listens for connection
    // requests from clients
    if (listen(listenfd, CONN_BUFFER) < 0)
    {
        close(listenfd);
        perror("[server] couldn't start listener");
        return -1;
    }
    return listenfd;
}

/**
 * @brief Listens for connection requests, and accepts
 * client requests to establish a connection with the client
 *
 * @param listenfd The server listener file descriptor
 *
 * @return If successful, the fd where server can read client requests
 * and respond to the client. On closed FD error, return -1. On any other
 * error return -2.
 */
int accept_client(int listenfd)
{
    // not interested in connected peer info as of now
    int connfd = accept(listenfd, NULL, NULL);

    if (connfd < 0)
    {
        if (errno == EBADF)
        {
            perror("socket is not open to accept clients");
            return -1;
        }
        else
        {
            perror("error while attempting accept client");
            return -2;
        }
    }
    return connfd;
}

/**
 * @brief Establish a connection with server running on
 * localhost, and port `PORT`
 * @return FD to read and write messages to server OR -1 in
 * case of an error
 */
int connect_server(int port)
{
    int clientfd;
    // AF_INET for IPv4
    struct sockaddr_in addr = {.sin_family = AF_INET,
                               .sin_addr.s_addr = inet_addr(LOCALHOST),
                               .sin_port = htons(port)};

    // Open a socket to communicate with server
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[client] Couldn't create socket");
        return -1;
    }

    // establish connection
    if (connect(clientfd, (struct sockaddr *)&addr, sizeof(sockaddr)) < 0)
    {
        close(clientfd);
        perror("[client] Couldn't connect to the server");
        return -1;
    }
    return clientfd;
}