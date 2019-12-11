 add_executable(example normal.c)
 add_executable(test a.c)

 target_link_libraries(example PUBLIC nanofs hunt)