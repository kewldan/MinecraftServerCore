#pragma once

#include <plog/Log.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

class SocketServer {
private:
    WSADATA wsData;
    struct addrinfo hints;
    struct addrinfo *result;
    SOCKET ListenSocket;
public:
    explicit SocketServer(const char *port);

    SOCKET acceptSocket() const;
};
