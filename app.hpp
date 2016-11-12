#pragma once

#include "graph/root.hpp"

struct GLFWwindow;

class App
{
public:
    App();

    void run();
private:
    void drawCell(const Graph::Name& name, const Graph::Env& env, float offset);
    void drawInstance(const Graph::Name& name, const Graph::Env& env);

    /*
     *  Draws an individual sheet as a column
     *  Returns the width of that sheet
     */
    float drawSheet(const Graph::Env& env, float offset=0.0f);

    void drawAddMenu(const Graph::Env& env);

    void renamePopup(Graph::Sheet* sheet, const Graph::Name& name,
                     bool set_focus, char* buf, size_t buf_size);
    void renameSheetPopup(Graph::Sheet* sheet, const Graph::Name& name,
                          bool set_focus, char* buf, size_t buf_size);
    void draw();

    static std::string windowName(const Graph::Env& env);

    Graph::Root root;
    GLFWwindow* window;

    // UI state
    Graph::Env focused;
    std::vector<char> editor_buf;
};
