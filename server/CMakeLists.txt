set(MODULE_NAME server)

message("Compiling \"${MODULE_NAME}\"")

add_executable(${MODULE_NAME} "")

# dependencies
find_package(Boost COMPONENTS system REQUIRED)
find_package(spdlog REQUIRED)

target_include_directories(${MODULE_NAME}
    PUBLIC
        ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${MODULE_NAME}
    PUBLIC
        ${Boost_LIBRARIES}
        spdlog::spdlog
)

# sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(${MODULE_NAME}
    PUBLIC
        external
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)