cmake_minimum_required (VERSION 2.6)
project (common_lib)

file(GLOB_RECURSE   ${PROJECT_NAME}_SOURCE
            ${PROJECT_SOURCE_DIR}/inc/*.h
            ${PROJECT_SOURCE_DIR}/src/*.cpp)

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCE})
