cmake_minimum_required(VERSION 3.17)
project(usoSQL C)

set(CMAKE_C_STANDARD 11)

include_directories(src/pager)

add_executable(usoSQL
        src/pager/pager.c src/pager/pager.h
        src/main.c
        src/queue/queue.h src/queue/queue.c)
