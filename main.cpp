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

void LoadLevelsFromFiles();
bool ParseLevelData(const char *path, LevelData &level_data);
void ParseLaserData(const std::vector<std::string> &laser_lines,
                    LaserData &laser_data, const char *path);
void InitialiseCurrentLevel();

void LoadNextLevel();
void LoadLevel(int level_id);
void DebugLoadLevel(int level_id);

constexpr std::string kName               = "L4S3R";
constexpr std::string kLevelsRoot         = "levels/";
constexpr std::string kLevelExtension     = ".txt";
constexpr int         kMaxNumberOfLevels  = 10;
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

    LoadLevelsFromFiles();
    InitialiseCurrentLevel();

    while (!glfwWindowShouldClose(window))
    {
        if (levels[current_level_id].IsFinished())
        {
            LoadNextLevel();
        }

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

    // Debug
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        DebugLoadLevel(1);
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        DebugLoadLevel(2);
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        DebugLoadLevel(3);
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        DebugLoadLevel(4);
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        DebugLoadLevel(5);
    }
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (levels[current_level_id].IsFinished())
    {
        return;
    }

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

void LoadLevelsFromFiles()
{
    for (int i = 0; i < kMaxNumberOfLevels; ++i)
    {
        LevelData   new_level_data;
        std::string level_file =
            kLevelsRoot + std::to_string(i + 1) + kLevelExtension;

        bool loaded = ParseLevelData(level_file.c_str(), new_level_data);
        if (!loaded)
        {
            break;
        }

        levels.push_back(Level(new_level_data));
    }
}

bool ParseLevelData(const char *path, LevelData &data)
{
    std::fstream in;
    in.open(path);
    if (!in.is_open())
    {
        return false;
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
                    current_board_line.push_back(-1);
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

    return true;
}

void ParseLaserData(const std::vector<std::string> &laser_lines,
                    LaserData &laser_data, const char *path)
{
    if (laser_lines.size() != kNumberOfLaserLines)
    {
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

void InitialiseCurrentLevel()
{
    levels[current_level_id].SetViewportDimensions(viewport_width,
                                                   viewport_height, false);
    levels[current_level_id].Initialise();
}

void LoadNextLevel()
{
    LoadLevel(current_level_id + 1);
}

void LoadLevel(int level_id)
{
    if (level_id == current_level_id)
    {
        return;
    }

    if (levels.size() <= level_id)
    {
        return;
    }

    levels[current_level_id].Reset();
    current_level_id = level_id;

    InitialiseCurrentLevel();
}

void DebugLoadLevel(int level_id)
{
    LoadLevel(level_id - 1);
}
