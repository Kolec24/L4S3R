#version 460 core

out vec4 FragColor;

in vec2 fragUV;
in vec4 fragColor;

uniform float uTime;

void main()
{
    vec2 centre = vec2(0.5f, 0.5f);
    float distance = distance(fragUV, centre);

    float min_radius = 0.1f * (sin(uTime) + 2.0f);
    float max_radius = 0.15f * (sin(uTime) + 2.0f);

    if (distance < max_radius && distance > min_radius)
    {
        FragColor = fragColor;
    }
    else
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
