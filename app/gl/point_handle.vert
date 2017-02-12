#version 330

layout(location=0) in vec2 vertex_position;

uniform mat4 m;

void main()
{
    //gl_Position = m * vec4(vertex_position, 0.0f, 1.0f);
    gl_Position = vec4(vertex_position, 0.0f, 1.0f);
}
