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
#include "../sys/conn.hpp"

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
        if ((connfd = accept_client(listenfd)) != -1)
        {
            process_requests(connfd);
        }
        close(connfd);
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
    // keep reading until EOF
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
}

/**
 * @brief display the current kv store state
 */
void Server::print_kv_state()
{
    std::cout << "\n[Server] KV Store state so far:" << std::endl;
    for (std::pair<std::string, std::string> p : kv_store)
    {
        std::cout << "\t" << p.first << " -> " << p.second << std::endl;
    }
}