cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME stream)

project(${PROJECT_NAME})

# C++
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)
set(CMAKE_CXX_COMPILER              "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS                 "-Wall -Wextra -Wpedantic -std=c++11 -pthread")

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
endif()

# subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/client)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/server)