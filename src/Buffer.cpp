#include "Buffer.h"

Buffer::Buffer(int length) {
    buffer = new char[length];
    clear();
}

void Buffer::clear() {
    index = 5;
}

char *Buffer::get() {
    return buffer;
}

int Buffer::getIndex() {
    return index;
}

void Buffer::writeString(const char *v) {
    writeVarInt((int) strlen(v));
    writeArray(v, (int) strlen(v));
}

void Buffer::writeArray(const char *v, int length) {
    memcpy(buffer + index, v, length);
    index += length;
}

void Buffer::writeVarInt(int v) {
    writeVarInt(VInt(v));
}

void Buffer::writeVarInt(VInt v) {
    index += v.write(buffer + index);
}

void Buffer::writeLongLong(long long v) {
    memcpy(buffer + index, &v, sizeof(v));
    index += sizeof(v);
}

int Buffer::getSendBufferSize() {
    VInt dataLength(index - 5); //Real written data length
    index -= 5;                        // } Fit buffer
    index += dataLength.write(buffer); // }
    memcpy(buffer + dataLength.getSize(), buffer + 5, dataLength.getValue());
    return index;
}

void Buffer::writeInt(int v) {
    memcpy(buffer + index, &v, sizeof(v));
    index += sizeof(v);
}

void Buffer::writeBool(bool v) {
    buffer[index] = v ? 1 : 0;
    index++;
}

void Buffer::writeByte(char v) {
    buffer[index] = v;
    index++;
}

void Buffer::writeLong(long v) {
    memcpy(buffer + index, &v, sizeof(v));
    index += sizeof(v);
}
