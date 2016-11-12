#include <cstring>

#include "imgui.h"
#include "IconsFontAwesome.h"

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

void App::drawCell(const Graph::Name& name, const Graph::Env& env, float offset)
{
    Graph::Sheet* sheet = env.back()->sheet;
    const auto cell = sheet->cells.left.at(name);
    bool erased = false;

    ImGui::PushID(name.c_str());
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
    if (ImGui::Button((name + ":").c_str()))
    {
        ImGui::OpenPopup("cell context menu");
    }
    ImGui::PopStyleColor();

    // Open up the cell editing menu
    if (ImGui::BeginPopup("cell context menu"))
    {
        static char buf[128];
        bool set_focus = false;

        erased |= ImGui::Selectable("Erase");

        // Nested menu for renaming the cell
        if (ImGui::Selectable("Rename", ImGui::IsPopupOpen("renameCell"),
                              ImGuiSelectableFlags_DontClosePopups))
        {
            strcpy(buf, &name[0]);
            ImGui::OpenPopup("rename");
            set_focus = true;
        }
        renamePopup(sheet, name, set_focus, buf, sizeof(buf));
        ImGui::EndPopup();
    }

    ImGui::SameLine(offset);
    static Graph::Cell* active_cell = nullptr;
    bool just_focused = false;

    if (cell->type == Graph::Cell::INPUT &&
        active_cell != cell)
    {
        if (ImGui::Button(ICON_FA_ARROW_RIGHT " (input)"))
        {
            active_cell = cell;
            ImGui::SetKeyboardFocusHere();
            just_focused = true;
        }
    }
    // Draw the cell's expression and handle changes here
    else if (cell->type != Graph::Cell::INPUT ||
             active_cell == cell)
    {
        if (cell->expr.size() + 256 > editor_buf.size())
        {
            editor_buf.resize(editor_buf.size() + 4096);
        }
        std::copy(cell->expr.begin(), cell->expr.end() + 1, editor_buf.begin());

        const auto height = ImGui::CalcTextSize(cell->expr.c_str()).y +
            (cell->expr.back() == '\n' ? ImGui::GetFontSize() : 0);
        if (ImGui::InputTextMultiline("##txt",
                editor_buf.data(), editor_buf.size(),
                {ImGui::GetContentRegionAvailWidth(),
                 ImGui::GetTextLineHeight() * 1.3f + height}))
        {
            auto type_before = cell->type;
            root.editCell(cell, std::string(&editor_buf[0]));

            // If this cell just became an input, then don't turn it into
            // a button right away
            if (cell->type == Graph::Cell::INPUT &&
                type_before != Graph::Cell::INPUT)
            {
                active_cell = cell;
            }
        }
    }

    // If the text field isn't active anymore, turn it into a pumpkin
    if (!ImGui::IsItemActive() && active_cell == cell && !just_focused)
    {
        active_cell = nullptr;
    }

    // Draw the value if it's different (as a string) from the expr
    if (cell->expr != cell->values[env].str)
    {
        ImGui::Dummy({0,0});
        ImGui::SameLine(offset);
        ImGui::PushItemWidth(-1.0f);
        ImGui::PushStyleColor(ImGuiCol_Text,
                cell->values[env].valid
                ? ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]
                : ImGui::GetStyle().Colors[ImGuiCol_Text]);
        ImGui::PushStyleColor(ImGuiCol_TextSelectedBg,
            Colors::transparent(ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg]));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,
                cell->values[env].valid ?
                ImGui::GetStyle().Colors[ImGuiCol_FrameBg] : ImVec4(Colors::red));

        ImGui::PushAllowKeyboardFocus(false);
        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        ImGui::InputText("##result", &cell->values[env].str[0],
                         cell->values[env].str.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::PopAllowKeyboardFocus();
        ImGui::PopStyleColor(3);
        ImGui::PopItemWidth();
    }
    ImGui::PopID();

    if (erased)
    {
        root.eraseCell(cell);
    }
}

