#include "SocketServer.h"

SocketServer::SocketServer(const char *port) {
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        PLOGF << "Failed to open socket";
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(nullptr, "25565", &hints, &result) != 0) {
        PLOGF << "getaddrinfo failed";
        WSACleanup();
        return;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        PLOGF << "socket failed with error: " << WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    if (bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen) == SOCKET_ERROR) {
        PLOGF << "bind failed with error: " << WSAGetLastError();
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        PLOGF << "listen failed with error: " << WSAGetLastError();
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
}

SOCKET SocketServer::acceptSocket() const {
    struct sockaddr_in addr{};
    int l = sizeof(addr);
    SOCKET clientSocket = accept(ListenSocket, (struct sockaddr *) &addr, &l);
    if (clientSocket == INVALID_SOCKET) {
        PLOGE << "accept failed with error: " << WSAGetLastError();
        return INVALID_SOCKET;
    }
    return clientSocket;
}
