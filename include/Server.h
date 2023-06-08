#pragma once

#include "TcpServerSocket.hpp"
#include "json.hpp"
#include <vector>
#include "Connection.h"
#include <thread>
#include "io/Filesystem.h"
#include "Base64.h"

class Server {
private:
    TcpServerSocket *server;
    std::vector<Connection *> connections;
    nlohmann::json serverInfo;
    char *serverInfoBuffer;
    std::thread *listenThread{}, *tickThread{};
    bool shouldClose = false;

    void listenSockets();
    void tickLoop();
public:
    explicit Server(short port);

    void run();
};