#version 330

layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 color;

uniform mat4 m;

out vec3 frag_color;

void main()
{
    gl_Position = m * vec4(vertex_position, 1.0f);
    frag_color = color;
}
