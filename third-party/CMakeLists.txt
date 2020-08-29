set(MODULE_NAME third-party)

message("Compiling \"${MODULE_NAME}\"")

# dependencies
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_LIB_NAME "glfw")
set(GLFW_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${GLFW_LIB_NAME}/include")

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(GLFW_VULKAN_STATIC  OFF CACHE BOOL "" FORCE)    # OFF by default
#set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)    # OFF by default
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

add_subdirectory(${GLFW_LIB_NAME})

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

target_include_directories(${IMGUI_LIB_NAME}
    PUBLIC
        "${IMGUI_INC_PATH}"
        "${GLFW_INC_PATH}"
        "${GLAD_INC_PATH}"
)
