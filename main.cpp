#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "sorg/sorg.h"
#include "src/level.h"

bool Initialise(const std::string &name, const int width, const int height);

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

constexpr std::string kName           = "L4S3R";
unsigned              viewport_width  = 600;
unsigned              viewport_height = 900;

GLFWwindow *window;

Level current_level;

int main()
{
    bool success = Initialise(kName, viewport_width, viewport_height);
    if (!success)
    {
        std::cout << "[L4S3R] Failed to initialise GLFW & GLAD." << std::endl;
        return -1;
    }

    current_level.Initialise();

    while (!glfwWindowShouldClose(window))
    {
        current_level.Update(glfwGetTime());

        sorg::Draw(glfwGetTime());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

bool Initialise(const std::string &name, const int width, const int height)
{
    // GLFW Initialisation - we force version 4.6.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    // Window - before GLAD initialisation, as GLAD requires a context to load.
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == nullptr)
    {
        std::cout << "[SORG] Failed to create window." << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // GLAD - initialisation before any OpenGL function, as it manages funcion
    // pointers for OpenGL.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "[SORG] Failed to initialise GLAD." << std::endl;
        return false;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    current_level.SetViewportDimensions(width, height, false);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    return true;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        current_level.OnLeftClicked(x, y);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        current_level.OnRightClicked(x, y);
    }
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    viewport_width  = width;
    viewport_height = height;
    glViewport(0, 0, width, height);

    current_level.SetViewportDimensions(viewport_width, viewport_height, true);
}
