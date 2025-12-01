#include <glad/glad.h>

#include "sorg.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

std::map<unsigned, sorg::ShaderObjects> shaders;
std::vector<unsigned>                   vertex_array_objects;

// TODO: Fix these objects.
unsigned basic_lines_vao = 0;
unsigned basic_lines_vbo = 0;
unsigned basic_lines_ebo = 0;
GLuint   basic_shader;
unsigned lines_counter = 0;

unsigned complex_triangles_vao = 0;
unsigned complex_triangles_vbo = 0;
unsigned complex_triangles_ebo = 0;
GLuint   complex_shader;
unsigned complex_triangles_counter = 0;

void sorg::InitialiseObjects()
{
    // Basic lines.
    glGenBuffers(1, &basic_lines_vbo);
    glGenVertexArrays(1, &basic_lines_vao);
    glGenBuffers(1, &basic_lines_ebo);

    glBindVertexArray(basic_lines_vao);
    glBindBuffer(GL_ARRAY_BUFFER, basic_lines_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basic_lines_ebo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Complex triangles.
    glGenBuffers(1, &complex_triangles_vbo);
    glGenVertexArrays(1, &complex_triangles_vao);
    glGenBuffers(1, &complex_triangles_ebo);

    glBindVertexArray(complex_triangles_vao);
    glBindBuffer(GL_ARRAY_BUFFER, complex_triangles_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, complex_triangles_ebo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                          (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

GLuint sorg::SetupShaderProgram(const char *vertexPath,
                                const char *fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;

    ReadShaderFiles(vertexPath, fragmentPath, vertexCode, fragmentCode);

    GLuint vertexShader;
    GLuint fragmentShader;
    CreateAndCompileShaders(vertexCode.c_str(), fragmentCode.c_str(),
                            vertexShader, fragmentShader);

    // Create program.
    GLuint id = glCreateProgram();

    AttachAndLinkShaders(id, vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return id;
}

void sorg::ReadShaderFiles(const char *vertexPath, const char *fragmentPath,
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

void sorg::CreateAndCompileShaders(const char *vertexCode,
                                   const char *fragmentCode,
                                   GLuint &vertexShader, GLuint &fragmentShader)
{
    // Compile shaders.
    int  success;
    char infoLog[512];

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
    int  success;
    char infoLog[512];

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

void sorg::SetupBasicShader(const char *vertexPath, const char *fragmentPath)
{
    basic_shader = SetupShaderProgram(vertexPath, fragmentPath);
}

void sorg::SetupComplexShader(const char *vertexPath, const char *fragmentPath)
{
    complex_shader = SetupShaderProgram(vertexPath, fragmentPath);
}

void sorg::SetupLines(std::span<const float>        vertices,
                      std::span<const unsigned int> indices, unsigned counter)
{
    lines_counter = counter;

    glBindVertexArray(basic_lines_vao);

    glBindBuffer(GL_ARRAY_BUFFER, basic_lines_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basic_lines_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void sorg::SetupTriangles(std::span<const float>        vertices,
                          std::span<const unsigned int> indices,
                          unsigned                      counter)
{
    complex_triangles_counter = counter;

    glBindVertexArray(complex_triangles_vao);

    glBindBuffer(GL_ARRAY_BUFFER, complex_triangles_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, complex_triangles_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void sorg::Draw(float current_time)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(complex_shader);
    glUniform1f(glGetUniformLocation(complex_shader, "uTime"), current_time);
    // complex_shader.use();
    // complex_shader.SetFloat("uTime", current_time);

    glBindVertexArray(complex_triangles_vao);
    glDrawElements(GL_TRIANGLES, complex_triangles_counter, GL_UNSIGNED_INT, 0);

    glUseProgram(basic_shader);
    // basic_shader.use();
    glBindVertexArray(basic_lines_vao);
    glDrawElements(GL_LINES, lines_counter, GL_UNSIGNED_INT, 0);
}
