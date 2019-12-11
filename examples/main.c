#include "header.h"

#include <hunt_fixture.h>

#define ARGC 2
const char *ARGV[2] = {"test_main", "-v"};

void run_tests() {
    RUN_TEST_GROUP(test_normal);
    RUN_TEST_GROUP(test_erase);
    RUN_TEST_GROUP(test_not_erase);
    RUN_TEST_GROUP(test_op_times);
}


void main() {
    HuntMain(ARGC, ARGV, run_tests);
}