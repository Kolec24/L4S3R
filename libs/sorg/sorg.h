#ifndef _SORG_H_
#define _SORG_H_

#include <glad/glad.h>

#include <span>
#include <string>

namespace sorg
{

struct AttributeData
{
    std::string name;
    GLenum      type;
    GLint       location;
    GLint       size;
    GLint       component_count;
    GLint       offset;

    bool operator==(const AttributeData &rhs)
    {
        return name == rhs.name && type == rhs.type &&
               location == rhs.location && offset == rhs.offset;
    }
};

struct ShaderData
{
    GLuint vertex_array_object;
    GLuint vertex_buffer_object;
    GLuint element_buffer_object;

    GLenum primitive;
    GLint  counter;
};

struct Colour
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

GLuint SetupShaderProgram(const char *vertexPath, const char *fragmentPath);
void   SetupShaderObjects(GLuint program);
void   ReadShaderFiles(const char *vertexPath, const char *fragmentPath,
                       std::string &vertexCode, std::string &fragmentCode);
void   CreateAndCompileShaders(const char *vertexCode, const char *fragmentCode,
                               GLuint &vertexShader, GLuint &fragmentShader);
void   AttachAndLinkShaders(const GLuint &program, const GLuint &vertexShader,
                            const GLuint &fragmentShader);

void SetupLines(GLuint program, std::span<const float> vertices,
                std::span<const unsigned int> indices, unsigned counter);
void SetupTriangles(GLuint program, std::span<const float> vertices,
                    std::span<const unsigned int> indices, unsigned counter);
void SetUniform(GLuint program, const char *name, float value);

void Draw(float current_time);
} // namespace sorg

#endif
