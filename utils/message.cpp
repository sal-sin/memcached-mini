#include <iostream>
#include <unistd.h>
#include "message.hpp"

#define RESPONSE_BUFSIZE 1000

using namespace std;

/**
 * @brief Wait to read a message from as and when available
 * on connfd, and store in the location passed by reference
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] msg_p Pointer to the location where the packet
 * contents need to be stored
 * @return 1 if successful, else -1
 */
int read_msg(int connfd, msg_t *msg_p)
{
    char ack[] = "Msg Rcvd";
    int len = read(connfd, msg_p, sizeof(*msg_p));

    if (len < 0)
    {
        perror("Error during read");
        return -1;
    }

    // TODO: Handle len = 0

    printf("Read from Conn | Len = %d | %s: %s\n", len, (*msg_p).key, (*msg_p).value);
    write(connfd, ack, strlen(ack));
    printf("Ack message sent\n");
    return 0;
}

/**
 * @brief Send a message to client/server
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] msg_p Pointer to the location where the message
 * contents are stored
 * @return 1 if successful, else -1
 */
int send_msg(int connfd, msg_t *msg_p)
{
    int len;
    char buffer[RESPONSE_BUFSIZE + 1];

    if (write(connfd, msg_p, sizeof(*msg_p)) < 0)
    {
        perror("Error during writing message to conn");
        return -1;
    }

    printf("Msg sent\n");

    // read response in a buffer with bounded size
    len = read(connfd, buffer, RESPONSE_BUFSIZE);
    printf("Received | Len = %d | Obj = %s\n", len, buffer);
    return 0;
}

/**
 * @brief Create reference for a `msg_t` type struct.
 * Caller should free the returned reference.
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
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] key The key
 * @param[in] value The value
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_put_msg(char key[], char value[])
{
    msg_t *msg = make_msg_ref();
    (*msg).type = put_t;
    strcpy((*msg).key, key);
    strcpy((*msg).value, value);
    return msg;
}

/**
 * @brief Create a `get` message with just the key. Caller should
 * free the returned reference.
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] key The key
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_get_msg(char key[])
{
    msg_t *msg = make_msg_ref();
    (*msg).type = get_t;
    strcpy((*msg).key, key);
    return msg;
}

/**
 * @brief Create an `ack` message. Caller should
 * free the returned reference.
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_ack_msg()
{
    msg_t *msg = make_msg_ref();
    (*msg).type = ack_t;
    return msg;
}