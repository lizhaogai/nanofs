cmake_minimum_required(VERSION 3.15)
project(nanofs C)

set(CMAKE_C_STANDARD 99)

add_library(nanofs src/nano_fs.h src/nano_fs.c)

add_executable(example examples/example.c)