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
    bool erased = false;

    ImGui::PushID(name.c_str());

    ImGui::Columns(2, "datum", false);
    ImGui::SetColumnOffset(1, 60.0f);
    ImGui::Text("Name");
    ImGui::NextColumn();

    {   // Draw the cell's name, allowing for renaming
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0, 0.5});

        // Temporary buffer in which we can rename the cell
        static char buf[128];
        bool set_focus = false;
        if (ImGui::Button(name.c_str(),
                    {ImGui::GetContentRegionAvailWidth() - 30, 0.0f}))
        {
            strcpy(buf, &name[0]);
            ImGui::OpenPopup("rename");
            set_focus = true;
        }
        ImGui::PopStyleVar(1);
        renamePopup(sheet, name, set_focus, buf, sizeof(buf));

        ImGui::SameLine();
        if (ImGui::Button("×", {-1.0f, 0.0f}))
        {
            erased = true;
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
    ImGui::Columns(1);

    if (erased)
    {
        root.eraseCell(cell);
    }
}

void App::renameSheetPopup(Graph::Sheet* parent, const Graph::Name& name,
                           bool set_focus, char* buf, size_t buf_size)
{
    if (ImGui::BeginPopup("rename"))
    {
        if (set_focus)
        {
            ImGui::SetKeyboardFocusHere();
        }
        const bool ret = ImGui::InputText("##rename", buf, buf_size,
                ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();

        // If the name hasn't changed or we can do the insertion, then
        // show a button and rename on button press
        if (name == buf || root.canCreateSheet(parent, buf))
        {
            if (ImGui::Button("Rename") || ret)
            {
                if (name != buf)
                {
                    root.renameSheet(parent, name, buf);
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

void App::renamePopup(Graph::Sheet* sheet, const Graph::Name& name,
                      bool set_focus, char* buf, size_t buf_size)
{
    if (ImGui::BeginPopup("rename"))
    {
        if (set_focus)
        {
            ImGui::SetKeyboardFocusHere();
        }
        const bool ret = ImGui::InputText("##rename", buf, buf_size,
                ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();

        // If the name hasn't changed or we can do the insertion, then
        // show a button and rename on button press
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

void App::drawInstance(const Graph::Name& name, const Graph::Env& env)
{
    Graph::Sheet* sheet = env.back()->sheet;
    auto instance = sheet->instances.left.at(name);
    auto parent_sheet = instance->sheet->parent;
    std::string sheet_name = parent_sheet->library.right.at(instance->sheet);
    bool erased = false;

    ImGui::PushID(instance);

    {   // Draw the instance's name, allowing for renaming
        ImGui::PushID("instance name");
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0, 0.5});

        // Temporary buffer in which we can rename the cell
        static char buf[128];
        bool set_focus = false;
        if (ImGui::Button(name.c_str(),
                    {ImGui::GetContentRegionAvailWidth()*0.3f, 0.0f}))
        {
            strcpy(buf, &name[0]);
            ImGui::OpenPopup("rename");
            set_focus = true;
        }
        ImGui::PopStyleVar(1);
        renamePopup(sheet, name, set_focus, buf, sizeof(buf));
        ImGui::PopID();
    }

    ImGui::SameLine();
    ImGui::Text("is a");
    ImGui::SameLine();

    {   // Draw the Sheet name, allowing for renaming
        ImGui::PushID("sheet name");
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0, 0.5});

        // Temporary buffer in which we can rename the cell
        static char buf[128];
        bool set_focus = false;
        if (ImGui::Button(sheet_name.c_str(),
                    {ImGui::GetContentRegionAvailWidth() - 30 - ImGui::GetStyle().ScrollbarSize, 0.0f}))
        {
            strcpy(buf, &sheet_name[0]);
            ImGui::OpenPopup("rename");
            set_focus = true;
        }
        ImGui::PopStyleVar(1);
        renameSheetPopup(parent_sheet, sheet_name,
                         set_focus, buf, sizeof(buf));
        ImGui::PopID();

        ImGui::SameLine();
        if (ImGui::Button("×", {ImGui::GetContentRegionAvailWidth() - ImGui::GetStyle().ScrollbarSize, 0.0f}))
        {
            erased = true;
        }
    }

    if (erased)
    {
        root.eraseInstance(instance);
    }
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
            ImGui::SetNextWindowSize({300, float(height)},
                    ImGuiSetCond_Always);
        }
    }
    ImGui::SetNextWindowSizeConstraints({0, -1}, {FLT_MAX, -1});
    ImGui::Begin("Cells", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    ImGui::PushID(env.back());

    ImGui::BeginChild("CellsSub", ImVec2(-1, ImGui::GetWindowHeight() - 50));

    // Reserve keys and erased keys in a separate list here to avoid
    // glitches when we iterate over a changing map
    std::list<void*> items;
    for (auto& d : sheet->order)
    {
        items.push_back(d);
    }

    bool drawn = false;
    for (auto k : items)
    {
        if (drawn)
        {
            ImGui::Separator();
        }
        drawn = true;

        if (auto c = sheet->isCell(k))
        {
            drawCell(sheet->cells.right.at(c), env);
        }
        else if (auto i = sheet->isInstance(k))
        {
            drawInstance(sheet->instances.right.at(i), env);
        }
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
        static char sheet_buf[128];
        bool set_focus = false;
        static Graph::Sheet* instance_target = nullptr;

        // Accumulate a list of all possible sheets
        // (recursively upwards through the document)
        std::set<std::pair<std::string, Graph::Sheet*>> library;
        auto p = env.back()->sheet;
        while (p)
        {
            for (auto& s : p->library.left)
            {
                library.insert({s.first, s.second});
            }
            p = p->parent;
        }
        // Draw all potential sheets in the library
        for (auto& s : library)
        {
            if (ImGui::Selectable(s.first.c_str(),
                                  ImGui::IsPopupOpen(s.first.c_str()),
                                  ImGuiSelectableFlags_DontClosePopups))
            {
                ImGui::OpenPopup(s.first.c_str());
                set_focus = true;
                instance_target = s.second;
                strcpy(name_buf, "instance-name");
            }
        }

        if (library.size())
        {
            ImGui::Separator();
        }

        // Draw normal menus for adding Cells and Sheets
        if (ImGui::Selectable("Cell", ImGui::IsPopupOpen("addCell"),
                              ImGuiSelectableFlags_DontClosePopups))
        {
            ImGui::OpenPopup("addCell");
            set_focus = true;
            strcpy(name_buf, "cell-name");
        }
        else if (ImGui::Selectable("Sheet", ImGui::IsPopupOpen("addSheet"),
                                   ImGuiSelectableFlags_DontClosePopups))
        {
            ImGui::OpenPopup("addSheet");
            set_focus = true;
            strcpy(name_buf, "instance-name");
            strcpy(sheet_buf, "Sheet name");
        }

        const auto sheet = env.back()->sheet;
        bool close_parent = false;

        // Submenu for adding a cell
        if (ImGui::BeginPopup("addCell"))
        {
            if (set_focus)
            {
                ImGui::SetKeyboardFocusHere();
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
        // Submenu for adding a sheet
        else if (ImGui::BeginPopup("addSheet"))
        {
            ImGui::Dummy({300, 0});
            bool ret = false;

            if (set_focus)
            {
                ImGui::SetKeyboardFocusHere();
            }
            ret = ImGui::InputText("Instance name", name_buf, sizeof(name_buf),
                    ImGuiInputTextFlags_EnterReturnsTrue);

            if (ret)
            {
                ImGui::SetKeyboardFocusHere();
            }
            ret = ImGui::InputText("Sheet name", sheet_buf, sizeof(sheet_buf),
                    ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::PushItemWidth(-1);
            if (!root.canInsert(sheet, name_buf))
            {
                ImGui::Text("Invalid instance name");
            }
            else if (!root.canCreateSheet(sheet, sheet_buf))
            {
                ImGui::Text("Invalid sheet name");
            }
            else
            {
                if (ImGui::Button("Create", {-1, 0}) || ret)
                {
                    auto s = root.createSheet(sheet, sheet_buf);
                    root.insertInstance(sheet, name_buf, s);
                    ImGui::CloseCurrentPopup();
                    close_parent = true;
                }
            }
            ImGui::PopItemWidth();
            ImGui::EndPopup();
        }
        // Submenu for adding an instance
        else
        {
            for (auto& s : library)
            {
                if (ImGui::BeginPopup(s.first.c_str()))
                {
                    if (set_focus)
                    {
                        ImGui::SetKeyboardFocusHere();
                    }

                    const bool ret = ImGui::InputText("##addInstance",
                            name_buf,sizeof(name_buf),
                            ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    if (root.canInsert(sheet, name_buf))
                    {
                        if (ImGui::Button("Insert") || ret)
                        {
                            root.insertInstance(
                                    sheet, name_buf, instance_target);
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
            }
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
