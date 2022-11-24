cmake_minimum_required(VERSION 3.22)

project(sm-ext-JSON)

include(${PROJECT_SOURCE_DIR}/configure.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/All.cmake)

find_package(fmt REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

message("${CMAKE_BUILD_TYPE}")

add_subdirectory(src)