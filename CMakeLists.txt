cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME stream)

project(${PROJECT_NAME})

# C++
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_COMPILER              "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS                 "-Wall")

add_executable(${PROJECT_NAME} "")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)