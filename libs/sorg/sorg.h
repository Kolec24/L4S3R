#ifndef _SORG_H_
#define _SORG_H_

#include <span>

namespace sorg
{

struct Colour
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

void InitialiseObjects();
void SetupBasicShader(const char *vertexPath, const char *fragmentPath);
void SetupComplexShader(const char *vertexPath, const char *fragmentPath);
void SetupLines(std::span<const float>        vertices,
                std::span<const unsigned int> indices, unsigned counter);
void SetupTriangles(std::span<const float>        vertices,
                    std::span<const unsigned int> indices, unsigned counter);

void Draw(float current_time);
} // namespace sorg

#endif
