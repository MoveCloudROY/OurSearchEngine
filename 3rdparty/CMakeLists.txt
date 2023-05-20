cmake_minimum_required(VERSION 3.24)


set(JIEBA_LIB_NAME "cppjieba")
set(JIEBA_SRC_PATH "${JIEBA_LIB_NAME}")
set(JIEBA_INC_PATH "${JIEBA_LIB_NAME}/include")
set(JIEBA_DEP_INC_PATH "${JIEBA_LIB_NAME}/deps/limonp/include")
add_library(${JIEBA_LIB_NAME} INTERFACE)
target_include_directories(${JIEBA_LIB_NAME} INTERFACE ${JIEBA_INC_PATH} ${JIEBA_DEP_INC_PATH})

set(SPDLOG_LIB_NAME "spdlog")
set(SPDLOG_SRC_PATH "${SPDLOG_LIB_NAME}")
set(SPDLOG_INC_PATH "${SPDLOG_LIB_NAME}/include")
add_subdirectory(${SPDLOG_LIB_NAME})


set(DOCTEST_LIB_NAME "doctest")
add_subdirectory(${DOCTEST_LIB_NAME})


set(INDICATORS_LIB_NAME "indicators")
add_subdirectory(${INDICATORS_LIB_NAME})

find_package(jsoncpp REQUIRED)
get_target_property(JSONCPP_INCLUDE_DIRS jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${JSONCPP_INCLUDE_DIRS})
message("-- JSONCPP_INCLUDE_DIRS: ${JSONCPP_INCLUDE_DIRS}")

if (WIN32)
set(CMAKE_SYSTEM_NAME Windows)
elseif(UNIX)
set(CMAKE_SYSTEM_NAME Linux)
else()
set(CMAKE_SYSTEM_NAME Generic)
endif()
message("-- CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
set(CMAKE_CROSSCOMPILING TRUE)
message("-- CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")
set(DROGON_LIB_NAME "drogon")
set(BUILD_TESTING OFF)
add_subdirectory(${DROGON_LIB_NAME})