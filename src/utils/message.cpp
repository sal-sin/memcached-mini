#include <iostream>
#include <unistd.h>
#include <poll.h>
#include "message.hpp"

/**
 * @brief validates that a given key and value are within
 * size specifications
 *
 * @param[in] key the key string
 * @param[in] value the value string
 *
 * @return true if valid, else false
 */
static bool validate_kv_size(std::string key, std::string value)
{
    if (key.length() > MAX_KSIZE)
    {
        printf("Cannot msg with key size greater than %d bytes",
               MAX_KSIZE);
        return false;
    }

    if (value.length() > MAX_VSIZE)
    {
        printf("Cannot msg with value size greater than %d bytes",
               MAX_VSIZE);
        return false;
    }
    return true;
}

/**
 * @brief Wait to read a message from as and when available
 * on connfd, and store in the location passed by reference
 *
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] msg_p Pointer to the location where the packet
 * contents need to be stored
 * @param[in] timeout_ms -1 if waiting forever is desired,
 * else pass timeout value in milliseconds.
 *
 * @return 1 if successful, else -1
 */
int read_msg(int connfd, msg_t *msg_p, int timeout_ms)
{
    struct pollfd pfd = {.fd = connfd, .events = POLL_IN, .revents = POLL_IN};
    if (timeout_ms > 0 && poll(&pfd, 1, timeout_ms) == 0)
    {
        printf("Timed out during read attempt\n");
        return -1;
    }

    int len = read(connfd, msg_p, sizeof(*msg_p));

    if (len < 0)
    {
        perror("Error during read\n");
        return -1;
    }
    if (len == 0)
    {
        printf("EOF reached\n");
        return -1;
    }

    printf("Read from Conn | Len = %d | Type = %d | %s: %s\n",
           len, (*msg_p).type, (*msg_p).key, (*msg_p).value);
    return 0;
}

/**
 * @brief Send a message to client/server
 *
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] msg_p Pointer to the location where the message
 * contents are stored
 *
 * @return 1 if successful, else -1
 */
int send_msg(int connfd, msg_t *msg_p)
{
    if (write(connfd, msg_p, sizeof(*msg_p)) < 0)
    {
        perror("Error during writing message to conn");
        return -1;
    }
    printf("Msg sent\n");
    return 0;
}

/**
 * @brief Create reference for a `msg_t` type struct.
 * Caller should free the returned reference.
 *
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *make_msg_ref()
{
    msg_t *msg = (msg_t *)malloc(sizeof(msg_t));
    return msg;
}

/**
 * @brief Create a `put` message with a KV pair. Caller should
 * free the returned reference.
 *
 * @param[in] key The key
 * @param[in] value The value
 *
 * @return Reference to a `msg_t` instance, NULL in case of
 * greater than specified size strings passed
 */
msg_t *create_put_msg(std::string key, std::string value)
{
    if (!validate_kv_size(key, value))
    {
        return NULL;
    }

    msg_t *msg = make_msg_ref();
    msg->type = req_put_t;
    strcpy(msg->key, key.c_str());
    strcpy(msg->value, value.c_str());
    return msg;
}

/**
 * @brief Create a `get` message with just the key. Caller should
 * free the returned reference.
 *
 * @param[in] key The key
 *
 * @return Reference to a `msg_t` instance, NULL in case of
 * greater than specified size strings passed
 */
msg_t *create_get_msg(std::string key)
{
    if (!validate_kv_size(key, ""))
    {
        return NULL;
    }
    msg_t *msg = make_msg_ref();
    msg->type = req_get_t;
    strcpy(msg->key, key.c_str());
    return msg;
}

/**
 * @brief Create an `ack` message. Caller should
 * free the returned reference.
 *
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_ack_msg()
{
    msg_t *msg = make_msg_ref();
    msg->type = resp_ack_t;
    return msg;
}

/**
 * @brief Create a `hit` message with the value.
 * Caller should free the returned reference.
 *
 * @param[in] value The value to be responded with
 *
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_hit_msg(std::string value)
{
    msg_t *msg = make_msg_ref();
    msg->type = resp_hit_t;
    strcpy(msg->value, value.c_str());
    return msg;
}

/**
 * @brief Create a `miss` message
 * Caller should free the returned reference.
 *
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_miss_msg()
{
    msg_t *msg = make_msg_ref();
    msg->type = resp_miss_t;
    return msg;
}