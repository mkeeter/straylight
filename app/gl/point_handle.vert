#version 330

layout(location=0) in vec2 vertex_position;

uniform mat4 m_world;
uniform mat4 m_proj;
uniform vec3 pos;

out float radius;
out float angle;

void main()
{
    vec4 v = m_world * vec4(pos, 1.0f);
    vec4 p = m_proj * vec4(vertex_position, 0.0f, 1.0f);
    gl_Position = vec4(p.xy*0.025f + v.xy, v.z, 1.0f);

    radius = length(vertex_position);
    angle = (vertex_position.y == 0 && vertex_position.x == 0) ? 0 : atan(vertex_position.y, vertex_position.x);
}
