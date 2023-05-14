#pragma once

#include "DataTypes.h"
#include <thread>
#include <plog/Log.h>
#include "Buffer.h"
#include "File.h"

char *uuid_to_string(char *id, char *out);

class Connection {
private:
    SOCKET socket;
    bool shouldClose = false;
    std::thread *listenThread;
    Buffer *buffer;
    int state = 0;
    char **serverInfo;
    std::vector<Connection *> *connections;

    void listenPackages();

public:
    Connection(std::vector<Connection *> *connections, char **serverInfo, SOCKET socket);
};