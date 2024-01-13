#include <unistd.h>
#include <iostream>
#include <vector>
#include "utils/message.hpp"
#include "sys/conn.hpp"

using namespace std;

#define PORT 6000

class Client
{
public:
    int clientfd;

    Client(int port)
    {
        clientfd = connect_server(PORT);
    }

    void send_put_req(string key, string value)
    {
        if (key.length() > MAX_KSIZE || value.length() > MAX_VSIZE)
        {
            printf("Cannot send key size greater than %d bytes \
                    or value size greater than %d bytes ",
                   MAX_KSIZE, MAX_VSIZE);
            return;
        }
        msg_t *put_msg, *rcv_msg;
        rcv_msg = make_msg_ref();
        put_msg = create_put_msg(key, value);

        send_msg(clientfd, put_msg);
        read_msg(clientfd, rcv_msg);
        free(put_msg);
        free(rcv_msg);
    }

    void send_get_req(string key)
    {
        if (key.length() > MAX_KSIZE)
        {
            printf("Key size cannot be greater than %d bytes", MAX_KSIZE);
            return;
        }

        msg_t *get_msg, *rcv_msg;
        rcv_msg = make_msg_ref();
        get_msg = create_get_msg(key);

        send_msg(clientfd, get_msg);
        read_msg(clientfd, rcv_msg);
        free(get_msg);
        free(rcv_msg);
    }
};

int main(int argc, char const *argv[])
{
    Client client(PORT);
    client.send_put_req("Me", "Hello World");
    client.send_put_req("World", "Hello You");
}