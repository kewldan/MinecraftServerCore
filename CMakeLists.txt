cmake_minimum_required(VERSION 3.25)
project(MinecraftCore)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARIES_PATH "C:/libraries")

include_directories(
        "${LIBRARIES_PATH}/plog/include"
        "${LIBRARIES_PATH}/bson"
        include
)

file(GLOB SRCS
        src/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${SRCS} main.cpp)

target_link_libraries(${PROJECT_NAME} Ws2_32)