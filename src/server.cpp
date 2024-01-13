#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "utils/message.hpp"
#include "sys/conn.hpp"

#define PORT 6000

using namespace std;

void print_map(unordered_map<string, string> m)
{
    for (pair<string, string> p : m)
    {
        cout << p.first << " -> " << p.second << endl;
    }
}

int main(int argc, char const *argv[])
{
    int listenfd = start_listener(PORT);
    int connfd = accept_client(listenfd);

    unordered_map<string, string> kvs;
    msg_t *rcv_msg = make_msg_ref(), *ack = create_ack_msg();

    read_msg(connfd, rcv_msg);
    send_msg(connfd, ack);
    kvs[(*rcv_msg).key] = (*rcv_msg).value;

    read_msg(connfd, rcv_msg);
    send_msg(connfd, ack);
    kvs[(*rcv_msg).key] = (*rcv_msg).value;
    free(rcv_msg);

    print_map(kvs);

    // close everything
    close(connfd);
    close(listenfd);
    return 0;
}