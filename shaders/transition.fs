#version 460 core

out vec4 FragColor;

in vec2 fragUV;

uniform float uVerticalLimit;

void main()
{
    float bottomLimit = uVerticalLimit;
    float topLimit = uVerticalLimit - 1.0f;
    if (fragUV.y <= bottomLimit && fragUV.y >= topLimit)
    {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}
