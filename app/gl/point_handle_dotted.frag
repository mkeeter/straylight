#version 330

out vec4 fragColor;

in float radius;
in vec2 pt;

uniform vec3 color_edge;

void main()
{
    const float PI = 3.14159265359;

    float angle = atan(pt.y, pt.x);
    if (radius < 0.6 || mod(angle + PI, PI/4) - PI/8 > 0)
    {
        discard;
    }
    else
    {
        fragColor = vec4(color_edge, 1.0f);
    }
}

