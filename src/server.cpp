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

class Server
{
public:
    int listenfd;
    unordered_map<string, string> kvs;

    Server(int port)
    {
        listenfd = start_listener(PORT);
    }

    void accept_and_serve_forever()
    {
        while (true)
        {
            int connfd;
            msg_t *rcv_msg = make_msg_ref(), *ack = create_ack_msg();
            if ((connfd = accept_client(listenfd)) != -1)
            {
                while (read_msg(connfd, rcv_msg) != -1)
                {
                    send_msg(connfd, ack);
                    kvs[(*rcv_msg).key] = (*rcv_msg).value;

                    cout << "Map State so Far: " << endl;
                    print_map(kvs);
                }
            }
            close(connfd);
        }
    }
};

int main(int argc, char const *argv[])
{
    Server server(PORT);
    server.accept_and_serve_forever();
}