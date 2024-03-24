clear
g++ -std=c++17 -o temp2 ./src/runclient.cpp ./src/utils/message.cpp ./src/utils/conn.cpp ./src/client/client.cpp ./src/hash/hash.cpp ./src/client/connection.cpp
./temp2 "$@"