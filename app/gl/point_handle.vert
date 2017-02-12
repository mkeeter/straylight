#version 330

layout(location=0) in vec2 vertex_position;

uniform mat4 m;
uniform vec3 p;

void main()
{
    vec4 v = m * vec4(p, 1.0f);
    gl_Position = vec4(vertex_position + v.xy, v.z, 1.0f);
}
