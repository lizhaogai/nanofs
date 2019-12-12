add_library(hunt STATIC
    hunt/src/hunt.c
    hunt/src/hunt_fixture.c
    )

target_include_directories(hunt PUBLIC
    hunt/src
    )