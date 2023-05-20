cmake_minimum_required(VERSION 3.24)

project(backends VERSION 0.1.0)


set(BACKENDS_INCLUDE_DIRS include)
set(BACKENDS_SOURCE_DIRS src)
file(GLOB_RECURSE BACKENDS_SRC_FILES CONFIGURE_DEPENDS ${BACKENDS_SOURCE_DIRS}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${BACKENDS_SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${BACKENDS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog
    indicators
    drogon
)