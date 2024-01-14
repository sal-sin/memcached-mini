#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "utils/message.hpp"
#include "sys/conn.hpp"

#define PORT 6000

using namespace std;

/**
 * @brief an instance of this class represents a single
 * server. Instantiating a `Server` object will start
 * a localhost server that listens on the port passed
 */
class Server
{
public:
    int listenfd;
    unordered_map<string, string> kv_store;

    Server(int port)
    {
        listenfd = start_listener(PORT);
        accept_and_serve_forever();
    }

    /**
     * @brief continuously and sequentially keep accepting connections
     * serving requests
     */
    void accept_and_serve_forever()
    {
        int connfd;
        while (true)
        {
            // accept
            if ((connfd = accept_client(listenfd)) != -1)
            {
                process_requests(connfd);
            }
            close(connfd);
        }
    }

    /**
     * @brief process requests received from the client
     * and respond back until EOF is reached
     * @param[in] connfd the fd to communicate with client
     */
    void process_requests(int connfd)
    {
        msg_t *resp, *req_msg = make_msg_ref();
        // keep reading until EOF
        while (read_msg(connfd, req_msg, -1) != -1)
        {
            switch (req_msg->type)
            {
            case req_put_t:
                kv_store[req_msg->key] = req_msg->value;
                resp = create_ack_msg();
                print_map();
                break;
            case req_get_t:
                resp = kv_store.count(req_msg->key) > 0
                           ? create_hit_msg(kv_store[req_msg->key])
                           : create_miss_msg();
                break;
            default:
                printf("[Server] Invalid message type received, type = %d", req_msg->type);
            }

            // respond back to the client
            send_msg(connfd, resp);
            free(resp);
        }
    }

    /**
     * @brief display the current kv store state
     */
    void print_map()
    {
        cout << "===================" << endl;
        cout << "KV Store state" << endl;
        cout << "===================" << endl;
        for (pair<string, string> p : kv_store)
        {
            cout << p.first << " -> " << p.second << endl;
        }
        cout << "===================" << endl;
    }
};

int main(int argc, char const *argv[])
{
    Server server(PORT);
}