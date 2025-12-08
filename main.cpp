#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "sorg/sorg.h"
#include "src/level.h"

// TODO: UNDERSTAND THE X SHADER! target_nohit.fs
bool Initialise(const std::string &name, const int width, const int height);

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

void ParseLevelData(const char *path, LevelData &level_data);
void ParseLaserData(const std::vector<std::string> &laser_lines,
                    LaserData &laser_data, const char *path);

constexpr std::string kName               = "L4S3R";
constexpr int         kNumberOfLaserLines = 4;
unsigned              viewport_width      = 600;
unsigned              viewport_height     = 900;

GLFWwindow *window;

std::vector<Level> levels;
int                current_level_id = 0;

int main()
{
    bool success = Initialise(kName, viewport_width, viewport_height);
    if (!success)
    {
        std::cout << "[L4S3R] Failed to initialise GLFW & GLAD." << std::endl;
        return -1;
    }

    LevelData new_level_data;
    ParseLevelData("levels/1.txt", new_level_data);

    levels.push_back(Level(new_level_data));
    levels[current_level_id].SetViewportDimensions(viewport_width,
                                                   viewport_height, false);
    levels[current_level_id].Initialise();
    while (!glfwWindowShouldClose(window))
    {
        levels[current_level_id].Update(glfwGetTime());

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

    glEnable(GL_DEPTH_TEST);

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
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        levels[current_level_id].OnLeftClicked(x, y);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        levels[current_level_id].OnRightClicked(x, y);
    }
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    viewport_width  = width;
    viewport_height = height;
    glViewport(0, 0, width, height);

    levels[current_level_id].SetViewportDimensions(viewport_width,
                                                   viewport_height, true);
}

void ParseLevelData(const char *path, LevelData &data)
{
    std::fstream in;
    in.open(path);
    if (!in.is_open())
    {
        std::cout << "[L4S3R] ParseLevelData: Error: File did not open!\n"
                  << path << std::endl;
        return;
    }

    bool                     parsing_board = true;
    std::string              current_input_line;
    std::vector<std::string> current_laser_lines;
    int                      row_counter = 0;

    while (getline(in, current_input_line))
    {
        if (parsing_board)
        {
            std::vector<int> current_board_line;
            if (current_input_line.empty())
            {
                parsing_board      = false;
                data.board_height_ = row_counter;
                continue;
            }

            for (const auto &c : current_input_line)
            {
                switch (c)
                {
                case '.':
                    current_board_line.push_back(0);
                    break;
                case 'x':
                    current_board_line.push_back(-2);
                    break;
                default:
                    break;
                }
            }

            data.board_state_.push_back(current_board_line);
            data.board_width_ = current_input_line.size();
            ++row_counter;
            continue;
        }

        // Parse lasers.
        if (current_input_line.empty())
        {
            LaserData new_laser;
            ParseLaserData(current_laser_lines, new_laser, path);

            data.lasers_.push_back(new_laser);
            current_laser_lines.clear();
            continue;
        }

        current_laser_lines.push_back(current_input_line);
    }

    LaserData last_laser;
    ParseLaserData(current_laser_lines, last_laser, path);

    data.lasers_.push_back(last_laser);
}

void ParseLaserData(const std::vector<std::string> &laser_lines,
                    LaserData &laser_data, const char *path)
{
    if (laser_lines.size() != kNumberOfLaserLines)
    {
        for (auto test : laser_lines)
        {
            std::cout << test << std::endl;
        }
        std::cout << "[L4S3R] ParseLevelData: Error: Laser segment "
                     "does not have "
                  << kNumberOfLaserLines << " lines!\n"
                  << path << std::endl;
        return;
    }

    std::vector<std::pair<int, int>> current_laser_pairs;
    sorg::Colour                     current_laser_colour;
    for (int i = 0; i < laser_lines.size(); ++i)
    {
        std::string        current_number_string;
        std::stringstream  stream{laser_lines[i]};
        std::vector<float> current_numbers;
        while (getline(stream, current_number_string, ','))
        {
            current_numbers.push_back(std::stoi(current_number_string));
        }

        if (current_numbers.size() != 2 && current_numbers.size() != 4)
        {
            std::cout << "[L4S3R]: ParseLevelData: Error: Laser line "
                         "does not have 2 (locations) nor 4 (colour) "
                         "numbers!\n"
                      << path << std::endl;
            return;
        }

        if (current_numbers.size() == 2)
        {
            current_laser_pairs.push_back(
                {current_numbers[0], current_numbers[1]});
        }

        if (current_numbers.size() == 4)
        {
            current_laser_colour = {current_numbers[0], current_numbers[1],
                                    current_numbers[2], current_numbers[3]};
        }
    }

    laser_data.source_cell_ = current_laser_pairs[0];
    laser_data.target_cell_ = current_laser_pairs[1];
    laser_data.direction_   = current_laser_pairs[2];
    laser_data.colour_      = current_laser_colour;
}
