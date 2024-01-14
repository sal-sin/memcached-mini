#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "utils/message.hpp"
#include "sys/conn.hpp"

#define PORT 6000

using namespace std;

/**
 * @brief an instance of this class represents a single
 * server. This class contains all the functions that a
 * server should do
 */
class Server
{
public:
    int listenfd;
    unordered_map<string, string> kvs;

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
        while (true)
        {
            int connfd;
            msg_t *req_msg = make_msg_ref();

            // accept
            if ((connfd = accept_client(listenfd)) != -1)
            {
                // keep reading until EOF
                while (read_msg(connfd, req_msg) != -1)
                {
                    process_req(connfd, req_msg);

                    cout << "KV store state so far: " << endl;
                    print_map();
                }
            }
            close(connfd);
        }
    }

    /**
     * @brief process a request received from the client
     * and respond back
     * @param[in] connfd the fd to communicate with client
     * @param[in] req_msg the request received from the client
     */
    void process_req(int connfd, msg_t *req_msg)
    {
        msg_t *resp;
        switch (req_msg->type)
        {
        case req_put_t:
            kvs[req_msg->key] = req_msg->value;
            resp = create_ack_msg();
            break;
        case req_get_t:
            resp = kvs.count(req_msg->key) > 0 ? create_hit_msg(kvs[req_msg->key]) : create_miss_msg();
            break;
        default:
            printf("[Server] Invalid message type received, type = %d", req_msg->type);
        }
        send_msg(connfd, resp);
        free(resp);
    }

    /**
     * @brief display the current kv store state
     */
    void print_map()
    {
        for (pair<string, string> p : kvs)
        {
            cout << p.first << " -> " << p.second << endl;
        }
    }
};

int main(int argc, char const *argv[])
{
    Server server(PORT);
}