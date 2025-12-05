#version 460 core

out vec4 FragColor;

in vec2 fragUV;
in vec4 fragColor;

float lineDist(vec2 point, vec2 start, vec2 end)
{
    vec2 to_point  = point - start;
    vec2 line = end - start;
    float h = clamp(dot(to_point, line) / dot(line, line), 0.0f, 1.0f);
    return length(to_point - line * h);
}

void main()
{
    float line_width = 0.01f;

    float x_1 = lineDist(fragUV, vec2(0.0f, 0.0f), vec2(1.0f, 1.0f));
    float x_2 = lineDist(fragUV, vec2(1.0f, 0.0f), vec2(0.0f, 1.0f));

    if (x_1 < line_width || x_2 < line_width)
    {
        FragColor = fragColor;
    }
    else
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
