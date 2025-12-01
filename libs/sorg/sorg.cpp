#include <glad/glad.h>

#include "sorg.h"
#include "shader.h"

// TODO: Fix these objects.
unsigned basic_lines_vao = 0;
unsigned basic_lines_vbo = 0;
unsigned basic_lines_ebo = 0;
Shader   basic_shader;
unsigned lines_counter = 0;

unsigned complex_triangles_vao = 0;
unsigned complex_triangles_vbo = 0;
unsigned complex_triangles_ebo = 0;
Shader   complex_shader;
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

void sorg::SetupBasicShader(const char *vertexPath, const char *fragmentPath)
{
    basic_shader = Shader(vertexPath, fragmentPath);
}

void sorg::SetupComplexShader(const char *vertexPath, const char *fragmentPath)
{
    complex_shader = Shader(vertexPath, fragmentPath);
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

    complex_shader.use();
    complex_shader.SetFloat("uTime", current_time);

    glBindVertexArray(complex_triangles_vao);
    glDrawElements(GL_TRIANGLES, complex_triangles_counter, GL_UNSIGNED_INT, 0);

    basic_shader.use();
    glBindVertexArray(basic_lines_vao);
    glDrawElements(GL_LINES, lines_counter, GL_UNSIGNED_INT, 0);
}
