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

/* Represents a server that the client is
 * connected to */
class Server
{
public:
    int hash;
    int port;
    int clientfd;
    Server(int hash, int port, int clientfd);
};

/**
 * @brief represents a single client. This class contains
 * the client APIs available to the application using memcached
 */
class Client
{
private:
    std::vector<Server *> server_pool;

    /**
     * @brief Store a server instance to the state
     * of the client
     * @param[in] server_p pointer to a `Server` instance
     */
    void add_server_to_pool(Server *server_p);

    /**
     * @brief Selects a server for a given key
     * @param[in] key The key to store
     * @return The server instance
     */
    Server *select_successor_server(std::string key);

public:
    /**
     * @brief Client starts by connecting to a localhost servers listening
     * at the ports passed
     *
     * @param[in] ports Ports of all servers in the server pool
     */
    Client(std::vector<int> ports);

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