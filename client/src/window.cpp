#include "window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <exception>

namespace ST::UI
{

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "Glfw Error " << error << ": " << description << "\n";
}

Window::Window()
{
    // Setup window
    glfwSetErrorCallback(&ST::UI::glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("Failed to init a GLFW");

        // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    window_ = glfwCreateWindow(1280, 720, "Stream Client", NULL, NULL);
    if (window_ == NULL)
        throw std::runtime_error("Failed to create a GLFWwindow");

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (gladLoadGL() == 0)
        throw std::runtime_error("Failed to initialize OpenGL loader");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

Window::~Window()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_);
    glfwTerminate();
}

int Window::exec()
{
    while (!glfwWindowShouldClose(window_))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render();

        // Rendering
        ImGui::Render();
        renderBackground();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
    }

    onClose();

    return 0;
}

void Window::renderBackground()
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int    display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace ST::UI