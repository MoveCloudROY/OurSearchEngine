cmake_minimum_required(VERSION 3.24)

project(query VERSION 0.1.0)


set(QUERY_INCLUDE_DIRS include)
set(QUERY_SOURCE_DIRS src)
file(GLOB_RECURSE QUERY_SRC_FILES CONFIGURE_DEPENDS ${QUERY_SOURCE_DIRS}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${QUERY_SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${QUERY_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
    utils
    spdlog
    indicators
    jsoncpp
)

