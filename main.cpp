#include <iostream>
#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"
#include "plog/Initializers/ConsoleInitializer.h"
#include "plog/Formatters/FuncMessageFormatter.h"
#include "Server.h"

int main() {
    std::remove("latest.log");
    plog::init(plog::debug, "latest.log");
    plog::get()->addAppender(new plog::ColorConsoleAppender<plog::FuncMessageFormatter>());

    PLOGI << "Starting minecraft server version 1.19.4";

    auto server = new Server("25565");

    PLOGI << "Server is ready";

    server->run();

    while (true) {
        std::string commandStr;
        std::cin >> commandStr;
        const char *command = commandStr.c_str();
        if (!strcmp(command, "exit")) {
            break;
        }else if(!strcmp(command, "gc")){
            PLOGI << "Memory ()";
        }
    }
    return 0;
}
