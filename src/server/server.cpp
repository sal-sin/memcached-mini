/**
 * @file /src/server/server.cpp
 *
 * @brief This file contains the implementation of the `Server` class
 * declared in /src/server/server.hpp
 */

#include <unistd.h>
#include <iostream>
#include "server.hpp"
#include "../utils/message.hpp"
#include "../utils/conn.hpp"
#include "../utils/colors.hpp"

/**
 * @brief an instance of this class represents a single
 * server. Instantiating a `Server` object will start
 * a localhost server that listens on the port passed
 *
 * @param[in] port The port where the server should listen
 */
Server::Server(int port)
{
    listenfd = start_listener(port);
}

/**
 * @brief continuously and sequentially keep accepting connections
 * serving requests
 */
void Server::accept_and_serve_forever()
{
    int connfd;
    while (true)
    {
        // accept
        std::cout << "\n[Server] Waiting for clients" << std::endl;
        connfd = accept_client(listenfd);
        if (connfd >= 0) // valid connection
        {
            process_requests(connfd);
        }
        else if (connfd == -1) // listenfd closed
        {
            break;
        }
    }
}

/**
 * @brief process requests received from the client
 * and respond back until EOF is reached
 *
 * @param[in] connfd the fd to communicate with client
 */
void Server::process_requests(int connfd)
{
    msg_t *resp, *req_msg = make_msg_ref();

    // keep reading until EOF/error
    while (read_msg(connfd, req_msg, -1) != -1)
    {
        display_msg("[Server] Received Request", req_msg);
        switch (req_msg->type)
        {
        case req_put_t:
            kv_store[req_msg->key] = req_msg->value;
            resp = create_ack_msg();
            print_kv_state();
            break;
        case req_get_t:
            resp = kv_store.count(req_msg->key) > 0
                       ? create_hit_msg(kv_store[req_msg->key])
                       : create_miss_msg();
            break;
        default:
            printf("[Server] Invalid message type received, type = %d", req_msg->type);
        }

        // respond back to the client
        send_msg(connfd, resp);
        display_msg("[Server] Sending Response", resp);
        free(resp);
    }
    printf("\n[Server] EOF recieved from connfd\n");
    close(connfd);
}

/**
 * @brief display the current kv store state
 */
void Server::print_kv_state()
{
    std::cout << GREEN << "\n[Server] KV Store state so far:" << RESET << std::endl;
    for (std::pair<std::string, std::string> p : kv_store)
    {
        std::cout << "\t" << YELLOW << p.first << RESET
                  << " -> " << YELLOW << p.second << RESET << std::endl;
    }
}

/**
 * @brief Server cannot accept new clients after this call
 * Connected clients will be served until they disconnect.
 */
void Server::close_server()
{
    close(listenfd);
}