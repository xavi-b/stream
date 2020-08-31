set(MODULE_NAME client)

message("Compiling \"${MODULE_NAME}\"")

add_executable(${MODULE_NAME} "")

# dependencies
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(spdlog REQUIRED)

target_compile_definitions(${MODULE_NAME}
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD=1
)

target_include_directories(${MODULE_NAME}
    PUBLIC
        ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${MODULE_NAME}
    PUBLIC
        ${Boost_LIBRARIES}
        ${OPENGL_LIBRARIES}
        spdlog::spdlog
        glfw
        glad
        imgui
)

# sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(${MODULE_NAME}
    PUBLIC
        external
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)