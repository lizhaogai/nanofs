#include <stdio.h>
#include <stdint.h>
#include "nanofs/nanofs.h"
#include <hunt_fixture.h>
#include "header.h"

int op_times() {

    uint16_t first_len = 300;
    uint8_t first_content[300];
    randomData(&first_content, first_len);
    const uint16_t filename_len = 20;
    const uint8_t filename[20];
    randomData(filename, filename_len);
    nano_fs_write(filename, filename_len, first_content, first_len);
    nano_fs_file_info_t fileInfo = {};

    int ret = nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(1, fileInfo.stat_times);
    TEST_ASSERT_EQUAL_UINT8(1, fileInfo.write_times);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(2, fileInfo.stat_times);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(3, fileInfo.stat_times);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(4, fileInfo.stat_times);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(5, fileInfo.stat_times);

    uint8_t content[300];
    nano_fs_read(filename, filename_len, content);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(1, fileInfo.read_times);
    TEST_ASSERT_EQUAL_UINT8(6, fileInfo.stat_times);
    nano_fs_read(filename, filename_len, content);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(2, fileInfo.read_times);
    TEST_ASSERT_EQUAL_UINT8(7, fileInfo.stat_times);
    nano_fs_read(filename, filename_len, content);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(3, fileInfo.read_times);
    TEST_ASSERT_EQUAL_UINT8(8, fileInfo.stat_times);
    nano_fs_read(filename, filename_len, content);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(4, fileInfo.read_times);
    TEST_ASSERT_EQUAL_UINT8(9, fileInfo.stat_times);
    nano_fs_read(filename, filename_len, content);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(5, fileInfo.read_times);
    TEST_ASSERT_EQUAL_UINT8(10, fileInfo.stat_times);
    nano_fs_read(filename, filename_len, content);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(6, fileInfo.read_times);
    TEST_ASSERT_EQUAL_UINT8(11, fileInfo.stat_times);

    uint16_t second_len = 2000;
    uint8_t second_content[2000];
    randomData(&second_content, second_len);
    nano_fs_write(filename, filename_len, second_content, second_len);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(2, fileInfo.write_times);
    TEST_ASSERT_EQUAL_UINT8(12, fileInfo.stat_times);
    nano_fs_write(filename, filename_len, second_content, second_len);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(3, fileInfo.write_times);
    TEST_ASSERT_EQUAL_UINT8(13, fileInfo.stat_times);
    nano_fs_write(filename, filename_len, second_content, second_len);
    nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL_UINT8(4, fileInfo.write_times);
    TEST_ASSERT_EQUAL_UINT8(14, fileInfo.stat_times);
}

TEST_GROUP(test_op_times);

TEST_SETUP(test_op_times) {
    fake_storage_init();
    nano_fs_init(NULL, 122 * 4 * 1024, 4 * 1024, 6, 0);
}

TEST_TEAR_DOWN(test_op_times) {
    nano_fs_destroy();
}

TEST(test_op_times, op_times) {
    op_times();
}


TEST_GROUP_RUNNER(test_op_times) {
    RUN_TEST_CASE(test_op_times, op_times);
}