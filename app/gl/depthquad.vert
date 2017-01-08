#version 330

layout(location=0) in vec3 vertex_position;

uniform mat4 m;
out vec2 tex_coord;

void main()
{
    gl_Position = m * vec4(vertex_position, 1.0f);
    tex_coord = (vertex_position.xy + 1.0f) / 2.0f;
}
