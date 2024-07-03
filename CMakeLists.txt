cmake_minimum_required(VERSION 3.25)
project(uring_http)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE_FILES src/*.cpp)
add_executable(uring_http ${SOURCE_FILES})

target_include_directories(uring_http PRIVATE include)
