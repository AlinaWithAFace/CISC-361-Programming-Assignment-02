cmake_minimum_required(VERSION 3.12)
project(CISC_361_Programming_Assignment_02 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(CISC_361_Programming_Assignment_02
        main.c
        sh.c
        sh.h
        get_path.c
        get_path.h)
