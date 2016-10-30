#pragma once

#include "graph/root.hpp"

struct GLFWwindow;

class App
{
public:
    App();

    void run();
private:
    void drawCell(const Graph::Name& name, const Graph::Env& env);
    void drawInstance(const Graph::Name& name, const Graph::Env& env);
    void drawSheet(const Graph::Env& env, float offset=0.0f);
    void drawAddMenu(const Graph::Env& env);

    void draw();

    Graph::Root root;
    GLFWwindow* window;

    // UI state
    Graph::Env focused;
    std::vector<char> editor_buf;
    std::map<Graph::Env, float> col_width;
};
