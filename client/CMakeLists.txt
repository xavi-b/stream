set(MODULE_NAME client)

message("Compiling \"${MODULE_NAME}\"")

add_executable(${MODULE_NAME} "")

# dependencies
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(Boost COMPONENTS system REQUIRED)
find_package(spdlog REQUIRED)
find_library(VLC_LIBRARY vlc)
find_package(X11 REQUIRED)

target_compile_definitions(${MODULE_NAME}
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD=1
)

target_include_directories(${MODULE_NAME}
    PUBLIC
        ${OPENGL_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/third-party/glad/include
        ${PROJECT_SOURCE_DIR}/third-party/imgui
        ${PROJECT_SOURCE_DIR}/third-party/stb
        ${PROJECT_SOURCE_DIR}/third-party/screen_capture_lite/include
)

target_link_libraries(${MODULE_NAME}
    PUBLIC
        ${Boost_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        spdlog::spdlog
        ${VLC_LIBRARY}
        ${X11_LIBRARIES}
        ${X11_Xfixes_LIB}
        ${X11_Xinerama_LIB}
        glad
        imgui
        screen_capture_lite
)

message("test: ${screen_capture_lite_PLATFORM_LIBS}")

include(${PROJECT_SOURCE_DIR}/third-party/cmrc/CMakeRC.cmake)

set(OPENH264_LIB_NAME openh264)
set(OPENH264_LIB_FILE "${PROJECT_SOURCE_DIR}/third-party/${OPENH264_LIB_NAME}/${CMAKE_SHARED_LIBRARY_PREFIX}${OPENH264_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
target_include_directories(${MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/third-party/${OPENH264_LIB_NAME})
target_link_libraries(${MODULE_NAME} PUBLIC ${OPENH264_LIB_FILE})

# sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(${MODULE_NAME}
    PUBLIC
        external
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# resources
cmrc_add_resource_library(${MODULE_NAME}-resources NAMESPACE ST::RC "${CMAKE_CURRENT_SOURCE_DIR}/logo.png")
target_link_libraries(${MODULE_NAME} PRIVATE ${MODULE_NAME}-resources)
