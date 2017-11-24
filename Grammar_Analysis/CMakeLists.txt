cmake_minimum_required(VERSION 3.8)
project(Grammar_Analysis)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        LA.h LA.cpp GA.h GA.cpp main.cpp)

add_executable(Grammar_Analysis ${SOURCE_FILES})