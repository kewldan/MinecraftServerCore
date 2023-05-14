#pragma once

#include "SocketServer.h"
#include "json.hpp"
#include <vector>
#include "Connection.h"
#include <thread>
#include "File.h"
#include "Base64.h"

class Server {
private:
    SocketServer *server;
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