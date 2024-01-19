/**
 * @file src/runclient.cpp
 *
 * @brief This program instantiates a `Client` instance as declared
 * in ./client/client.hpp. The client must be initialized with a pool
 * of localhost servers. Ports of servers in the pool are passed through
 * command line arguments to this programme. Once a `Client` is
 * initialized, this program offers an command line interface to interact
 * with memcached servers via Get and Put requests.
 */

#include <unistd.h>
#include <iostream>
#include <vector>
#include "client/client.hpp"
#include "utils/colors.hpp"

using namespace std;

void start_client_interface(Client cl);

int main(int argc, char const *argv[])
{
    vector<int> ports;
    int port;
    if (argc < 2)
    {
        printf("You must enter server pool ports as command line arguments\n");
        exit(1);
    }

    cout << YELLOW << "Client attempting to connect to " << argc - 1
         << " localhost servers\n"
         << endl;

    for (int port_idx = 1; port_idx < argc; port_idx++)
    {
        port = stoi(argv[port_idx]);
        ports.push_back(port);
    }

    Client client(ports);
    start_client_interface(client);

    return 0;
}

/**
 * @brief provides a command line interface for a
 * user to issue requests to the pool of memcached
 * servers
 * @param[in] cl a `Client` instance
 */
void start_client_interface(Client cl)
{
    int op;
    bool success;
    string key, value, response_val;
    msg_t *resp = make_msg_ref();

    cout << GREEN << "========================" << RESET << endl;
    cout << GREEN << "Started Memcached Client" << RESET << endl;
    cout << GREEN << "========================" << RESET << endl;

    while (true)
    {
        cout << "\nAvaialble operations: " << endl;
        cout << "\t1. Get" << endl;
        cout << "\t2. Put" << endl;
        cout << "\t3. Quit" << endl;
        cout << "Choose an operation: ";
        cin >> op;

        switch (op)
        {
        case 1:
            cout << "Enter the key to fetch: ";
            cin >> key;
            response_val = cl.send_get_req(key, resp);
            if (response_val.length() > 0)
            {
                cout << "\tReceived value: "
                     << GREEN << response_val << RESET << endl;
            }
            else
            {
                cout << RED << "\tValue not found" << RESET << endl;
            }
            break;

        case 2:
            cout << "Enter the key to put: ";
            cin >> key;
            cout << "Enter the value to associate with key: ";
            cin >> value;

            success = cl.send_put_req(key, value, resp);
            if (success)
            {
                cout << "\tPut Successful for key: "
                     << GREEN << key << RESET << endl;
            }
            else
            {
                cout << "\t" << RED << "Server did not respond" << RESET << endl;
            }
            break;

        case 3:
            cout << RED << "\n================" << RESET << endl;
            cout << RED << "Closing Client" << RESET << endl;
            cout << RED << "================" << RESET << endl;
            cl.close_client();
            return;

        default:
            break;
        }
    }
}