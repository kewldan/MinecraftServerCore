#include "Server.h"

Server::Server(const char *port) {
    server = new SocketServer(port);

    serverInfo["version"]["name"] = "1.19.4";
    serverInfo["version"]["protocol"] = 762;
    serverInfo["players"]["max"] = 20;
    serverInfo["players"]["online"] = 0;
    serverInfo["description"] = "Custom C++ Minecraft server core\nIn development";

    if (File::exists("./favicon.png")) {
        int size = 0;
        const char *data = File::readFile("./favicon.png", &size);
        std::string base64 = Base64::base64_encode(reinterpret_cast<const BYTE *>(data), size);
        static const char *prefix = "data:image/png;base64,";
        int bsize = (int) strlen(prefix) + base64.size() + 1;
        char *buffer = new char[bsize];
        strcpy_s(buffer, bsize, prefix);
        strcat_s(buffer, bsize, base64.c_str());
        buffer[bsize - 1] = 0;
        serverInfo["favicon"] = const_cast<const char *>(buffer);
        PLOGI << "Favicon loaded";
    }

    std::string infoStr = serverInfo.dump();
    serverInfoBuffer = new char[infoStr.size() + 1];
    strcpy_s(serverInfoBuffer, infoStr.size() + 1, infoStr.c_str());
}

void Server::run() {
    listenThread = new std::thread(&Server::listenSockets, this);
    tickThread = new std::thread(&Server::tickLoop, this);
}

void Server::listenSockets() {
    while (!shouldClose) {
        auto socket = server->acceptSocket();
        auto connection = new Connection(&connections, &serverInfoBuffer, socket);
        connections.push_back(connection);
    }
}

long long millis(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void Server::tickLoop() {
    static long long lastTick = millis(), tickCount = 0;

    while(true){
        if(millis() > lastTick + 50){


            tickCount++;
            lastTick = millis();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}
