#define MAX_KSIZE 100
#define MAX_VSIZE 1000

/**
 * @brief Message types that can be sent to/from the
 * memcache server/client
 */
enum msg_type_t
{
    req_get_t,
    req_put_t,
    resp_ack_t,
    resp_hit_t,
    resp_miss_t,
};

/**
 * @brief `msg_t` type represents the format of a single
 * packet that can be sent to/from the server/client
 */
struct msg_t
{
    msg_type_t type;
    char key[MAX_KSIZE];
    char value[MAX_VSIZE];
};

/**
 * @brief Wait to read a message from as and when available
 * on connfd, and store in the location passed by reference
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] msg_p Pointer to the location where the packet
 * contents need to be stored
 * @return 1 if successful, else -1
 */
int read_msg(int connfd, msg_t *msg_p);

/**
 * @brief Send a message to client/server
 * @param[in] connfd File descriptor for client/server
 * communication on either side
 * @param[in] msg_p Pointer to the location where the message
 * contents are stored
 * @return 1 if successful, else -1
 */
int send_msg(int connfd, msg_t *msg_p);

/**
 * @brief Create a `put` message with a KV pair. Caller should
 * free the returned reference.
 * @param[in] key The key
 * @param[in] value The value
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_put_msg(const char key[], const char value[]);

/**
 * @brief Create a `get` message with just the key. Caller should
 * free the returned reference.
 * @param[in] key The key
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_get_msg(const char key[]);

/**
 * @brief Create an `ack` message. Caller should
 * free the returned reference.
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_ack_msg();

/**
 * @brief Create a `hit` message with the value.
 * Caller should free the returned reference.
 * @param[in] value The value to be responded with
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_hit_msg(const char value[]);

/**
 * @brief Create a `miss` message
 * Caller should free the returned reference.
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *create_miss_msg(const char value[]);

/**
 * @brief Create reference for a `msg_t` type struct.
 * Caller should free the returned reference.
 * @return Reference to a `msg_t` instance, null in case of error
 */
msg_t *make_msg_ref();