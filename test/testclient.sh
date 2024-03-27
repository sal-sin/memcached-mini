clear
g++ -std=c++17 -o temp2 ./testclient.cpp ../src/utils/message.cpp ../src/utils/logger.cpp ../src/utils/conn.cpp ../src/client/client.cpp ../src/hash/hash.cpp ../src/client/connection.cpp ../src/server/server.cpp
./temp2 "$@"