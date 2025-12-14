#version 460 core

out vec4 FragColor;

in vec2 fragUV;

void main()
{
    float border_width = 5.0f;

    // distance to closest edge in UV space
    vec2 d = min(fragUV, 1.0 - fragUV);

    // gradient of UV in screen space
    vec2 duvdx = dFdx(fragUV);
    vec2 duvdy = dFdy(fragUV);

    // convert UV distance to pixel distance
    float pxDist = min(
        d.x / length(vec2(duvdx.x, duvdy.x)),
        d.y / length(vec2(duvdx.y, duvdy.y))
    );

    if (pxDist < border_width)
    {
        FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
