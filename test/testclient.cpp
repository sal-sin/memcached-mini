/**
 * @file test/runtests.cpp
 *
 * @brief TODO
 */

#include <unistd.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include "../src/client/client.hpp"
#include "../src/server/server.hpp"
#include "../src/utils/colors.hpp"

using namespace std;

class TestClient : public Client
{
    using Client::Client;

public:
    bool test_all_servers_saved(int expected_num_servers)
    {
        int n = server_pool.size();
        if (expected_num_servers != n)
            return false;
        return true;
    }

    bool test_server_pool_hash_ordered()
    {
        int n = server_pool.size();
        ServerMeta *s, *prev = NULL;

        for (int i = 1; i < n; i++)
        {
            s = server_pool[i];
            if (prev && s->hash < prev->hash)
                return false;
            prev = s;
        }
        return true;
    }

    bool test_successor_server(std::string key, int expected_port)
    {
        ServerMeta *s = select_successor_server(key);
        if (s->port != expected_port)
            return false;
        return true;
    }

    bool test_put(std::string key, std::string val)
    {
        msg_t *resp = make_msg_ref();
        bool success = false;
        send_put_req(key, val, resp);
        if (resp->type == resp_ack_t)
            success = true;
        free(resp);
        return success;
    }

    bool test_get_hit(std::string key, std::string val)
    {
        msg_t *resp = make_msg_ref();
        bool success = false;
        send_get_req(key, resp);
        if (resp->type == resp_hit_t && resp->value == val)
            success = true;
        free(resp);
        return success;
    }

    bool test_get_miss(std::string key)
    {
        msg_t *resp = make_msg_ref();
        bool success = false;
        send_get_req(key, resp);
        if (resp->type == resp_miss_t)
            success = true;
        free(resp);
        return success;
    }
};

void test(string name, bool success)
{
    std::cout << "\t" << name << ": ";
    if (success)
        std::cout << GREEN << "PASS";
    else
        std::cout << RED << "FAIL";
    cout << RESET << std::endl;
    assert(success);
}

void testBasicClientNoServer()
{
    vector<int> ports = {1000, 6060, 8000, 9000};
    TestClient cl(ports);

    cout << "TEST: " << __FUNCTION__ << endl;
    test("test_all_servers_saved", cl.test_all_servers_saved(4));
    test("test_server_pool_hash_ordered", cl.test_server_pool_hash_ordered());

    cl.close_client();
}

void testBasicClientOneServer()
{
    Server server(6060);
    vector<int> ports = {1000, 6060, 8000, 9000};
    TestClient cl(ports);

    cout << "TEST: " << __FUNCTION__ << endl;
    test("test_all_servers_saved", cl.test_all_servers_saved(4));
    test("test_server_pool_hash_ordered", cl.test_server_pool_hash_ordered());
    test("test_successor_server", cl.test_successor_server("key1", 6060));
    test("test_put: (key1, val1)", cl.test_put("key1", "val1"));
    test("test_get_hit: (key1, expected: val1)", cl.test_get_hit("key1", "val1"));
    test("test_get_miss: (key1, expected: miss)", cl.test_get_miss("key2"));

    cl.close_client();
    server.close_server();
}

int main(int argc, char const *argv[])
{
    testBasicClientNoServer();
    testBasicClientOneServer();
    return 0;
}