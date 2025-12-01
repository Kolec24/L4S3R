#include <glad/glad.h>

#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCodeString;
    std::string fragmentCodeString;
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

        vertexCodeString = vertexStream.str();
        fragmentCodeString = fragmentStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "[SORG] Error: Shader: File reading failed!" << std::endl;
    }

    const char *vertexCode = vertexCodeString.c_str();
    const char *fragmentCode = fragmentCodeString.c_str();

    // Compile shaders.
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "[SORG] Error: Shader: Vertex compilation failed!\n"
                  << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "[SORG] Error: Shader: Fragment compilation failed!\n"
                  << infoLog << std::endl;
    }

    // Shader program.
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "[SORG] Error: Shader: Program linkink failed!\n"
                  << infoLog << std::endl;
    }

    // Shaders are linked into our program, so they are no longer necessary.
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(ID); }

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::Set4Float(const std::string &name, float value1, float value2,
                       float value3, float value4) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3,
                value4);
}
