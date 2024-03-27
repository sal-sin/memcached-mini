/**
 * @file /src/client/client.hpp
 *
 * @brief This file contains the declaration of the Client API.
 * Use the `Client` object to send requests to the memcached distributed
 * server pool. The implementation is present in /src/client/client.cpp
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <iostream>
#include <shared_mutex>
#include "../utils/message.hpp"
#include "../utils/logger.hpp"
#include "connection.hpp"

/**
 * @brief represents a single client. This class contains
 * the client APIs available to the application using memcached
 */
class Client
{
public:
    /**
     * @brief Client starts by connecting to a localhost servers listening
     * at the ports passed
     *
     * @param[in] ports Ports of all servers in the server pool
     * @param[in] print_logs Indicates if log should be printed
     */
    Client(std::vector<int> ports, bool print_logs);

    /**
     * @brief sends a `put` request to the server it is connected to
     *
     * @param[in] key the key (max length = 100 bytes)
     * @param[in] value the value (max length = 1000 bytes)
     * @param[in] response `msg_t` location where the server's
     * reply can be saved
     *
     * @return true if request was successful, else false
     */
    bool send_put_req(std::string key, std::string value, msg_t *response);

    /**
     * @brief sends a `get` request to the server it is connected to
     *
     * @param[in] key the key (max length = 100 bytes)
     * @param[in] response `msg_t` location where the server's
     * reply can be saved
     *
     * @return The value as a non-empty string, return "" if no
     * value was received
     */
    std::string send_get_req(std::string key, msg_t *response);

    /**
     * @brief terminates connection with all servers
     */
    void close_client();

protected:
    std::vector<Connection *> server_pool;

    /**
     * @brief Store a server instance to the state
     * of the client
     * @param[in] server_p pointer to a `Server` instance
     */
    void add_server_to_pool(Connection *server_p);

    /**
     * @brief Selects a server for a given key
     * @param[in] key The key to store
     * @return The server instance
     */
    Connection *select_successor_server(std::string key);

private:
    std::shared_mutex close_mutex;
    bool close_flag;
    Logger *logger;

    /**
     * @brief Try connecting to servers that have been
     * disconnected. This function is expected to run
     * in the background throughout while the client is
     * active
     */
    void poll_disconnected_servers();
};

#endif