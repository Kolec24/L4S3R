#ifndef _SORG_H_
#define _SORG_H_

#include <glad/glad.h>

#include <span>
#include <string>

namespace sorg
{

struct AttributeInfo
{
    std::string name;
    GLenum      type;
    GLint       location;
    GLint       size;
    GLint       componentCount;
};

struct ShaderObjects
{
    unsigned vertex_array_object;
    unsigned vertex_buffer_object;
    unsigned element_buffer_object;
};

struct Colour
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

void   InitialiseObjects();
GLuint SetupShaderProgram(const char *vertexPath, const char *fragmentPath);
void   ReadShaderFiles(const char *vertexPath, const char *fragmentPath,
                       std::string &vertexCode, std::string &fragmentCode);
void   CreateAndCompileShaders(const char *vertexCode, const char *fragmentCode,
                               GLuint &vertexShader, GLuint &fragmentShader);
void   AttachAndLinkShaders(const GLuint &program, const GLuint &vertexShader,
                            const GLuint &fragmentShader);

void SetupBasicShader(const char *vertexPath, const char *fragmentPath);
void SetupComplexShader(const char *vertexPath, const char *fragmentPath);
void SetupLines(std::span<const float>        vertices,
                std::span<const unsigned int> indices, unsigned counter);
void SetupTriangles(std::span<const float>        vertices,
                    std::span<const unsigned int> indices, unsigned counter);

void Draw(float current_time);
} // namespace sorg

#endif
