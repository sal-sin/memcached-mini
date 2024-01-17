#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "server/server.hpp"

int main(int argc, char const *argv[])
{
    Server server(PORT);
    server.accept_and_serve_forever();
}