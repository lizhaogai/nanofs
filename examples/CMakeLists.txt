 add_executable(example example.c)
 add_executable(test test.c)

 target_link_libraries(example PUBLIC nanofs)