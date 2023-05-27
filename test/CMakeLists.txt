cmake_minimum_required(VERSION 3.24)
project(test VERSION 0.0.1 LANGUAGES CXX)

add_executable(test_server test_server.cpp)
target_link_libraries(test_server PUBLIC
    backends
)

add_executable(test_pre test_pre.cpp)
target_link_libraries(test_pre PUBLIC
    preprocessing
)

add_executable(test_jsonbuilder test_jsonbuilder.cpp)
target_link_libraries(test_jsonbuilder PUBLIC
    query
)

add_executable(test_skiplist test_skiplist.cpp)
target_link_libraries(test_skiplist PUBLIC
    utils
)


add_executable(test_fst test_fst.cpp)
target_link_libraries(test_fst PUBLIC
    utils
)

add_executable(test_queryer test_queryer.cpp)
target_link_libraries(test_queryer PUBLIC
    query
)

if(UNIX AND CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(test_skiplist PRIVATE -fno-omit-frame-pointer)
    target_compile_options(test_skiplist PRIVATE -fsanitize=address)
    target_link_options(test_skiplist PRIVATE -fsanitize=address)

    target_compile_options(test_pre PRIVATE -fno-omit-frame-pointer)
    target_compile_options(test_pre PRIVATE -fsanitize=address)
    target_link_options(test_pre PRIVATE -fsanitize=address)

    target_compile_options(test_server PRIVATE -fno-omit-frame-pointer)
    target_compile_options(test_server PRIVATE -fsanitize=address)
    target_link_options(test_server PRIVATE -fsanitize=address)
endif()