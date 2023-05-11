#include <iostream>
#include "DataTypes.h"
#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"
#include "plog/Initializers/ConsoleInitializer.h"
#include "plog/Formatters/FuncMessageFormatter.h"
#include "json.hpp"
#include <thread>

WSADATA wsData;
struct addrinfo hints;
struct addrinfo* result;
nlohmann::json serverInfo;

void handleConnection(SOCKET socket){
    std::thread::id thr = std::this_thread::get_id();
    int handshakeMode = 0;
    char* buf = new char[65536];
    while(true){
        int dataLength;
        VInt packetLength = VInt::read(socket);
        VInt PID = VInt::read(socket);
        dataLength = packetLength.getValue() - PID.getSize();

        PLOGD << "[" << thr << "] PL/ID/DL: " << packetLength.getValue() << "/" << PID.getValue() << "/" << dataLength;
        if(PID.getValue() == 0){
            if(handshakeMode == 0){
                VInt protocolVersion = VInt::read(socket);
                VInt addressLength = VInt::read(socket);
                char *address = new char[addressLength.getValue()];
                address[addressLength.getValue()] = 0;
                recv(socket, address, addressLength.getValue(), 0);
                char *portBuffer = new char[2];
                recv(socket, portBuffer, 2, 0);
                unsigned short port;
                memcpy(&port, portBuffer, 2);
                VInt nextState = VInt::read(socket);
                PLOGI << "[" << thr << "] Handshake packet (Handshake (" << nextState.getValue() << "))";
                handshakeMode = nextState.getValue();
            }else if(handshakeMode == 1){
                int offset = 0;
                std::string infoStr = serverInfo.dump();
                const char* info = infoStr.c_str();
                VInt id(0x0);
                VInt infoLength(strlen(info));
                VInt length(id.getSize() + infoLength.getSize() + strlen(info));
                offset += length.write(buf);
                offset += id.write(buf + offset);
                offset += infoLength.write(buf + offset);
                memcpy(buf + offset, info, strlen(info));
                offset += strlen(info);
                send(socket, buf, offset, 0);
                PLOGI << "[" << thr << "] Handshake packet (Info)";
            }else if(handshakeMode == 2){
                VInt nicknameLength = VInt::read(socket);
                char* nickname = new char[nicknameLength.getValue()];
                nickname[nicknameLength.getValue()] = 0;
                recv(socket, nickname, nicknameLength.getValue(), 0);
                bool hasUUID;
                recv(socket, reinterpret_cast<char *>(&hasUUID), 1, 0);
                if(hasUUID){
                    char* uuid = new char[16];
                    recv(socket, uuid, 16, 0);
                }
                PLOGI << "[" << thr << "] Login start (" << nickname << ")";
                const char* serverId = "Appear empty";
                VInt id(0x1);
                VInt serverIdLength(strlen(serverId));
                VInt publicKeyLength(0);
                VInt privateKeyLength(0);
                VInt length(id.getSize() + serverIdLength.getSize() + serverIdLength.getValue() + publicKeyLength.getSize() + publicKeyLength.getValue() + privateKeyLength.getSize() + privateKeyLength.getValue());
                int offset = 0;
                offset += id.write(buf);
                offset += serverIdLength.write(buf + offset);
                memcpy(buf + offset, serverId, strlen(serverId));
                offset += strlen(serverId);
                offset += publicKeyLength.write(buf + offset);
                offset += privateKeyLength.write(buf + offset);
                send(socket, buf, offset, 0);
            }
        }else if(PID.getValue() == 1){
            long number = 0;
            recv(socket, reinterpret_cast<char *>(&number), 8, 0);
            int offset = 0;
            VInt id(0x1);
            VInt length(id.getSize() + 8);
            offset += length.write(buf);
            offset += id.write(buf + offset);
            memcpy(buf + offset, &number, 8);
            offset += 8;
            send(socket, buf, offset, 0);
            PLOGI << "[" << thr << "] Ping packet";
        }else{
            PLOGE << "[" << thr << "] Unknown packet (" << PID.getValue() << ")";
        }
    }
    delete[] buf;
}

int main() {
    std::remove("latest.log");
    plog::init(plog::debug, "latest.log");
    plog::get()->addAppender(new plog::ColorConsoleAppender<plog::FuncMessageFormatter>());

    serverInfo["version"]["name"] = "1.19.4";
    serverInfo["version"]["protocol"] = 762;
    serverInfo["players"]["max"] = -3;
    serverInfo["players"]["online"] = 4;
    serverInfo["description"] = "Custom C++ Minecraft server core\nIn development";

    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        PLOGF << "Failed to open socket";
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(nullptr, "25565", &hints, &result) != 0) {
        PLOGF << "getaddrinfo failed";
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        PLOGF << "socket failed with error: " << WSAGetLastError();
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        PLOGF << "bind failed with error: " << WSAGetLastError();
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        PLOGF << "listen failed with error: " << WSAGetLastError();
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    PLOGI << "Server is ready";

    struct sockaddr_in addr{};
    int l = sizeof(addr);

    std::vector<std::thread*> connectionThreads;

    while (true) {
        SOCKET clientSocket = accept(ListenSocket, (struct sockaddr *) &addr, &l);
        if (clientSocket == INVALID_SOCKET) {
            PLOGE << "accept failed with error: " << WSAGetLastError();
            continue;
        }
        auto *connectionThread = new std::thread(handleConnection, clientSocket);
        connectionThreads.push_back(connectionThread);
    }
    return 0;
}
