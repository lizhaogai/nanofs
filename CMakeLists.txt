cmake_minimum_required(VERSION 3.15)
project(nanofs C)

set(CMAKE_C_STANDARD 99)

set(TARGET_NANO_FS_GROUP prod CACHE STRING "Group to build")

if (ZEPHYR_BASE)
    include(CMakeListsZephyr.txt)
else ()
    add_library(nanofs STATIC
            src/nanofs.c
            )
    target_include_directories(nanofs PUBLIC include)
    if (TARGET_NANO_FS_GROUP STREQUAL test)
        add_subdirectory(testlib)
        add_subdirectory(examples)
    endif ()
endif ()