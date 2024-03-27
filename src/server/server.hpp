/**
 * @file /src/server/server.hpp
 *
 * @brief This file contains the declaration of the `Server` class.
 * An instance of this class represents a single server.
 * Instantiating a `Server` object will start a localhost server that
 * listens for client requests on the port passed and responds to them.
 * The key-value store state of the server is present as an instance
 * variable of this class.
 */

#ifndef SERVER_H
#define SERVER_H

#include <shared_mutex>
#include "../utils/logger.hpp"

/**
 * @brief Represents a single server
 */
class Server
{
public:
    /**
     * The constructor should start a new server
     * @param[in] port the localhost port where server should be started
     * @param[in] print_logs Indicates if logs should be printed
     * to console
     */
    Server(int port, bool print_logs);

    /**
     * @brief continuously and sequentially keep accepting connections
     * serving requests
     */
    void accept_and_serve_forever();

    /**
     * @brief Server cannot accept new clients after this call
     * Connected clients will be served until they disconnect.
     */
    void close_server();

private:
    int listenfd;
    std::shared_mutex kv_store_mutex;
    std::unordered_map<std::string, std::string> kv_store;
    Logger *logger;

    /**
     * @brief process requests received from the client
     * and respond back until EOF is reached
     * @param[in] connfd the fd to communicate with client
     */
    void process_requests(int connfd);

    /**
     * @brief display the current kv store state
     */
    void print_kv_state();
};
#endif