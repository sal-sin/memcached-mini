/**
 * @file /src/client/client.cpp
 *
 * @brief This file contains the implementation of the Client API methods
 * declared in /src/client/client.hpp
 */

#include <unistd.h>
#include <iostream>
#include "client.hpp"
#include "../sys/conn.hpp"

/**
 * @brief Placeholder for a hash function until
 * openssl setup works
 *
 * @param[in] num An integer to get the hash of
 *
 * @return The computer hash value
 */
int hash_value(int num)
{
    srand(num);
    return rand() % INT_MAX;
}

/**
 * @brief Implementation of Server constructor
 * @param[in] h The hash value
 * @param[in] p The port
 * @param[in] c The clientfd\
 */
Server::Server(int h, int p, int c)
{
    hash = h;
    port = p;
    clientfd = c;
}

/**
 * @brief Client starts by connecting to a localhost server listening
 * at the ports passed
 *
 * @param[in] ports ports of the servers in the server pool
 */
Client::Client(std::vector<int> ports)
{
    int hash, clientfd;
    for (int port : ports)
    {
        hash = hash_value(port);
        clientfd = connect_server(port);
        Server *s = new Server(hash, port, clientfd);
        add_server_to_pool(s);
    }
}

/**
 * @brief Store a server instance to the state
 * of the client
 * @param[in] server_p pointer to a `Server` instance
 */
void Client::add_server_to_pool(Server *server_p)
{
    int n = server_pool.size(), i;
    Server *s;

    if (n == 0 || server_pool[n - 1]->hash < server_p->hash)
    {
        server_pool.push_back(server_p);
        return;
    }

    for (i = 0; i < n; i++)
    {
        s = server_pool[i];
        if (s->hash > server_p->hash)
        {
            server_pool.insert(server_pool.begin() + i, server_p);
            return;
        }
    }
}

/**
 * @brief Selects a server for a given key
 *
 * @param[in] key The key to store/fetch
 *
 * @return The server instance
 */
Server *Client::select_successor_server(std::string key)
{
    std::hash<std::string> str_hash_func;
    int key_hash = str_hash_func(key);

    for (Server *s : server_pool)
    {
        // The first alive server that has >= hash value
        if (s->clientfd >= 0 && s->hash >= key_hash)
        {
            return s;
        }
    }

    return server_pool[0];
}

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
bool Client::send_put_req(std::string key, std::string value, msg_t *response)
{
    msg_t *put_msg = create_put_msg(key, value);
    if (!put_msg)
    {
        return false;
    }

    Server *server_p = select_successor_server(key);
    bool success = false;

    send_msg(server_p->clientfd, put_msg);
    display_msg("[Client] Sent Request to server at port " + std::to_string(server_p->port),
                put_msg);

    // wait for acknowledgement
    if (read_msg(server_p->clientfd, response, -1) >= 0)
    {
        display_msg("[Client] Received Response", response);
        success = true;
    }
    else
    {
        success = false;
    }
    free(put_msg);
    return success;
}

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
std::string Client::send_get_req(std::string key, msg_t *response)
{
    msg_t *get_msg = create_get_msg(key);

    if (!get_msg)
    {
        return "";
    }

    Server *server_p = select_successor_server(key);
    std::string value = "";

    send_msg(server_p->clientfd, get_msg);
    display_msg("[Client] Sent Request to server at port " + std::to_string(server_p->port),
                get_msg);

    // wait for value
    if (read_msg(server_p->clientfd, response, -1) >= 0)
    {
        display_msg("[Client] Received Response", response);

        if (response->type == resp_hit_t)
            value = response->value;
    }

    free(get_msg);
    return value;
}

/**
 * @brief terminates connection with all servers
 */
void Client::close_client()
{
    for (Server *s : server_pool)
    {
        if (s->clientfd >= 0)
        {
            close(s->clientfd);
        }
    }
}