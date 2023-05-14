#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

const int SEGMENT_BITS = 0x7F;
const int CONTINUE_BIT = 0x80;

class VInt {
    int value;
public:
    explicit VInt(int val);

    static VInt *read(SOCKET socket);

    static VInt *read(const char *buffer);

    int write(char *buffer) const;

    int getSize() const;

    int getValue() const;

    void setValue(int val);
};
