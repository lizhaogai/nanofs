cmake_minimum_required(VERSION 3.15)
project(nanofs C)

set(CMAKE_C_STANDARD 99)

add_library(nanofs src/nanofs.h src/nanofs.c)

add_executable(example examples/example.c)
add_executable(test examples/test.c)