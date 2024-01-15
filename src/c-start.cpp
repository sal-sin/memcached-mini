#include <unistd.h>
#include <iostream>
#include <vector>
#include "client/client.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    Client client(PORT);
    msg_t *resp = make_msg_ref();
    client.send_put_req("Me", "Hello World", resp);
    client.send_put_req("World", "Hello You", resp);
    client.send_get_req("Me", resp);
}