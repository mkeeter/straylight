#version 330

uniform mat4 m;

in vec2 tex_coord;
uniform sampler2D depth;
uniform sampler2D norm;

out vec4 fragColor;

vec4 shade(vec3 n)
{
    vec3 light = vec3(0.99f, 0.96f, 0.89f);
    vec3 dark  = vec3(0.20f, 0.25f, 0.30f);

    // a is a light pointed directly down
    float a = dot(n, vec3(0.0f,   0.0f, 1.0f));

    // b is a light coming in from an angle
    float b = dot(n, vec3(0.57f, 0.57f, 0.57f));

    return vec4((a*light + (1.0f - a)*dark)*0.35f +
                (b*light + (1.0f - b)*dark)*0.65f, 1.0f);
}

void main()
{
    float d = texture(depth, tex_coord).r;
    if (d == 1.0f)
    {
        discard;
    }
    else
    {
        gl_FragDepth = d;
        if (d == 0.0f)
        {
            // Special color to indicate clipped pixels
            fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            fragColor = vec4(texture(norm, tex_coord).rgb, 1.0f); //shade(2.0f * (texture(norm, tex_coord).rgb - 0.5f));
        }
    }
}
