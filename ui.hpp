#pragma once

#include <GLFW/glfw3.h>

namespace ui
{
typedef GLFWwindow Window;

Window* init();
void shutdown();
bool finished(Window* w);
void draw(Window* w);
}
