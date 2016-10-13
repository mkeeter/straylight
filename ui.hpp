#pragma once

#include <map>
#include <GLFW/glfw3.h>

class Datum;

namespace ui
{
typedef GLFWwindow Window;

Window* init();
void shutdown();
bool finished(Window* w);
void draw(Window* w, std::map<std::string, Datum*>& ds);
}
