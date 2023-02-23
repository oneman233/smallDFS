cmake_minimum_required(VERSION 3.23)
project(smallDFS)

set(CMAKE_CXX_STANDARD 17)

add_executable(smallDFS main.cpp utils.h utils.cpp filetree.h filetree.cpp dataserver.h dataserver.cpp nameserver.h nameserver.cpp threadpool.h)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")