#pragma once

#include <map>
#include <GLFW/glfw3.h>

namespace ui
{
GLFWwindow* init();
void start();
void finish(GLFWwindow* window);

bool finished(GLFWwindow* w);
void shutdown();
}
