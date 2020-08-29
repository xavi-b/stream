cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME stream)

project(${PROJECT_NAME})

# C++
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_COMPILER              "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS                 "-Wall -Wextra -Wpedantic -std=c++11")

add_executable(${PROJECT_NAME} "")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party)

find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
    message('OPENGL_FOUND')
endif()

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD=1
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
        "${OPENGL_INCLUDE_DIR}"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PUBLIC
        ${OPENGL_LIBRARIES}
        glfw
        glad
        imgui
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)