#version 330

uniform mat4 m;

uniform int draw_mode;
uniform vec3 picker_color;

in vec2 tex_coord;
uniform sampler2D depth;
uniform sampler2D norm;

out vec4 fragColor;

vec4 shade(vec3 n)
{
    if (draw_mode == 0)
    {
        return vec4(picker_color, 1.0f);
    }

    // Calculate dot product coming in from three different angles
    // (imitating three-point lighting)
    vec3 n_ = normalize(n);
    float key = dot(n_, vec3(0.0f,   0.0f, 1.0f));
    float fill = max(0.0f, dot(n_, vec3(0.4f, -0.4f, 0.8f)));
    float back = max(0.0f, dot(n_, vec3(-0.6f, 0.6f, 0.5f)));

    // Mix the three lights to get an overall brightness
    float brightness = key * 0.5f + fill * 0.4f + back * 0.1f;

    // Fake gamma correction
    brightness = sqrt(brightness);

    // Brighten up if we're being hovered
    if (draw_mode == 2)
    {
        brightness = brightness * 0.8f + 0.2f;
    }

    vec3 light = vec3(0.99f, 0.96f, 0.89f);
    vec3 dark  = vec3(0.10f, 0.15f, 0.20f);

    return vec4(brightness * light + (1 - brightness) * dark, 1.0f);
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
            fragColor = shade(2.0f * (texture(norm, tex_coord).rgb - 0.5f));
        }
    }
}
