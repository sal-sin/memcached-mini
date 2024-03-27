/**
 * @file src/runserver.cpp
 *
 * @brief This program instantiates a `Server` instance as declared
 * in ./server/server.hpp, and starts it. The server will always run
 * on localhost and on the port passed as a command line argument to
 * this program.
 */

#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "server/server.hpp"

int main(int argc, char const *argv[])
{
    int port, opt;
    if (argc < 2)
    {
        printf("You must enter `port` as an argument\n");
        exit(1);
    }

    port = std::stoi(argv[1]);
    Server server(port, true);

    while (1)
    {
        std::cout << "Enter 0 to Quit server: " << std::endl;
        std::cin >> opt;
        if (opt == 0)
        {
            server.close_server();
            sleep(1);
            break;
        }
    }
}