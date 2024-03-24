/**
 * @file /src/client/client.cpp
 *
 * @brief This file contains the implementation of the Client API methods
 * declared in /src/client/client.hpp
 */

#include <unistd.h>
#include <iostream>
#include <thread>
#include "client.hpp"
#include "../utils/conn.hpp"
#include "../hash/hash.hpp"

/** The client will try to connect
 * with disconnected servers in
 * `POLL_INTERVAL` second intervals */
unsigned int POLL_INTERVAL = 5;

/** Wait for `RESPONSE_TIMEOUT` milliseconds
 * before declaring the server disconnected */
unsigned int RESPONSE_TIMEOUT = 2000;

/**
 * @brief Client starts by connecting to a localhost server listening
 * at the ports passed
 *
 * @param[in] ports ports of the servers in the server pool
 */
Client::Client(std::vector<int> ports)
{
    close_flag = false;
    for (int port : ports)
    {
        Connection *s = new Connection(port);
        add_server_to_pool(s);
    }

    std::thread poll_thread(&Client::poll_disconnected_servers, this);
    poll_thread.detach();
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

    Connection *server_p = select_successor_server(key);
    if (!server_p)
    {
        display_msg("[Client] No server alive at the moment for", put_msg);
        return false;
    }

    bool success = false;

    send_msg(server_p->get_fd(), put_msg);
    display_msg("[Client] Sent Request to server at port " + std::to_string(server_p->get_port()),
                put_msg);

    // wait for acknowledgement
    if (read_msg(server_p->get_fd(), response, RESPONSE_TIMEOUT) >= 0)
    {
        display_msg("[Client] Received Response", response);
        success = true;
    }
    else
    {
        server_p->disconnect();
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

    Connection *server_p = select_successor_server(key);
    if (!server_p)
    {
        display_msg("[Client] No server alive at the moment for", get_msg);
        return "";
    }

    std::string value = "";
    send_msg(server_p->get_fd(), get_msg);
    display_msg("[Client] Sent Request to server at port " + std::to_string(server_p->get_port()),
                get_msg);

    // wait for value
    if (read_msg(server_p->get_fd(), response, RESPONSE_TIMEOUT) >= 0)
    {
        display_msg("[Client] Received Response", response);

        if (response->type == resp_hit_t)
            value = response->value;
    }
    else
    {
        server_p->disconnect();
    }

    free(get_msg);
    return value;
}

/**
 * @brief terminates connection with all servers
 */
void Client::close_client()
{
    close_mutex.lock();
    close_flag = true;
    close_mutex.unlock();

    for (Connection *s : server_pool)
    {
        if (s->is_connected())
        {
            s->disconnect();
        }
    }
}

/**
 * @brief Store a server instance to the state
 * of the client
 * @param[in] server_p pointer to a `Server` instance
 */
void Client::add_server_to_pool(Connection *server_p)
{
    int n = server_pool.size(), i;
    Connection *s;

    if (n == 0 ||
        server_pool[n - 1]->get_port_hash() < server_p->get_port_hash())
    {
        server_pool.push_back(server_p);
        return;
    }

    for (i = 0; i < n; i++)
    {
        s = server_pool[i];
        if (s->get_port_hash() > server_p->get_port_hash())
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
Connection *Client::select_successor_server(std::string key)
{
    unsigned int key_hash = get_hash(key);
    Connection *first_alive = NULL;

    for (Connection *s : server_pool)
    {
        // The first alive server that has >= hash value
        if (s->is_connected())
        {
            if (!first_alive)
                first_alive = s;
            if (s->get_port_hash() >= key_hash)
                return s;
        }
    }

    return first_alive;
}

/**
 * @brief Try connecting to servers that have been
 * disconnected. This function is expected to run
 * in the background throughout while the client is
 * active
 */
void Client::poll_disconnected_servers()
{
    while (true)
    {
        sleep(POLL_INTERVAL);

        close_mutex.lock();

        if (close_flag)
        {
            close_mutex.unlock();
            return;
        }

        for (Connection *s : server_pool)
        {
            if (!s->is_connected())
                s->connect();
        }
        close_mutex.unlock();
    }
}