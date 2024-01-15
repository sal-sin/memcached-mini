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

#define PORT 6000

using namespace std;

/**
 * @brief
 */
class Server
{
public:
    /**
     * The constructor should start a new server
     */
    Server(int port);

private:
    int listenfd;
    unordered_map<string, string> kv_store;

    /**
     * @brief continuously and sequentially keep accepting connections
     * serving requests
     */
    void accept_and_serve_forever();

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