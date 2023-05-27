cmake_minimum_required(VERSION 3.24)
project(OurSearchEngine)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin )

add_subdirectory(3rdparty)
add_subdirectory(src)
add_subdirectory(test)