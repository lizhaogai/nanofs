file(
        GLOB
        fs_test_sources # This is an output parameter
        test_normal.c
        utils.c
        fake-storage.c
        test_nerase.c
        test_earse.c
        test_op_times.c
)

target_include_directories(nanofs PUBLIC ./header.h)
add_executable(example ${fs_test_sources} main.c)
target_link_libraries(example PUBLIC nanofs hunt)