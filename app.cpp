#include <cstring>

#include "imgui.h"

#include "app.hpp"
#include "ui.hpp"
#include "colors.hpp"

#include "graph/cell.hpp"

App::App()
 : window(ui::init()), focused({root.instance.get()})
{
    root.insertCell(root.sheet.get(), "a", "(+ 1 2)");
}

void App::run()
{
    // Main loop
    while (!ui::finished(window))
    {
        ui::start();
        draw();
        ui::finish(window);
    }

    ui::shutdown();
}

void App::drawCell(const Graph::Name& name, const Graph::Env& env)
{
    Graph::Sheet* sheet = env.back()->sheet;

    ImGui::PushID(name.c_str());

    ImGui::Columns(2, "datum", false);
    ImGui::SetColumnOffset(1, 60.0f);
    ImGui::Text("Name");
    ImGui::NextColumn();

    // Draw the cell's name, allowing for renaming
    {   // Temporary buffer for editing a cell's name
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0, 0.5});
        char buf[128];
        strcpy(buf, &name[0]);
        if (ImGui::Button(buf, {-1.0f, 0.0f}))
        {
            ImGui::OpenPopup("rename");
        }
        ImGui::PopStyleVar(1);
        if (ImGui::BeginPopup("rename"))
        {
            const bool ret = ImGui::InputText("##rename", buf, sizeof(buf),
                    ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            if (name == buf || root.canInsert(sheet, buf))
            {
                if (ImGui::Button("Rename") || ret)
                {
                    root.rename(sheet, name, buf);
                }
            }
            else
            {
                ImGui::Text("Invalid name");
            }
            ImGui::EndPopup();
        }
    }

    auto cell = sheet->cells.left.at(name);

    ImGui::Columns(2, "datum", false);
    ImGui::Text("Script");
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1.0f);

    {
        if (cell->expr.size() + 256 > editor_buf.size())
        {
            editor_buf.resize(editor_buf.size() + 4096);
        }
        std::copy(cell->expr.begin(), cell->expr.end(), editor_buf.begin());

        auto height = ImGui::CalcTextSize(cell->expr.c_str()).y +
            (cell->expr.back() == '\n' ? ImGui::GetFontSize() : 0);
        if (ImGui::InputTextMultiline("##txt",
                editor_buf.data(), editor_buf.size(),
                {-1.0f, ImGui::GetTextLineHeight() * 1.3f + height}))
        {
            root.editCell(cell, std::string(&editor_buf[0]));
        }
    }

    ImGui::Columns(2, "datum", false);
    ImGui::Text("Value");
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, Colors::base04);
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg,
                          Colors::transparent(Colors::blue));
    ImGui::PushAllowKeyboardFocus(false);
    ImGui::InputText("##result", &cell->strs[env][0],
                     cell->strs[env].size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::PopAllowKeyboardFocus();
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::PopID();

}

void App::drawSheet(const Graph::Env& env, float offset)
{
    Graph::Sheet* sheet = env.back()->sheet;

    ImGui::SetNextWindowPos({offset, 0});
    {   // Set window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        if (col_width.count(env))
        {
            ImGui::SetNextWindowSize({col_width[env], float(height)},
                    ImGuiSetCond_Always);
        }
        else
        {
            ImGui::SetNextWindowSize({200, float(height)},
                    ImGuiSetCond_Always);
        }
    }
    ImGui::SetNextWindowSizeConstraints({0, -1}, {FLT_MAX, -1});
    ImGui::Begin("Cells", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    ImGui::PushID(env.back());

    ImGui::BeginChild("CellsSub", ImVec2(-1, ImGui::GetWindowHeight() - 50));

    // Reserve keys and erased keys in a separate list here to avoid
    // glitches when we iterate over a changing map
    std::list<std::string> cells;
    bool drawn = false;
    for (auto& d : sheet->cells.left)
    {
        if (drawn)
        {
            ImGui::Separator();
        }
        drawn = true;
        cells.push_back(d.first);
    }

    for (auto k : cells)
    {
        drawCell(k, env);
    }
    ImGui::EndChild();

    ImGui::Separator();
    ImGui::Button("Add");
    ImGui::PopID();

    col_width[env] = ImGui::GetWindowSize().x;
    ImGui::End();
}

void App::draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    ImGui::SetNextWindowSize({float(width)/4, float(height)}, ImGuiSetCond_Appearing);
    float offset = 0;

    Graph::Env current_env = {};
    for (auto& f : focused)
    {
        current_env.push_back(f);
        drawSheet(current_env, offset);
        offset += ImGui::GetWindowSize().x;
    }

    ImGui::PopStyleVar(1);

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow();
}
