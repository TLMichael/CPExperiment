cmake_minimum_required(VERSION 2.6)
project(Lexical_Analysis)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES LA.cpp LA.h)
add_executable(Lexical_Analysis ${SOURCE_FILES})

set (EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
