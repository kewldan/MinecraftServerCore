#pragma once

#include <plog/Log.h>
#include "DataTypes.h"

class Buffer {
private:
    char *buffer;
    int index{};
public:
    explicit Buffer(int length);

    void clear();

    char *get();

    int getIndex();

    int getSendBufferSize();

    void writeString(const char *v);

    void writeArray(const char *v, int length);

    void writeVarInt(int v);

    void writeVarInt(VInt v);

    void writeLong(long v);

    void writeLongLong(long long v);

    void writeInt(int v);

    void writeBool(bool v);

    void writeByte(char v);
};