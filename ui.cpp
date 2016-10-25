#include <iostream>
#include <list>

#include <string>
#include <cstdio>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include "ui.hpp"
#include "colors.hpp"

namespace ui
{

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

// Load a better font and adjust its scale for high-DPI monitors
static void load_font(GLFWwindow* window)
{
    int window_width, window_height;
    int framebuffer_width, framebuffer_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

    const float scale = framebuffer_width / float(window_width);
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("../imgui/Inconsolata.ttf", scale * 13.0f);
    io.Fonts->Fonts[0]->Scale /= scale;
}

static GLFWwindow* make_window()
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);

    return window;
}

static void set_style()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Colors::base01);

    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, Colors::base01);
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, Colors::base02);
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, Colors::base03);
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, Colors::base03);

    ImGui::PushStyleColor(ImGuiCol_Button, Colors::base02);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Colors::base03);
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, Colors::blue);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::base03);
    ImGui::PushStyleColor(ImGuiCol_Text, Colors::base06);
}

GLFWwindow* init()
{
    auto window = make_window();
    ImGui_ImplGlfwGL3_Init(window, true);
    load_font(window);
    set_style();

    return window;
}

void shutdown()
{
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}

bool finished(GLFWwindow* w)
{
    return glfwWindowShouldClose(w);
}

void start()
{
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();
}

void finish(GLFWwindow* window)
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    const auto& clear_color = Colors::base00;
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Render();
    glfwSwapBuffers(window);
}

} // namespace ui
