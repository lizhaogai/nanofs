#include <stdio.h>
#include <stdint.h>
#include "nanofs/nanofs.h"
#include <hunt_fixture.h>
#include "header.h"

int pain() {
    uint16_t first_len = 300;
    uint8_t first_content[300];
    randomData(&first_content, first_len);

    const uint16_t filename_len = 20;
    const uint8_t filename[20];
    randomData(filename, filename_len);

    nano_fs_write(filename, filename_len, first_content, first_len);
    nano_fs_file_info_t fileInfo = {};

    int ret = nano_fs_stat(filename, filename_len, &fileInfo);

    uint8_t second_content[300];
    nano_fs_read(filename, filename_len, second_content);

    nano_fs_stat(filename, filename_len, &fileInfo);
    nano_fs_read(filename, filename_len, second_content);
    nano_fs_read(filename, filename_len, second_content);
    nano_fs_stat(filename, filename_len, &fileInfo);
    nano_fs_stat(filename, filename_len, &fileInfo);
    nano_fs_stat(filename, filename_len, &fileInfo);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(filename_len, fileInfo.filename_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(filename, fileInfo.filename, fileInfo.filename_len));
    TEST_ASSERT_EQUAL_UINT8(first_len, fileInfo.content_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(first_content, second_content, fileInfo.content_len));

    uint16_t third_len = 2900;
    uint8_t third_content[2900];
    uint8_t third_content_[2900];
    randomData(third_content, third_len);
    nano_fs_write(filename, filename_len, third_content, third_len);
    ret = nano_fs_stat(filename, filename_len, &fileInfo);
    nano_fs_read(filename, filename_len, third_content_);
    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(filename_len, fileInfo.filename_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(filename, fileInfo.filename, fileInfo.filename_len));
    TEST_ASSERT_EQUAL_UINT8(third_len, fileInfo.content_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(third_content, third_content_, fileInfo.content_len));

    nano_fs_delete(filename, filename_len);
    ret = nano_fs_stat(filename, filename_len, &fileInfo);
    TEST_ASSERT_EQUAL(NANO_FS_NOT_FOUND, ret);
    printf("Normal test passed.");
}

TEST_GROUP(test_normal);

TEST_SETUP(test_normal) {
    fake_storage_init();
    nano_fs_init(NULL, 122 * 4 * 1024, 4 * 1024, 6, 0);
}

TEST_TEAR_DOWN(test_normal) {
    nano_fs_destroy();
}

TEST(test_normal, pain) {
    pain();
}


TEST_GROUP_RUNNER(test_normal) {
    RUN_TEST_CASE(test_normal, pain);
}