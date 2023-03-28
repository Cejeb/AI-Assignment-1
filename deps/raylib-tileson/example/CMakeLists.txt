find_package(raylib CONFIG REQUIRED)

# raylib-tileson-example
add_executable(raylib-tileson-example raylib-tileson-example.c)
target_link_libraries(raylib-tileson-example PUBLIC
    raylib
    raylib-tileson
)

target_compile_definitions(raylib-tileson-example PRIVATE -DTILESON_ROOT_DIR="${CMAKE_SOURCE_DIR}")

if (UNIX)
    target_link_libraries(raylib-tileson-example PUBLIC m dl pthread)

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_options(raylib-tileson-example PRIVATE -fsanitize=address)
        target_link_options(raylib-tileson-example PRIVATE -fsanitize=undefined)
    endif()
endif()

# This fails at runtime on Linux with a debug build due to an allocation bug
# spotted by ASan. To avoid the message, set the environment via:
# export ASAN_OPTIONS=new_delete_type_mismatch=0
