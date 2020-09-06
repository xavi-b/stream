set(MODULE_NAME third-party)

message("Compiling \"${MODULE_NAME}\"")

# dependencies
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# Glad
set(GLAD_LIB_NAME "glad")
set(GLAD_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${GLAD_LIB_NAME}/src")
set(GLAD_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${GLAD_LIB_NAME}/include")

add_library(${GLAD_LIB_NAME}
    SHARED
        "${GLAD_SRC_PATH}/glad.c"
)

target_include_directories(${GLAD_LIB_NAME}
    PUBLIC
        "${GLAD_INC_PATH}"
)

# ImGui
set(IMGUI_LIB_NAME "imgui")

set(IMGUI_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${IMGUI_LIB_NAME}/")

set(IMGUI_SOURCES
    "${IMGUI_INC_PATH}/imgui.cpp"
    "${IMGUI_INC_PATH}/imgui_demo.cpp"
    "${IMGUI_INC_PATH}/imgui_draw.cpp"
    "${IMGUI_INC_PATH}/imgui_widgets.cpp"
    "${IMGUI_INC_PATH}/examples/imgui_impl_glfw.cpp"
    "${IMGUI_INC_PATH}/examples/imgui_impl_opengl3.cpp"
)

set(IMGUI_HEADERS
    "${IMGUI_INC_PATH}/imconfig.h"
    "${IMGUI_INC_PATH}/imgui.h"
    "${IMGUI_INC_PATH}/imgui_internal.h"
    "${IMGUI_INC_PATH}/imstb_rectpack.h"
    "${IMGUI_INC_PATH}/imstb_textedit.h"
    "${IMGUI_INC_PATH}/imstb_truetype.h"
    "${IMGUI_INC_PATH}/examples/imgui_impl_glfw.h"
    "${IMGUI_INC_PATH}/examples/imgui_impl_opengl3.h"
)

add_library(${IMGUI_LIB_NAME}
    SHARED
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
)

target_compile_definitions(${IMGUI_LIB_NAME}
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD=1
)

target_link_libraries(${IMGUI_LIB_NAME}
    PUBLIC
        ${GLFW_LIBRARIES}
)

target_include_directories(${IMGUI_LIB_NAME}
    PUBLIC
        "${IMGUI_INC_PATH}"
        "${GLFW_INCLUDE_DIRS}"
        "${GLAD_INC_PATH}"
)

# openh264
find_program(NASM_EXECUTABLE nasm REQUIRED)

set(OPENH264_LIB_NAME openh264)
set(OPENH264_LIB_FILE "${PROJECT_SOURCE_DIR}/third-party/${OPENH264_LIB_NAME}/${CMAKE_SHARED_LIBRARY_PREFIX}${OPENH264_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")

add_custom_command(
    OUTPUT ${OPENH264_LIB_FILE}
    COMMAND "make"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${OPENH264_LIB_NAME}"
    COMMENT "Generate ${OPENH264_LIB_NAME}"
)

add_custom_target(${OPENH264_LIB_NAME} ALL
    DEPENDS ${OPENH264_LIB_FILE}
)