#include <unistd.h>
#include <iostream>
#include <vector>
#include "utils/message.hpp"
#include "sys/conn.hpp"

using namespace std;

#define PORT 6000

/**
 * @brief represents a single client. This class contains
 * public functions that can be interacted with by the application.
 */
class Client
{
public:
    int clientfd;

    Client(int port)
    {
        clientfd = connect_server(PORT);
    }

    void send_put_req(string key, string value, msg_t *response)
    {
        msg_t *put_msg;
        put_msg = create_put_msg(key, value);

        if (!put_msg)
        {
            return;
        }

        send_msg(clientfd, put_msg);
        read_msg(clientfd, response, -1); // wait for acknowledgement
        free(put_msg);
    }

    void send_get_req(string key, msg_t *response)
    {
        msg_t *get_msg;
        get_msg = create_get_msg(key);

        if (!get_msg)
        {
            return;
        }

        send_msg(clientfd, get_msg);
        read_msg(clientfd, response, -1); // wait for value
        free(get_msg);
    }
};

int main(int argc, char const *argv[])
{
    Client client(PORT);
    msg_t *resp = make_msg_ref();
    client.send_put_req("Me", "Hello World", resp);
    client.send_put_req("World", "Hello You", resp);
    client.send_get_req("Me", resp);
}