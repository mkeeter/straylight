#version 330

layout(location=0) in vec2 vertex_position;

uniform mat4 m;
uniform mat4 m_quad;
out vec2 tex_coord;

void main()
{
    gl_Position = m * m_quad * vec4(vertex_position, 0.0f, 1.0f);
    tex_coord = (vertex_position.yx + 1.0f) / 2.0f;
}
