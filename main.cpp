#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "sorg/sorg.h"
#include "src/game.h"

// TODO: UNDERSTAND THE X SHADER! target_nohit.fs
bool Initialise(const std::string &name, const int width, const int height);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

constexpr std::string kName                = "L4S3R";
constexpr unsigned    kStartViewportWidth  = 600;
constexpr unsigned    kStartViewportHeight = 900;
constexpr unsigned    kStartLevelId        = 0;

GLFWwindow *window;
Game        game;

int main()
{
    bool success = Initialise(kName, kStartViewportWidth, kStartViewportHeight);
    if (!success)
    {
        std::cout << "[L4S3R] Failed to initialise GLFW & GLAD." << std::endl;
        return -1;
    }

    game.Initialise(kStartViewportWidth, kStartViewportHeight, kStartLevelId);

    while (!glfwWindowShouldClose(window))
    {
        game.Update(glfwGetTime());
        sorg::Draw();

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

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

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

    // Debug
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        game.DebugLoadLevel(1);
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        game.DebugLoadLevel(2);
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        game.DebugLoadLevel(3);
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        game.DebugLoadLevel(4);
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        game.DebugLoadLevel(5);
    }
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        game.OnLeftMouseButtonClicked(x, y);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        game.OnRightMouseButtonClicked(x, y);
    }
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);

    game.SetGameViewport(width, height, true);
}
