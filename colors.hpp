#pragma once

#include <glm/vec4.hpp>

namespace Colors
{
extern glm::vec4 red;
extern glm::vec4 orange;
extern glm::vec4 yellow;
extern glm::vec4 green;
extern glm::vec4 teal;
extern glm::vec4 blue;
extern glm::vec4 violet;
extern glm::vec4 brown;

extern glm::vec4 base00;
extern glm::vec4 base01;
extern glm::vec4 base02;
extern glm::vec4 base03;
extern glm::vec4 base04;
extern glm::vec4 base05;
extern glm::vec4 base06;
extern glm::vec4 base07;

glm::vec4 adjust(glm::vec4 c, float scale);
glm::vec4 highlight(glm::vec4 c);
glm::vec4 dim(glm::vec4 c);
glm::vec4 transparent(glm::vec4 c, float a=0.5);
}
