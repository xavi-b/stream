cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME stream)

project(${PROJECT_NAME})

# C++
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)
set(CMAKE_CXX_FLAGS                 "-Wall -Wextra -Wpedantic -pthread")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
    add_definitions(-DDEBUG)
else()
    message("RELEASE")
endif()

# subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/client)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/server)