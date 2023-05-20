cmake_minimum_required(VERSION 3.24)

project(utils VERSION 0.1.0)


set(UTILS_INCLUDE_DIRS include)
set(UTILS_SOURCE_DIRS src)
file(GLOB_RECURSE UTILS_SRC_FILES CONFIGURE_DEPENDS ${UTILS_SOURCE_DIRS}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${UTILS_SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${UTILS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
    cppjieba
    jsoncpp
    spdlog
    indicators
)