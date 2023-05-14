#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>
#include "plog/Log.h"
#include <Windows.h>
#include "zlib.h"

#define ASSERT(text, expression) assert(((void) text, expression))

class File {
public:
    static char *readFile(const char *path, int *size = nullptr);

    static char *readResourceFile(const char *path, int *size = nullptr);

    static bool writeFile(const char *path, const char *data, unsigned int size);

    static char *readString(const char *path);

    static char *readResourceString(const char *path);

    static bool writeString(const char *path, const char *data);

    static bool exists(const char *path);

    static bool resourceExists(const char *path);

    static unsigned char *compress(unsigned char *data, unsigned int length, unsigned long *compressedLength);

    static unsigned char *decompress(unsigned char *data, unsigned int length, unsigned long *decompressedLength);
};