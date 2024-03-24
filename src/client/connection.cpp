/**
 * @file /src/client/connection.cpp
 *
 * @brief This file contains the implementation of the Connection class.
 * This can be used by memcached client to esbalish and manage a server
 * connection.
 */

#include <unistd.h>
#include <iostream>
#include <thread>
#include "connection.hpp"
#include "../utils/conn.hpp"
#include "../hash/hash.hpp"

/**
 * @brief Store server metadata and attempt to
 * establish a connection with the server at instantiation.
 * Should not block.
 *
 * @param[in] port The port of the localhost
 * server
 */
Connection::Connection(int p)
{
    hash = get_hash(p);
    port = p;
    connect();
}

/**
 * @brief Attempt to connect to the server
 * and keep track of the client fd.
 */
void Connection::connect()
{
    int cfd = connect_server(port);
    mutex.lock();
    clientfd = cfd;
    mutex.unlock();
}

/**
 * @brief Disconnects and marks
 * this server dead
 */
void Connection::disconnect()
{
    close(clientfd);
    mutex.lock();
    clientfd = -1;
    mutex.unlock();
}

/**
 * @brief Return the hash value of that uniquely
 * identifies the current server and connection.
 *
 * @return Unsigned hash value
 */
unsigned int Connection::get_port_hash()
{
    return hash;
}

/**
 * @brief Indicates if connection is active
 *
 * @return true if server is connected, else false
 */
bool Connection::is_connected()
{
    mutex.lock_shared();
    bool ret = clientfd >= 0;
    mutex.unlock_shared();
    return ret;
}

/**
 * @brief Get the client file descriptor that
 * can be used to communicate with the server
 *
 * @return The file descriptor
 */
int Connection::get_fd()
{
    mutex.lock_shared();
    int cfd = clientfd;
    mutex.unlock_shared();
    return cfd;
}

/**
 * @brief return the port number of
 * localhost server
 *
 * @return the port number
 */
int Connection::get_port()
{
    return port;
}