void App::renameSheetPopup(Graph::Sheet* parent, const Graph::Name& name,
                           bool set_focus, char* buf, size_t buf_size)
{
    if (ImGui::BeginPopup("renameSheet"))
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
    bool expand = false;

    ImGui::PushID(instance);
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
    if (ImGui::Button((name + " (" + sheet_name + ")").c_str()))
    {
        ImGui::OpenPopup("sheet context menu");
    }
    ImGui::PopStyleColor();

    if (ImGui::BeginPopupContextItem("sheet context menu"))
    {
        // Temporary buffer in which we can rename the cell
        static char buf[128];
        bool set_focus = false;
        if (ImGui::Selectable("Rename instance",
                ImGui::IsPopupOpen("rename"),
                ImGuiSelectableFlags_DontClosePopups))
        {
            strcpy(buf, &name[0]);
            ImGui::OpenPopup("rename");
            set_focus = true;
        }
        else if (ImGui::Selectable("Rename sheet",
                ImGui::IsPopupOpen("renameSheet"),
                ImGuiSelectableFlags_DontClosePopups))

        {
            strcpy(buf, &sheet_name[0]);
            ImGui::OpenPopup("renameSheet");
            set_focus = true;
        }
        else if (ImGui::Selectable("Erase instance"))
        {
            erased = true;
        }
        else if (ImGui::Selectable("Edit sheet"))
        {
            expand = true;
        }
        renamePopup(sheet, name, set_focus, buf, sizeof(buf));
        renameSheetPopup(parent_sheet, sheet_name,
                         set_focus, buf, sizeof(buf));
        ImGui::EndPopup();
    }

    // Get all input or output cells that should be drawn
    std::list<Graph::Cell*> cells;
    for (const auto& k : instance->sheet->order)
    {
        if (auto c = instance->sheet->isCell(k))
        {
            if (c->type == Graph::Cell::INPUT ||
                c->type == Graph::Cell::OUTPUT)
            {
                cells.push_back(c);
            }
        }
    }

    float max_width = 0.0f;
    for (const auto& c : cells)
    {
        auto s = ImGui::CalcTextSize(instance->sheet->cells.right.at(c).c_str());
        max_width = fmax(s.x + 30.0f, max_width);
    }

    for (auto& c : cells)
    {
        ImGui::Dummy({0,0});
        ImGui::SameLine(20.0f);

        ImGui::PushID(c);
        if (c->type == Graph::Cell::INPUT)
        {
            ImGui::Text("%s: ", instance->sheet->cells.right.at(c).c_str());
            ImGui::SameLine(max_width);

            const auto& expr = instance->inputs.at(c);
            if (expr.size() + 256 > editor_buf.size())
            {
                editor_buf.resize(editor_buf.size() + 4096);
            }
            std::copy(expr.begin(), expr.end() + 1, editor_buf.begin());

            const auto height = ImGui::CalcTextSize(expr.c_str()).y +
                (expr.back() == '\n' ? ImGui::GetFontSize() : 0);
            if (ImGui::InputTextMultiline("##txt",
                    editor_buf.data(), editor_buf.size(),
                    {ImGui::GetContentRegionAvailWidth(),
                     ImGui::GetTextLineHeight() * 1.3f + height}))
            {
                root.editInput(instance, c, &editor_buf[0]);
            }
        }
        ImGui::PopID();
    }


    if (erased)
    {
        root.eraseInstance(instance);
    }
    else if (expand)
    {
        focused.erase(std::find(focused.begin(), focused.end(), env.back()) + 1,
                      focused.end());
        focused.push_back(instance);
    }
    ImGui::PopID();
}

