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
 * @brief Client starts by connecting to a localhost server listening
 * at the port passed
 *
 * @param[in] port The port of the server
 */
Client::Client(int port)
{
    clientfd = connect_server(PORT);
}

/**
 * @brief sends a `put` request to the server it is connected to
 *
 * @param[in] key the key (max length = 100 bytes)
 * @param[in] value the value (max length = 1000 bytes)
 * @param[in] response `msg_t` location where the server's
 * reply can be saved
 */
void Client::send_put_req(std::string key, std::string value, msg_t *response)
{
    msg_t *put_msg = create_put_msg(key, value);

    if (!put_msg)
    {
        return;
    }

    send_msg(clientfd, put_msg);
    read_msg(clientfd, response, -1); // wait for acknowledgement
    free(put_msg);
}

/**
 * @brief sends a `get` request to the server it is connected to
 *
 * @param[in] key the key (max length = 100 bytes)
 * @param[in] response `msg_t` location where the server's
 * reply can be saved
 */
void Client::send_get_req(std::string key, msg_t *response)
{
    msg_t *get_msg = create_get_msg(key);

    if (!get_msg)
    {
        return;
    }

    send_msg(clientfd, get_msg);
    read_msg(clientfd, response, -1); // wait for value
    free(get_msg);
}