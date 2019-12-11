#include <stdio.h>
#include <stdint.h>
#include "nanofs/nanofs.h"
#include <hunt_fixture.h>
#include "header.h"

int test_force_erase() {
    uint16_t first_len = 300;
    uint8_t first_content[300];
    randomData(&first_content, first_len);

    const uint16_t filename_len = 20;
    const uint8_t filename[20];
    randomData(filename, filename_len);
    nano_fs_write(filename, filename_len, first_content, first_len);
    nano_fs_file_info_t fileInfo = {};
    int ret = nano_fs_stat(filename, filename_len, &fileInfo);
    const uint8_t first_read[300];
    nano_fs_read(filename, filename_len, first_read);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(filename_len, fileInfo.filename_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(filename, fileInfo.filename, fileInfo.filename_len));
    TEST_ASSERT_EQUAL_UINT8(first_len, fileInfo.content_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(first_content, first_read, fileInfo.content_len));
    TEST_ASSERT_EQUAL_UINT8(NANO_FS_CONTENT_OFFSET, fileInfo.content_offset);

    uint16_t second_len = 2000;
    uint8_t second_content[2000];
    randomData(&second_content, second_len);

    nano_fs_write(filename, filename_len, second_content, second_len);
    ret = nano_fs_stat(filename, filename_len, &fileInfo);

    const uint8_t second_read[2000];
    nano_fs_read(filename, filename_len, second_read);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(filename_len, fileInfo.filename_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(filename, fileInfo.filename, fileInfo.filename_len));
    TEST_ASSERT_EQUAL_UINT8(second_len, fileInfo.content_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(second_content, second_read, fileInfo.content_len));
    TEST_ASSERT_EQUAL_UINT8(NANO_FS_CONTENT_OFFSET + first_len + 4, fileInfo.content_offset);

    nano_fs_write(filename, filename_len, second_content, second_len);
    ret = nano_fs_stat(filename, filename_len, &fileInfo);

    nano_fs_read(filename, filename_len, second_read);

    TEST_ASSERT_EQUAL_UINT8(0, ret);
    TEST_ASSERT_EQUAL_UINT8(filename_len, fileInfo.filename_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(filename, fileInfo.filename, fileInfo.filename_len));
    TEST_ASSERT_EQUAL_UINT8(second_len, fileInfo.content_len);
    TEST_ASSERT_EQUAL_UINT8(0, nano_fs_array_cmp(second_content, second_read, fileInfo.content_len));
    TEST_ASSERT_EQUAL_UINT8(NANO_FS_CONTENT_OFFSET, fileInfo.content_offset);

    printf("Test force erase before write passed.");
}

TEST_GROUP(test_erase);

TEST_SETUP(test_erase) {
    fake_storage_init();
    nano_fs_init(NULL, 122 * 4 * 1024, 4 * 1024, 6, 1);
}

TEST_TEAR_DOWN(test_erase) {
    nano_fs_destroy();
}

TEST(test_erase, test_force_erase) {
    test_force_erase();
}


TEST_GROUP_RUNNER(test_erase) {
    RUN_TEST_CASE(test_erase, test_force_erase);
}