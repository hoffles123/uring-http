cmake_minimum_required(VERSION 3.25)
project(uring_http)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE_FILES src/*.cpp)
add_executable(uring_http ${SOURCE_FILES}
        main.cpp)

target_include_directories(uring_http PRIVATE include)
target_compile_options(uring_http PRIVATE -Wall -Wextra)

set(LIBURING_PATH "~/usr/lib")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(uring_http PRIVATE asan ubsan uring)
    target_compile_options(uring_http PRIVATE -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined)
else ()
    target_link_libraries(uring_http PRIVATE uring)
endif ()

