/**
 * @file /src/server/server.cpp
 *
 * @brief This file contains the implementation of the `Server` class
 * declared in /src/server/server.hpp
 */

#include <unistd.h>
#include <iostream>
#include <thread>
#include "server.hpp"
#include "../utils/message.hpp"
#include "../utils/conn.hpp"
#include "../utils/logger.hpp"
#include "../utils/colors.hpp"

/**
 * @brief an instance of this class represents a single
 * server. Instantiating a `Server` object will start
 * a localhost server that listens on the port passed
 *
 * @param[in] port The port where the server should listen
 * @param[in] print_logs Indicates if logs should be printed
 * to console
 */
Server::Server(int port, bool print_logs)
{
    logger = new Logger(print_logs);
    listenfd = start_listener(port);
    std::thread serve_thread(&Server::accept_and_serve_forever, this);
    serve_thread.detach();
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
        *logger << "\n[Server] Waiting for clients\n";
        connfd = accept_client(listenfd);
        if (connfd >= 0) // valid connection
        {
            *logger << GREEN << "\n[Server] Client connected\n"
                    << RESET;
            std::thread cl_thread(&Server::process_requests, this, connfd);
            cl_thread.detach();
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
        logger->display_msg("[Server] Received Request", req_msg);
        switch (req_msg->type)
        {
        case req_put_t:
            kv_store_mutex.lock(); // write
            kv_store[req_msg->key] = req_msg->value;
            kv_store_mutex.unlock();
            resp = create_ack_msg();
            print_kv_state();
            break;
        case req_get_t:
            kv_store_mutex.lock_shared(); // read
            resp = kv_store.count(req_msg->key) > 0
                       ? create_hit_msg(kv_store[req_msg->key])
                       : create_miss_msg();
            kv_store_mutex.unlock_shared();
            break;
        default:
            printf("[Server] Invalid message type received, type = %d", req_msg->type);
        }

        // respond back to the client
        send_msg(connfd, resp);
        logger->display_msg("[Server] Sending Response", resp);
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
    kv_store_mutex.lock_shared(); // read
    *logger << GREEN << "\n[Server] KV Store state so far:\n"
            << RESET;
    for (std::pair<std::string, std::string> p : kv_store)
    {
        *logger << "\t" << YELLOW << p.first << RESET
                << " -> " << YELLOW << p.second << RESET << "\n";
    }
    kv_store_mutex.unlock_shared();
}

/**
 * @brief Server cannot accept new clients after this call
 * Connected clients will be served until they disconnect.
 */
void Server::close_server()
{
    close(listenfd);
}