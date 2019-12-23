cmake_minimum_required(VERSION 3.15)
project(nanofs C)

set(CMAKE_C_STANDARD 99)

if (ZEPHYR_BASE)
    include(CMakeListsZephyr.txt)
else ()
    set(TARGET_NANO_FS_GROUP test CACHE STRING "Group to build")
    add_library(nanofs STATIC
            src/nanofs.c
            )
    target_include_directories(nanofs PUBLIC include)
    if (TARGET_NANO_FS_GROUP STREQUAL test)
        add_subdirectory(testlib)
        add_subdirectory(examples)
    endif ()
endif ()