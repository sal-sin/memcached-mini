clear
g++ -std=c++17 -o temp1 ./src/runserver.cpp ./src/utils/message.cpp ./src/utils/logger.cpp ./src/utils/conn.cpp ./src/server/server.cpp
./temp1 "$1"