#include <unistd.h>
#include <iostream>
#include "utils/message.hpp"
#include "sys/conn.hpp"

#define PORT 6000

int main(int argc, char const *argv[])
{
    int client_fd = connect_server(PORT);
    msg_t *put_msg, *put_msg_2, *rcv_msg;
    rcv_msg = make_msg_ref();

    put_msg = create_put_msg((char *)"Me", (char *)"Hello World");
    send_msg(client_fd, put_msg);
    read_msg(client_fd, rcv_msg);
    free(put_msg);

    put_msg_2 = create_put_msg((char *)"World", (char *)"Hello You");
    send_msg(client_fd, put_msg_2);
    read_msg(client_fd, rcv_msg);
    free(put_msg_2);

    close(client_fd);
    return 0;
}