float App::drawSheet(const Graph::Env& env, float offset)
{
    static std::map<Graph::Env, float> col_width;

    Graph::Sheet* sheet = env.back()->sheet;
    bool stay_open = true;

    ImGui::PushID(env.back());
    ImGui::SetNextWindowPos({offset, 0});

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // Set window size
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

    ImGui::SetNextWindowSizeConstraints({0, -1}, {FLT_MAX, -1});
    ImGui::Begin(windowName(env).c_str(),
                 env.size() == 1 ? nullptr : &stay_open,
                 ImGuiWindowFlags_CollapseHorizontal);

    /*
    if (offset > 0)
    {
        auto p = ImGui::GetCursorScreenPos();
        auto pad = ImGui::GetStyle().WindowPadding;
        printf("%f %f\t%f\n", p.x, p.y, offset);
        ImGui::GetWindowDrawList()->AddLine(
                {p.x - pad.x + 2, p.y - pad.y},
                {p.x - pad.x + 2, p.y - pad.y - 50 + height},
                ImColor(Colors::blue), 4);
    }
    */

    ImGui::BeginChild("CellsSub", ImVec2(-1, ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() - 50));

    // Reserve keys and erased keys in a separate list here to avoid
    // glitches when we iterate over a changing map
    std::list<void*> items;
    for (auto& d : sheet->order)
    {
        items.push_back(d);
    }

    float max_width = 0.0f;
    for (const auto& k : items)
    {
        if (auto c = sheet->isCell(k))
        {
            auto s = ImGui::CalcTextSize(sheet->cells.right.at(c).c_str());
            max_width = fmax(s.x + 20.0f, max_width);
        }
    }

    bool drawn = false;
    for (const auto& k : items)
    {
        if (drawn)
        {
            ImGui::Separator();
        }
        drawn = true;

        if (auto c = sheet->isCell(k))
        {
            drawCell(sheet->cells.right.at(c), env, max_width);
        }
        else if (auto i = sheet->isInstance(k))
        {
            drawInstance(sheet->instances.right.at(i), env);
        }
    }
    ImGui::EndChild();

    ImGui::Separator();
    drawAddMenu(env);

    float output;
    if (ImGui::IsWindowCollapsed())
    {
        output = ImGui::GetWindowSize().x;
    }
    else
    {
        col_width[env] = ImGui::GetWindowSize().x;
        output = col_width[env];
    }
    ImGui::End();
    ImGui::PopID();

    if (env.size() > 1 && !stay_open)
    {
        focused.erase(std::find(focused.begin(), focused.end(), env.back()),
                      focused.end());
    }

    return output;
}

void App::drawAddMenu(const Graph::Env& env)
{
    if (ImGui::Button("Add " ICON_FA_PLUS, {-1, -1}))
    {
        ImGui::OpenPopup("add");
    }

    if (ImGui::BeginPopup("add"))
    {
        static char name_buf[128];
        static char sheet_buf[128];
        bool set_focus = false;
        const auto sheet = env.back()->sheet;
        static Graph::Sheet* instance_target = nullptr;

        // Accumulate a list of all possible sheets
        // (recursively upwards through the document)
        std::set<std::pair<std::string, Graph::Sheet*>> library;
        auto p = env.back()->sheet;
        while (p)
        {
            for (auto& s : p->library.left)
            {
                std::pair<std::string, Graph::Sheet*> key = {s.first, s.second};
                if (!library.count(key))
                {
                    library.insert(key);
                }
            }
            p = p->parent;
        }

        // Draw all potential sheets in the library
        for (auto& s : library)
        {
            const bool can_insert = root.canInsertInstance(sheet, s.second);
            ImGui::PushStyleColor(ImGuiCol_Text,
                    can_insert ?
                    ImGui::GetStyle().Colors[ImGuiCol_Text] :
                    ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
            if (ImGui::Selectable(s.first.c_str(),
                                  ImGui::IsPopupOpen(s.first.c_str()),
                                  ImGuiSelectableFlags_DontClosePopups) &&
                can_insert)
            {
                ImGui::OpenPopup(s.first.c_str());
                set_focus = true;
                instance_target = s.second;
                strcpy(name_buf, "instance-name");
            }
            else if (ImGui::BeginPopupContextItem("wtf"))
            {
                ImGui::EndPopup();
            }


            ImGui::PopStyleColor(1);
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
    for (unsigned i=0; i < focused.size(); ++i)
    {
        current_env.push_back(focused[i]);
        offset += drawSheet(current_env, offset);
    }

    ImGui::PopStyleVar(1);

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow();
}

std::string App::windowName(const Graph::Env& env)
{
    std::stringstream ss;

    if (env.size() == 1)
    {
        ss << "Root";
    }
    else
    {
        const auto instance_name =
            env.back()->sheet->parent->instances.right.at(env.back()).c_str();
        const auto sheet_name =
            env.back()->parent->library.right.at(env.back()->sheet).c_str();
        ss << instance_name << " (" << sheet_name << ")";
    }
    ss << "###";
    for (const auto& e : env)
    {
        ss << (void*)e;
    }
    return ss.str();
}
