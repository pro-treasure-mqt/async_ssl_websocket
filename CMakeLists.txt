cmake_minimum_required(VERSION 2.8)

project(async_ssl_websocket)

set(CMAKE_BUILD_TYPE "Release")

include_directories(/usr/local/include ${CMAKE_SOURCE_DIR}/include)
link_directories(/usr/local/lib ${CMAKE_SOURCE_DIR}/lib)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath=./lib/ -std=c++11 -g -Wall -Wno-unused")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(src/core)
add_subdirectory(src/example)

file(GLOB header_files "${CMAKE_SOURCE_DIR}/src/core/*.h")
file(COPY ${header_files} DESTINATION  "${CMAKE_SOURCE_DIR}/include/")


