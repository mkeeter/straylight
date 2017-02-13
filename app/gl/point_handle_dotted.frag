#version 330

out vec4 fragColor;

in float radius;
in float angle;

uniform vec3 color_edge;

void main()
{
    const float PI = 3.14159265359;

    if (radius < 0.6 || mod(angle + PI, PI/5) - PI/10 > 0)
    {
        discard;
    }
    else
    {
        fragColor = vec4(color_edge, 1.0f);
    }
}

