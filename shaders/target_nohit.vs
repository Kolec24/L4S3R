#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

out vec2 fragUV;
out vec4 fragColor;

void main()
{
    gl_Position = vec4(position, 1.0f);
    fragUV = uv;
    fragColor = color;
}
