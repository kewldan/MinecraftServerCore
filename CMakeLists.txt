cmake_minimum_required(VERSION 3.25)
project(MinecraftCore)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARIES_PATH "E:/Projects/libraries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
        "${LIBRARIES_PATH}/plog/include"
        "${LIBRARIES_PATH}/zlib"
        "${LIBRARIES_PATH}/nbt"
        "${LIBRARIES_PATH}/bson"
        "${LIBRARIES_PATH}/b64"
        "${LIBRARIES_PATH}/engine/include"
        "${LIBRARIES_PATH}/sockets"
        include
)

link_directories("${LIBRARIES_PATH}/zlib/lib")

file(GLOB SRCS
        src/*.cpp include/*.h
        )

file(GLOB ENGINE_SRCS
        ${LIBRARIES_PATH}/engine/src/network/*.cpp
        ${LIBRARIES_PATH}/engine/include/network/*.h

        ${LIBRARIES_PATH}/engine/src/io/*.cpp
        ${LIBRARIES_PATH}/engine/include/io/*.h
        )

add_executable(${PROJECT_NAME} ${ENGINE_SRCS} ${SRCS} main.cpp "${LIBRARIES_PATH}/b64/Base64.cpp")

target_link_libraries(${PROJECT_NAME}
        debug zlibstaticd
        optimized zlibstatic)

target_link_libraries(${PROJECT_NAME} Ws2_32)