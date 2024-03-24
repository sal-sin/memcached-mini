/**
 * @file /src/client/connection.hpp
 *
 * @brief This file contains the declaration of the Connection class.
 * This can be used by memcached client to esbalish and manage a server
 * connection. The implementation is present in /src/client/connection.cpp
 */

#include <shared_mutex>

/* A class with metadata about a connection
between a client and a server, with methods
to manage the connection */
class Connection
{
public:
    /**
     * @brief Store server metadata and attempt to
     * establish a connection with the server at instantiation.
     * Should not block.
     *
     * @param[in] port The port of the localhost
     * server
     */
    Connection(int port);

    /**
     * @brief Attempt to connect to the server
     * and keep track of the client fd.
     */
    void connect();

    /**
     * @brief Disconnects and marks
     * this server dead
     */
    void disconnect();

    /**
     * @brief Return the hash value of that uniquely
     * identifies the current server and connection.
     *
     * @return Unsigned hash value
     */
    unsigned int get_port_hash();

    /**
     * @brief Indicates if connection is active
     *
     * @return true if server is connected, else false
     */
    bool is_connected();

    /**
     * @brief Get the client file descriptor that
     * can be used to communicate with the server
     *
     * @return The file descriptor
     */
    int get_fd();

    /**
     * @brief return the port number of
     * localhost server
     *
     * @return the port number
     */
    int get_port();

private:
    unsigned int hash;
    int port;
    int clientfd;

    // to manage exclusive access to clientfd
    std::shared_mutex mutex;
};