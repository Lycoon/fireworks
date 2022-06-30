#version 460 core

in vec2 UV;
in vec4 fragmentColor;

uniform sampler2D sampler;

out vec4 color;

void main()
{
    vec4 col = texture(sampler, UV) * fragmentColor;
    if (all(lessThan(col, vec4(0.1, 0.1, 0.1, 1.0))))
        discard;

    color = col;
}