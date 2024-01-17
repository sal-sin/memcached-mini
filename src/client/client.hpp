/**
 * @file /src/client/client.hpp
 *
 * @brief This file contains the declaration of the Client API.
 * Use the `Client` object to send requests to the memcached distributed
 * server pool. The implementation is present in /src/client/client.cpp
 */

#include <unistd.h>
#include <iostream>
#include "../utils/message.hpp"

#define PORT 6000

/**
 * @brief represents a single client. This class contains
 * the client APIs available to the application using memcached
 */
class Client
{
private:
    int clientfd;

public:
    /**
     * @brief Client starts by connecting to a localhost server listening
     * at the port passed
     *
     * @param[in] port The port of the server
     */
    Client(int port);

    /**
     * @brief sends a `put` request to the server it is connected to
     *
     * @param[in] key the key (max length = 100 bytes)
     * @param[in] value the value (max length = 1000 bytes)
     * @param[in] response `msg_t` location where the server's
     * reply can be saved
     */
    void send_put_req(std::string key, std::string value, msg_t *response);

    /**
     * @brief sends a `get` request to the server it is connected to
     *
     * @param[in] key the key (max length = 100 bytes)
     * @param[in] response `msg_t` location where the server's
     * reply can be saved
     */
    void send_get_req(std::string key, msg_t *response);
};