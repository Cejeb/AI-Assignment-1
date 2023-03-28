# raylib-tileson-test
add_executable(raylib-tileson-test raylib-tileson-test.cpp)
if (UNIX)
target_compile_options(raylib-tileson-test PRIVATE -Wall -Wextra -Wconversion -Wsign-conversion)
endif()
target_link_libraries(raylib-tileson-test PUBLIC
    raylib
    raylib-tileson
)

target_compile_definitions(raylib-tileson-test PRIVATE -DTILESON_ROOT_DIR="${CMAKE_SOURCE_DIR}")

if (UNIX)
    target_link_libraries(raylib-tileson-test PUBLIC m dl pthread)

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_options(raylib-tileson-test PRIVATE -fsanitize=address)
        target_link_options(raylib-tileson-test PRIVATE -fsanitize=undefined)
    endif()
endif()

# Set up the test
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_test(NAME raylib-tileson-test COMMAND raylib-tileson-test)
