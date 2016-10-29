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
    root.insertCell(root.sheet.get(), "b", "(+ (a) 2)");
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
    auto cell = sheet->cells.left.at(name);

    ImGui::PushID(name.c_str());

    ImGui::Columns(2, "datum", false);
    ImGui::SetColumnOffset(1, 60.0f);
    ImGui::Text("Name");
    ImGui::NextColumn();

    {   // Draw the cell's name, allowing for renaming
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0, 0.5});

        // Temporary buffer in which we can rename the cell
        static char buf[128];
        static bool set_focus = false;
        if (ImGui::Button(name.c_str(), {-1.0f, 0.0f}))
        {
            strcpy(buf, &name[0]);
            ImGui::OpenPopup("rename");
            set_focus = true;
        }
        ImGui::PopStyleVar(1);

        if (ImGui::BeginPopup("rename"))
        {
            if (set_focus)
            {
                ImGui::SetKeyboardFocusHere();
                set_focus = false;
            }
            const bool ret = ImGui::InputText("##rename", buf, sizeof(buf),
                    ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            if (name == buf || root.canInsert(sheet, buf))
            {
                if (ImGui::Button("Rename") || ret)
                {
                    if (name != buf)
                    {
                        root.rename(sheet, name, buf);
                    }
                    ImGui::CloseCurrentPopup();
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

    {   // Draw the cell's expression and handle changes here
        if (cell->expr.size() + 256 > editor_buf.size())
        {
            editor_buf.resize(editor_buf.size() + 4096);
        }
        std::copy(cell->expr.begin(), cell->expr.end() + 1, editor_buf.begin());

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
    ImGui::PushStyleColor(ImGuiCol_FrameBg,
            cell->values[env].valid ?
            Colors::base03 : Colors::red);
    ImGui::InputText("##result", &cell->values[env].str[0],
                     cell->values[env].str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::PopAllowKeyboardFocus();
    ImGui::PopStyleColor(3);
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
    for (auto& d : sheet->cells.left)
    {
        cells.push_back(d.first);
    }

    bool drawn = false;
    for (auto k : cells)
    {
        if (drawn)
        {
            ImGui::Separator();
        }
        drawn = true;
        drawCell(k, env);
    }
    ImGui::EndChild();

    ImGui::Separator();
    drawAddMenu(env);
    ImGui::PopID();

    col_width[env] = ImGui::GetWindowSize().x;
    ImGui::End();
}

void App::drawAddMenu(const Graph::Env& env)
{
    if (ImGui::Button("Add", {-1, -1}))
    {
        ImGui::OpenPopup("add");
    }

    if (ImGui::BeginPopup("add"))
    {
        static char name_buf[128];
        static bool set_focus = false;

        if (ImGui::Selectable("Cell", ImGui::IsPopupOpen("addCell"),
                              ImGuiSelectableFlags_DontClosePopups))
        {
            ImGui::OpenPopup("addCell");
            set_focus = true;
            name_buf[0] = 0;
        }
        else if (ImGui::Selectable("Sheet", ImGui::IsPopupOpen("addSheet"),
                                   ImGuiSelectableFlags_DontClosePopups))
        {
            ImGui::OpenPopup("addSheet");
            set_focus = true;
            name_buf[0] = 0;
        }

        const auto sheet = env.back()->sheet;
        bool close_parent = false;

        // Submenu for adding a cell
        if (ImGui::BeginPopup("addCell"))
        {
            if (set_focus)
            {
                ImGui::SetKeyboardFocusHere();
                set_focus = false;
            }

            const bool ret = ImGui::InputText("##addCellInput", name_buf, sizeof(name_buf),
                    ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            if (root.canInsert(sheet, name_buf))
            {
                if (ImGui::Button("Insert") || ret)
                {
                    root.insertCell(sheet, name_buf, "(+ 1 2)");
                    ImGui::CloseCurrentPopup();
                    close_parent = true;
                }
            }
            else
            {
                ImGui::Text("Invalid name");
            }
            ImGui::EndPopup();
        }
        else if (ImGui::BeginPopup("addSheet"))
        {
            if (ImGui::Button("Boop"))
            {
                ImGui::CloseCurrentPopup();
                close_parent = true;
            }
            ImGui::EndPopup();
        }

        if (close_parent)
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
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
