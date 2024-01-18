/**
 * @file conn.hpp
 *
 * @brief Provides functions to establish server/client connections
 * over the network. Currently, both server/client assumed to be connected
 * over the local network.
 */

/**
 * Open a socket where the server can listen for connections
 *
 * @param[in] port The port to start server on
 *
 * @return listener descriptor if no error was encountered, else, -1
 */
int start_listener(int port);

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
int accept_client(int listenfd);

/**
 * @brief Establish a connection with server running on
 * localhost, and the port passed
 *
 * @param[in] port The port to connect to
 *
 * @return FD to read and write messages to server OR -1 in
 * case of an error
 */
int connect_server(int port);