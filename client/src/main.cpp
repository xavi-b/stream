#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

enum class ConnectionState
{
    NotConnected = 0,
    Connecting   = 1,
    Connected    = 2
};

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Stream Client", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (gladLoadGL() == 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool            show_demo_window = true;
    ConnectionState connectionState  = ConnectionState::NotConnected;
    ImVec4          clear_color      = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);

        if (connectionState == ConnectionState::NotConnected)
        {
            float height = 80;
            float width  = 200;
            ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - width) / 2, (io.DisplaySize.y - height) / 2), 0);
            ImGui::SetNextWindowSize(ImVec2(width, height), 0);
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoNav;

            ImGui::Begin("Connection", NULL, window_flags);

            // TODO

            ImGui::End();
        }

        if (connectionState == ConnectionState::Connected)
        {
            float height = 200;
            ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - height), 0);
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, height), 0);
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoNav;

            ImGui::Begin("Streams", NULL, window_flags);

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
            ImVec2 scrolling_child_size = ImGui::GetContentRegionAvail();
            ImGui::BeginChild("scrolling", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar);
            for (int line = 0; line < 6; line++)
            {
                // Display random stuff. For the sake of this trivial demo we are using basic Button() + SameLine()
                // If you want to create your own time line for a real application you may be better off manipulating
                // the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets
                // yourself. You may also want to use the lower-level ImDrawList API.
                int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
                for (int n = 0; n < num_buttons; n++)
                {
                    if (n > 0)
                        ImGui::SameLine();
                    ImGui::PushID(n + line * 1000);
                    char num_buf[16];
                    sprintf(num_buf, "%d", n);
                    const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;
                    float       hue   = n * 0.05f;
                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                    ImGui::Button(label, ImVec2(40.0f, 0.0f));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleVar(2);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}