# Memcached-Mini

An implementation of a distributed, in-memory key-value storage system inspired by the design of [memcached](https://memcached.org/). This is one of my pet projects to keep my distributed systems skills sharp.

## Design overview

High-level design decisions made by developers of memcached can be found [here](https://github.com/memcached/memcached/wiki/Overview). The memcached client queries the key-value cache storage provided by the memcached server. Both the client and server are implemented as the client needs to know where to store/fetch a key, and the server needs to manage storage and implement an eviction policy.

I have used the following implementation approach for my project:

### Memcached Client

- Clients are initialized with the server endpoints available to them and are aware of these servers at all times.
- A client can send the following requests to any server belonging to the server pool it was initialized with:
  - Put: A request containing a key-value pair where the key maps to the value. The server responds with an acknowledgement. Keys and values must be within 100 and 1000 bytes respectively.
  - Get: A request containing a key. The server either responds with the value or indicates that the key is not present.
- For a given key, the client issues Get/Put requests to a single server. The client picks the server to contact using chord protocol that uses consistent hashing. The main idea is that a key will be stored on a server that has the smallest hash value greater than or equal to that of the key. More details can be found in [this paper describing the chord protocol](https://pdos.csail.mit.edu/papers/ton:chord/paper-ton.pdf). This offers the advantage of even load balancing under normal operation and in the event of server failure/rejoin. This also ensures fault tolerance and availability since one server failure doesn't impact all the keys stored in the system.
- The client should be able to detect server failures and rejoins. To keep this simple, the client will declare a server dead when no response is received from the server by a pre-defined timeout. Thenafter, the client will keep pinging the dead servers at certain time intervals to detect the server rejoins.

### Memcached Server

- The server responds to Get and Put requests issued by clients as described in the previous section.
  - The server must respond with an acknowledgement upon receiving a Put request.
  - The server must respond with a cache hit/miss event and a value (if hit) upon receiving a Get request.
- No two servers are aware of each other.
- Consistency management: All requests are processed in the order they are received by the server
- Eviction policy when cache gets full: `TBD`

## Usage
Go inside the repository and follow the steps below:

- Run a localhost memcached server.

      sh server.sh <port>

- Run a memcached client configured with the ports of localhost servers present in the server pool available:

      sh client.sh <server1_port> <server2_port> ...

## Features to implement

- [x] Utilities to get started

  - [x] Add a module for connection establishment b/w server & client
  - [x] Add a module defining the types of messages that can be received
  - [x] Add utilities to create messages
  - [x] Document the above
  - [x] `read()` timeout configuration using `poll()`

- [x] Single server KV store interacting with one client

  - [x] Put requests get reflected on the server's state
  - [x] Get requests from client result in appropriate response from server

- [x] Distributed KV Store

  - [x] Configure client with multiple servers
  - [x] Hash function
  - [x] Key-server mapper

- [x] User Interaction

  - [x] Create command line interface
  - [x] Test it manually

- [ ] Logging

  - [ ] Server log files
  - [ ] Client log files

- [ ] Concurrent request processing

  - [ ] Spawn new thread for every client
  - [ ] Protect kv store state from concurrent access

- [ ] Server failure handling
  - [ ] Keep track of disconnected servers in a data structure
  - [ ] Flag server as `inactive` after response `read()` timeout
  - [ ] Ping dead servers in specified intervals to check if they have come alive
  - [ ] Add logic to connect with rejoined server and flag as `active`
