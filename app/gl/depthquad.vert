#version 330

layout(location=0) in vec2 vertex_position;

uniform mat4 m;
uniform mat4 m_quad;
uniform sampler2D depth;
out vec2 tex_coord;

void main()
{
    tex_coord = (vertex_position.yx + 1.0f) / 2.0f;

    // Position in view space (as rendered)
    float t = texture(depth, tex_coord).r;
    vec3 p = vec3(vertex_position.xy, (t == 1.0f) ? 0.0f : (1.0f - t  * 2.0f));

    // Transformed into our current view space
    gl_Position = m * m_quad * vec4(p, 1.0f);
}
