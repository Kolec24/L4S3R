#include <glad/glad.h>

#include "sorg.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

std::unordered_map<GLuint, sorg::ShaderData>                 shaders;
std::unordered_map<GLuint, std::vector<sorg::AttributeData>> vao_attributes;

GLuint sorg::SetupShaderProgram(const GLchar *vertexPath,
                                const GLchar *fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;

    ReadShaderFiles(vertexPath, fragmentPath, vertexCode, fragmentCode);

    GLuint vertexShader;
    GLuint fragmentShader;
    CreateAndCompileShaders(vertexCode.c_str(), fragmentCode.c_str(),
                            vertexShader, fragmentShader);

    GLuint id = glCreateProgram();
    AttachAndLinkShaders(id, vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    SetupShaderObjects(id);

    return id;
}

void sorg::SetupShaderObjects(GLuint program)
{
    // TODO: This is risky, as GL_ACTIVE_ATTRIBUTES returns the number of
    // properly used attributes - they must contribute to the final pipeline
    // output.
    // THINK ABOUT A BETTER SOLUTION!

    // Gather attribute information.
    GLint attribute_count;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attribute_count);

    std::vector<AttributeData> attributes(attribute_count);
    for (auto i = 0; i < attribute_count; ++i)
    {
        GLchar name[256];
        GLenum type;
        GLint  size;
        GLint  location;
        GLint  component_count;

        glGetActiveAttrib(program, i, sizeof(name), NULL, &size, &type, name);
        switch (type)
        {
        case GL_FLOAT:
            component_count = 1;
            break;
        case GL_FLOAT_VEC2:
            component_count = 2;
            break;
        case GL_FLOAT_VEC3:
            component_count = 3;
            break;
        case GL_FLOAT_VEC4:
            component_count = 4;
            break;
        default:
            component_count = 0;
            break;
        }

        location = glGetAttribLocation(program, name);

        AttributeData attribute;
        attribute.name            = name;
        attribute.type            = type;
        attribute.size            = size;
        attribute.location        = location;
        attribute.component_count = component_count;

        if (location >= attributes.size())
        {
            std::cout << "[SORG] Warning: SetupShaderObjects: Attribute layout "
                         "is bigger than number of attributes!\n"
                      << "SORG supports attributes that are numbered "
                         "consecutively from 0."
                      << std::endl;
            return;
        }

        attributes[location] = attribute;
    }

    // Compute stride and offset.
    GLint stride = 0;
    for (auto &attribute : attributes)
    {
        attribute.offset = stride;
        stride += attribute.component_count;
    }

    GLuint VAO;
    bool   found_vao = false;
    // for (const auto &vao : vertex_array_objects)
    // {
    //     // TODO: Decide what to do about this.
    //
    //     if (vao_attributes.count(vao) != 0 && vao_attributes[vao] ==
    //     attributes)
    //     {
    //         VAO       = vao;
    //         found_vao = true;
    //     }
    // }

    if (!found_vao)
    {
        glGenVertexArrays(1, &VAO);
        vao_attributes.insert({VAO, attributes});
    }

    // Setup a new VBO and EBO for this shader.
    GLuint VBO;
    GLuint EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    if (!found_vao)
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        for (const auto &attribute : attributes)
        {
            glVertexAttribPointer(attribute.location, attribute.component_count,
                                  GL_FLOAT, GL_FALSE, stride * sizeof(float),
                                  (void *)(attribute.offset * sizeof(float)));
            glEnableVertexAttribArray(attribute.location);
        }
    }

    ShaderData shader_data;
    shader_data.vertex_array_object   = VAO;
    shader_data.vertex_buffer_object  = VBO;
    shader_data.element_buffer_object = EBO;

    shaders.insert({program, shader_data});
}

void sorg::ReadShaderFiles(const GLchar *vertexPath, const GLchar *fragmentPath,
                           std::string &vertexCode, std::string &fragmentCode)
{
    // Read shader files.
    std::string   vertexCodeString;
    std::string   fragmentCodeString;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;

    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit |
                                  std::ifstream::badbit);
    try
    {
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);

        std::stringstream vertexStream, fragmentStream;

        vertexStream << vertexShaderFile.rdbuf();
        fragmentStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();

        vertexCode   = vertexStream.str();
        fragmentCode = fragmentStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "[SORG] Error: Shader: File reading failed!" << std::endl;
    }
}

void sorg::CreateAndCompileShaders(const GLchar *vertexCode,
                                   const GLchar *fragmentCode,
                                   GLuint &vertexShader, GLuint &fragmentShader)
{
    // Compile shaders.
    int    success;
    GLchar infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "[SORG] Error: Shader: Vertex compilation failed!\n"
                  << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "[SORG] Error: Shader: Fragment compilation failed!\n"
                  << infoLog << std::endl;
    }
}

void sorg::AttachAndLinkShaders(const GLuint &program,
                                const GLuint &vertexShader,
                                const GLuint &fragmentShader)
{
    int    success;
    GLchar infoLog[512];

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "[SORG] Error: Shader: Program linkink failed!\n"
                  << infoLog << std::endl;
    }
}

void sorg::ResetShaderProgram(GLuint program)
{
    if (shaders.count(program) == 0)
    {
        std::cout << "[SORG] Error: Reset: Trying to reset a shader which is "
                     "not initialised!"
                  << std::endl;
        return;
    }

    auto &shader_data = shaders[program];

    glUseProgram(0);
    glBindVertexArray(0);

    auto &shader_vao = shader_data.vertex_array_object;
    vao_attributes.erase(shader_vao);

    glDeleteVertexArrays(1, &shader_data.vertex_array_object);
    glDeleteBuffers(1, &shader_data.vertex_buffer_object);
    glDeleteBuffers(1, &shader_data.element_buffer_object);

    shaders.erase(program);
    glDeleteProgram(program);
}

void sorg::SetupLines(GLuint program, std::span<const float> vertices,
                      std::span<const unsigned int> indices, unsigned counter)
{
    if (shaders.count(program) == 0)
    {
        std::cout << "[SORG] Error: SetupLines: Invalid program ID!"
                  << std::endl;
        return;
    }

    auto &shader_data     = shaders[program];
    shader_data.counter   = counter;
    shader_data.primitive = GL_LINES;

    glBindVertexArray(shader_data.vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, shader_data.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_data.element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void sorg::SetupTriangles(GLuint program, std::span<const float> vertices,
                          std::span<const unsigned int> indices,
                          unsigned                      counter)
{
    if (shaders.count(program) == 0)
    {
        std::cout << "[SORG] Error: SetupTriangles: Invalid program ID!"
                  << std::endl;
        return;
    }

    auto &shader_data = shaders[program];

    shader_data.counter   = counter;
    shader_data.primitive = GL_TRIANGLES;

    glBindVertexArray(shader_data.vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, shader_data.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_data.element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void sorg::SetUniform(GLuint program, const GLchar *name, bool value)
{
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, name), value);
}

void sorg::SetUniform(GLuint program, const GLchar *name, float value)
{
    glUseProgram(program);
    glUniform1f(glGetUniformLocation(program, name), value);
}

void sorg::Draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto const &[shader, data] : shaders)
    {
        glUseProgram(shader);

        glBindVertexArray(data.vertex_array_object);
        glDrawElements(data.primitive, data.counter, GL_UNSIGNED_INT, 0);
    }
}
