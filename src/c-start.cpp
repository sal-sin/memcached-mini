#include <unistd.h>
#include <iostream>
#include <vector>
#include "client/client.hpp"

int main(int argc, char const *argv[])
{
    std::vector<int> ports;
    int port;
    if (argc < 2)
    {
        printf("You must enter server pool ports as command line arguments\n");
        exit(1);
    }

    printf("Attempting to connect to %d localhost servers\n", argc);

    for (int port_idx = 1; port_idx < argc; port_idx++)
    {
        port = std::stoi(argv[port_idx]);
        ports.push_back(port);
    }

    Client client(ports);

    msg_t *resp = make_msg_ref();
    client.send_put_req("Me", "Hello World", resp);
    client.send_put_req("World", "Hello You", resp);
    client.send_get_req("Me", resp);

    return 0;
}