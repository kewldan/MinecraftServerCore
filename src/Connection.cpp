#include "Connection.h"

char *uuid_to_string(char *id, char *out) {
    static const char TOHEXCHAR[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    char *c = out;
    int src_byte = 0;
    for (int i = 0; i < 4; ++i) {
        *c++ = TOHEXCHAR[(id[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 2; ++i) {
        *c++ = TOHEXCHAR[(id[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 2; ++i) {
        *c++ = TOHEXCHAR[(id[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 2; ++i) {
        *c++ = TOHEXCHAR[(id[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id[src_byte] & 0xF];
        ++src_byte;
    }
    *c++ = '-';

    for (int i = 0; i < 6; ++i) {
        *c++ = TOHEXCHAR[(id[src_byte] >> 4) & 0xF];
        *c++ = TOHEXCHAR[id[src_byte] & 0xF];
        ++src_byte;
    }

    return out;
}

Connection::Connection(std::vector<Connection *> *connections, char **serverInfo, SOCKET socket) : connections(
        connections), socket(socket), serverInfo(serverInfo) {
    buffer = new Buffer(65536);

    listenThread = new std::thread(&Connection::listenPackages, this);
}

void Connection::listenPackages() {
    while (!shouldClose) {
        auto packetLength = VInt::read(socket);
        if (!packetLength) break;
        auto PID = VInt::read(socket);
        if (!PID) break;

        if (PID->getValue() == 0) {
            if (state == 0) {
                auto protocolVersion = VInt::read(socket);
                if (protocolVersion->getValue() != 762) { PLOGW << "Unsupported client"; }
                auto addressLength = VInt::read(socket);
                char *address = new char[addressLength->getValue()];
                address[addressLength->getValue()] = 0;
                recv(socket, address, addressLength->getValue(), 0);
                char *port = new char[2];
                recv(socket, port, 2, 0);
                auto nextState = VInt::read(socket);
                state = nextState->getValue();
            } else if (state == 1) {
                buffer->clear();
                buffer->writeVarInt(0x00);
                buffer->writeString(*serverInfo);
                send(socket, buffer->get(), buffer->getSendBufferSize(), 0);
            } else if (state == 2) {
                static char *nickname = new char[17];
                auto nicknameLength = VInt::read(socket);
                memset(nickname, 0, 17);
                recv(socket, nickname, nicknameLength->getValue(), 0);
                bool hasUUID;
                recv(socket, reinterpret_cast<char *>(&hasUUID), 1, 0);
                static char *uuid = new char[16];
                if (hasUUID) {
                    recv(socket, uuid, 16, 0);
                    static char *uuidString = new char[37];
                    memset(uuidString, 0, 37);
                    uuid_to_string(uuid, uuidString);
                    PLOGI << "UUID of player " << nickname << " is " << uuidString;
                }
                buffer->clear();
                buffer->writeVarInt(0x02);
                buffer->writeArray(uuid, 16);
                buffer->writeString(nickname);
                buffer->writeVarInt(0);
                send(socket, buffer->get(), buffer->getSendBufferSize(), 0);

                buffer->clear();
                buffer->writeVarInt(0x28); // PID
                buffer->writeInt(0); // Is hardcode
                buffer->writeByte(0U); // Gamemode
                buffer->writeByte(-1); // Previous gamemode
                buffer->writeVarInt(3); // Dimension count
                buffer->writeString("minecraft:overworld"); // Dimension names (Array)
                buffer->writeString("minecraft:the_end");
                buffer->writeString("minecraft:the_nether");
                int size = 0;
                const char *data = File::readFile("registry-codec.nbt", &size);
                buffer->writeArray(data, size); // Registry codec
                buffer->writeString("minecraft:overworld"); // Dimension type
                buffer->writeString("minecraft:overworld"); // Dimension name
                buffer->writeLong(1L); // Hashed seed
                buffer->writeVarInt(20); // Max players
                buffer->writeVarInt(8); // View distance
                buffer->writeVarInt(4); // Simulation distance
                buffer->writeBool(false); // Debug info
                buffer->writeBool(true); // Respawn screen
                buffer->writeBool(false); // Is debug
                buffer->writeBool(false); // Is flat
                buffer->writeBool(false); // Has death location
                send(socket, buffer->get(), buffer->getSendBufferSize(), 0);
            }
        } else if (PID->getValue() == 1) {
            static char* number = new char[8];
            recv(socket, number, 8, 0);
            buffer->clear();
            buffer->writeVarInt(0x01);
            buffer->writeArray(number, 8);
            send(socket, buffer->get(), buffer->getSendBufferSize(), 0);
        } else {
            PLOGE << "Unknown packet (" << PID->getValue() << ")";
        }
    }

    connections->erase(std::remove(connections->begin(), connections->end(), this), connections->end());
}
