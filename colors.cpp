#include <cmath>
#include "colors.hpp"

namespace Colors
{
    glm::vec4    red(0xac/255.0f, 0x41/255.0f, 0x42/255.0f, 1.0f);
    glm::vec4 orange(0xd2/255.0f, 0x84/255.0f, 0x45/255.0f, 1.0f);
    glm::vec4 yellow(0xf4/255.0f, 0xbf/255.0f, 0x75/255.0f, 1.0f);
    glm::vec4  green(0x90/255.0f, 0xa9/255.0f, 0x59/255.0f, 1.0f);
    glm::vec4   teal(0x75/255.0f, 0xb5/255.0f, 0xaa/255.0f, 1.0f);
    glm::vec4   blue(0x6a/255.0f, 0x9f/255.0f, 0xb5/255.0f, 1.0f);
    glm::vec4 violet(0xaa/255.0f, 0x75/255.0f, 0x9f/255.0f, 1.0f);
    glm::vec4  brown(0x8f/255.0f, 0x55/255.0f, 0x36/255.0f, 1.0f);

    glm::vec4 base00(0x15/255.0f, 0x15/255.0f, 0x15/255.0f, 1.0f);
    glm::vec4 base01(0x20/255.0f, 0x20/255.0f, 0x20/255.0f, 1.0f);
    glm::vec4 base02(0x30/255.0f, 0x30/255.0f, 0x30/255.0f, 1.0f);
    glm::vec4 base03(0x50/255.0f, 0x50/255.0f, 0x50/255.0f, 1.0f);
    glm::vec4 base04(0xb0/255.0f, 0xb0/255.0f, 0xb0/255.0f, 1.0f);
    glm::vec4 base05(0xd0/255.0f, 0xd0/255.0f, 0xd0/255.0f, 1.0f);
    glm::vec4 base06(0xe0/255.0f, 0xe0/255.0f, 0xe0/255.0f, 1.0f);
    glm::vec4 base07(0xf5/255.0f, 0xf5/255.0f, 0xf5/255.0f, 1.0f);

glm::vec4 adjust(glm::vec4 c, float scale)
{
    c.r = fmax(fmin(c.r*scale, 255), 0);
    c.g = fmax(fmin(c.g*scale, 255), 0);
    c.b = fmax(fmin(c.b*scale, 255), 0);
    return c;
}

glm::vec4 highlight(glm::vec4 c)
{
    return adjust(c, 1.4);
}

glm::vec4 dim(glm::vec4 c)
{
    return adjust(c, 1/1.4);
}

glm::vec4 transparent(glm::vec4 c, float a)
{
    return {c.r, c.g, c.b, a};
}

}
