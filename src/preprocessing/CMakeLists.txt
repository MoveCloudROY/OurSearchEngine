cmake_minimum_required(VERSION 3.24)

project(preprocessing VERSION 0.1.0)


set(PREPROCESS_INCLUDE_DIRS include)
set(PREPROCESS_SOURCE_DIRS src)
file(GLOB_RECURSE PREPROCESS_SRC_FILES CONFIGURE_DEPENDS ${PREPROCESS_SOURCE_DIRS}/*.cpp)

add_library(${PROJECT_NAME} ${PREPROCESS_SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PREPROCESS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
    cppjieba
    spdlog
    indicators
)

