#version 330

out vec4 fragColor;

in float radius;
in float angle;

uniform vec3 color_center;
uniform vec3 color_edge;

void main()
{
    fragColor = vec4(radius < 0.6 ? color_center : color_edge, 1.0f);
}
