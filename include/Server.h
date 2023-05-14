#pragma once

#include "network/SocketServer.h"
#include "json.hpp"
#include <vector>
#include "Connection.h"
#include <thread>
#include "io/Filesystem.h"
#include "Base64.h"

class Server {
private:
    Engine::SocketServer *server;
    std::vector<Connection *> connections;
    nlohmann::json serverInfo;
    char *serverInfoBuffer;
    std::thread *listenThread{}, *tickThread{};
    bool shouldClose = false;

    void listenSockets();
    void tickLoop();
public:
    explicit Server(const char *port);

    void run();
};