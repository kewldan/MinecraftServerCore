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
        include
)

link_directories("${LIBRARIES_PATH}/zlib/lib")

file(GLOB SRCS
        src/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${SRCS} main.cpp "${LIBRARIES_PATH}/b64/Base64.cpp")

target_link_libraries(${PROJECT_NAME}
        debug zlibstaticd
        optimized zlibstatic)

target_link_libraries(${PROJECT_NAME} Ws2_32)