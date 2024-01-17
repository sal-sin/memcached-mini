#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "server/server.hpp"

int main(int argc, char const *argv[])
{
    int port;
    if (argc < 2)
    {
        printf("You must enter `port` as an argument\n");
        exit(1);
    }

    port = std::stoi(argv[1]);

    Server server(port);
    server.accept_and_serve_forever();
}