cmake_minimum_required(VERSION 3.15)
project(filesystem C)

set(CMAKE_C_STANDARD 99)

add_executable(filesystem main.c map.h map.c utils.h utils.c file_system.h file_system.c help.h help.c)