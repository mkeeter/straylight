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
#include "datum.hpp"

namespace ui
{

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

// Load a better font and adjust its scale for high-DPI monitors
static void load_font(Window* window)
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

static Window* make_window()
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
    ImGui::PushStyleColor(ImGuiCol_Button, Colors::base02);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Colors::base03);
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, Colors::blue);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::base03);
    ImGui::PushStyleColor(ImGuiCol_Text, Colors::base06);
}

Window* init()
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

bool finished(Window* w)
{
    return glfwWindowShouldClose(w);
}

static void start()
{
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();
}

static void finish(GLFWwindow* window)
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

void draw(Window* window, std::map<std::string, Datum*>& ds)
{
    start();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::SetNextWindowPos({0, 0});

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    ImGui::SetNextWindowSize({float(width)/4, float(height)});
    ImGui::Begin("Screens", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    // Reserve keys and erased keys in a separate list here to avoid
    // glitches when we iterate over a changing map
    std::list<std::string> keys;
    std::list<std::string> erased;
    for (auto d : ds)
    {
        keys.push_back(d.first);
    }

    for (auto k : keys)
    {
        ImGui::PushID(k.c_str());

        ImGui::Columns(2, "datum", false);
        ImGui::SetColumnOffset(1, 60.0f);
        ImGui::Text("Name");
        ImGui::NextColumn();

        {   // Temporary buffer for editing a cell's name
            char buf[128];
            if (ImGui::Button(k.c_str(), {-1.0f, 0.0f}))
            {
                std::copy(k.begin(), k.end(), buf);
                ImGui::OpenPopup("rename");
            }
            if (ImGui::BeginPopup("rename"))
            {
                const bool ret = ImGui::InputText("##rename", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::SameLine();
                if (ds[k]->canRenameTo(buf))
                {
                    if (ImGui::Button("Rename") || ret)
                    {
                        ds[std::string(buf)] = ds[k];
                        erased.push_back(k);
                    }
                }
                else
                {
                    ImGui::Text("Invalid name");
                }
                ImGui::EndPopup();
            }
        }

        ImGui::Columns(2, "datum", false);
        ImGui::Text("Script");
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1.0f);
        auto newlines = ds[k]->newlines();
        if (ImGui::InputTextMultiline("##txt",
                ds[k]->expr, sizeof(ds[k]->expr),
                {-1.0f, ImGui::GetTextLineHeight() * (2.3f + newlines)}))
        {
            ds[k]->update();
        }

        ImGui::Columns(2, "datum", false);
        ImGui::Text("Value");
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, Colors::base05);
        ImGui::PushStyleColor(ImGuiCol_TextSelectedBg,
                              Colors::transparent(Colors::blue));
        ImGui::InputText("##result", ds[k]->val_str, strlen(ds[k]->val_str),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();

        ImGui::PopID();
        ImGui::Separator();
    }

    for (auto k : erased)
    {
        ds.erase(k);
    }

    ImGui::End();
    ImGui::PopStyleVar(1);

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow();

    finish(window);
}


} // namespace ui
