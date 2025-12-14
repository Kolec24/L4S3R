#include "game.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "depths.h"

float    Game::current_time_    = 0.0f;
unsigned Game::viewport_width_  = 0;
unsigned Game::viewport_height_ = 0;

unsigned Game::GetViewportWidth()
{
    return viewport_width_;
}

unsigned Game::GetViewportHeight()
{
    return viewport_height_;
}

float Game::GetCurrentTime()
{
    return current_time_;
}

float Game::GetRelativeTopOffset()
{
    return kRelativeTopOffset;
}

float Game::GetRelativeBottomOffset()
{
    return kRelativeBottomOffset;
}

float Game::GetRelativeLeftOffset()
{
    return kRelativeLeftOffset;
}

float Game::GetRelativeRightOffset()
{
    return kRelativeRightOffset;
}

void Game::Initialise(int viewport_width, int viewport_height,
                      int start_level_id)
{
    SetGameViewport(viewport_width, viewport_height, false);
    SetupOverlayShaders();

    LoadLevelsFromFiles();
    LoadLevel(start_level_id);
}

void Game::Update(float current_time)
{
    current_time_ = current_time;

    levels_[current_level_id_].Update();

    if (transition_in_progress_)
    {
        UpdateTransition();
        return;
    }

    if (levels_[current_level_id_].IsFinished())
    {
        if (kUseTransitions)
        {
            StartTransitionToNextLevel();
        }
        else
        {
            LoadNextLevel();
        }
    }
}

void Game::SetGameViewport(int viewport_width, int viewport_height,
                           bool refresh)
{
    viewport_width_  = viewport_width;
    viewport_height_ = viewport_height;

    if (refresh)
    {
        levels_[current_level_id_].Refresh();
    }
}

void Game::OnLeftMouseButtonClicked(float x, float y)
{
    if (transition_in_progress_)
    {
        return;
    }

    if (levels_[current_level_id_].IsFinished())
    {
        return;
    }

    levels_[current_level_id_].OnLeftClicked(x, y);
}

void Game::OnRightMouseButtonClicked(float x, float y)
{
    if (transition_in_progress_)
    {
        return;
    }

    if (levels_[current_level_id_].IsFinished())
    {
        return;
    }

    levels_[current_level_id_].OnRightClicked(x, y);
}

void Game::DebugLoadLevel(int level_id)
{
    LoadLevel(level_id - 1);
}

void Game::LoadLevelsFromFiles()
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

        levels_.push_back(Level(new_level_data));
    }
}

bool Game::ParseLevelData(const char *path, LevelData &data)
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

void Game::ParseLaserData(const std::vector<std::string> &laser_lines,
                          LaserData &laser_data, const char *path)
{
    if (laser_lines.size() != kNumberOfLaserLines)
    {
        std::cout << "[L4S3R] Game::ParseLevelData: Error: Laser segment "
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
            std::cout << "[L4S3R]: Game::ParseLevelData: Error: Laser line "
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

bool Game::LoadNextLevel()
{
    return LoadLevel(current_level_id_ + 1);
}

bool Game::LoadLevel(int level_id)
{
    if (level_id == current_level_id_)
    {
        return false;
    }

    // Reset current level here, since we might not have another one.
    if (current_level_id_ >= 0)
    {
        levels_[current_level_id_].Reset();
    }

    if (levels_.size() <= level_id)
    {
        return false;
    }

    current_level_id_ = level_id;
    levels_[current_level_id_].Initialise();

    return true;
}

void Game::StartTransitionToNextLevel()
{
    StartTransitionToLevel(current_level_id_ + 1);
}

void Game::StartTransitionToLevel(int level_id)
{
    transition_in_progress_    = true;
    level_id_to_transition_to_ = level_id;
    transition_start_time_     = GetCurrentTime();
}

void Game::UpdateTransition()
{
    float time_difference = GetCurrentTime() - transition_start_time_;
    if (time_difference <= kTransitionDelay)
    {
        return;
    }

    time_difference -= kTransitionDelay;
    if (time_difference > kTransitionHalfTime)
    {
        if (current_level_id_ != level_id_to_transition_to_)
        {
            bool level_changed = LoadLevel(level_id_to_transition_to_);
            if (!level_changed)
            {
                FinishTransition();
            }
        }
    }

    if (time_difference > 2 * kTransitionHalfTime)
    {
        FinishTransition();
        return;
    }

    float value = time_difference / kTransitionHalfTime;
    sorg::SetUniform(level_transition_shader_, "uVerticalLimit", value);
}

void Game::FinishTransition()
{
    transition_in_progress_ = false;
}

void Game::SetupOverlayShaders()
{
    // Shader programs.
    top_shader_ =
        sorg::SetupShaderProgram("shaders/white.vs", "shaders/white.fs");
    bottom_shader_ =
        sorg::SetupShaderProgram("shaders/white.vs", "shaders/white.fs");
    level_transition_shader_ = sorg::SetupShaderProgram(
        "shaders/transition.vs", "shaders/transition.fs");

    // Typical quad indices.
    std::vector<unsigned> indices = {0, 1, 2, 1, 3, 2};

    // Vertices.
    std::vector<float> top_vertices;
    std::vector<float> bottom_vertices;
    std::vector<float> level_vertices;

    float level_top    = 1.0f - GetRelativeTopOffset() * 2.0f;
    float level_bottom = -1.0f + GetRelativeBottomOffset() * 2.0f;
    float level_left   = -1.0f + GetRelativeLeftOffset() * 2.0f;
    float level_right  = 1.0f - GetRelativeRightOffset() * 2.0f;

    // Top.
    top_vertices.push_back(level_left);
    top_vertices.push_back(1.0f);
    top_vertices.push_back(depth::kOverlayDepth);
    top_vertices.push_back(0.0f);
    top_vertices.push_back(0.0f);

    top_vertices.push_back(level_right);
    top_vertices.push_back(1.0f);
    top_vertices.push_back(depth::kOverlayDepth);
    top_vertices.push_back(1.0f);
    top_vertices.push_back(0.0f);

    top_vertices.push_back(level_left);
    top_vertices.push_back(level_top);
    top_vertices.push_back(depth::kOverlayDepth);
    top_vertices.push_back(0.0f);
    top_vertices.push_back(1.0f);

    top_vertices.push_back(level_right);
    top_vertices.push_back(level_top);
    top_vertices.push_back(depth::kOverlayDepth);
    top_vertices.push_back(1.0f);
    top_vertices.push_back(1.0f);

    // Bottom.
    bottom_vertices.push_back(level_left);
    bottom_vertices.push_back(level_bottom);
    bottom_vertices.push_back(depth::kOverlayDepth);
    bottom_vertices.push_back(0.0f);
    bottom_vertices.push_back(0.0f);

    bottom_vertices.push_back(level_right);
    bottom_vertices.push_back(level_bottom);
    bottom_vertices.push_back(depth::kOverlayDepth);
    bottom_vertices.push_back(1.0f);
    bottom_vertices.push_back(0.0f);

    bottom_vertices.push_back(level_left);
    bottom_vertices.push_back(-1.0f);
    bottom_vertices.push_back(depth::kOverlayDepth);
    bottom_vertices.push_back(0.0f);
    bottom_vertices.push_back(1.0f);

    bottom_vertices.push_back(level_right);
    bottom_vertices.push_back(-1.0f);
    bottom_vertices.push_back(depth::kOverlayDepth);
    bottom_vertices.push_back(1.0f);
    bottom_vertices.push_back(1.0f);

    // Transition.
    level_vertices.push_back(level_left);
    level_vertices.push_back(level_top);
    level_vertices.push_back(depth::kOverlayDepth);
    level_vertices.push_back(0.0f);
    level_vertices.push_back(0.0f);

    level_vertices.push_back(level_right);
    level_vertices.push_back(level_top);
    level_vertices.push_back(depth::kOverlayDepth);
    level_vertices.push_back(1.0f);
    level_vertices.push_back(0.0f);

    level_vertices.push_back(level_left);
    level_vertices.push_back(level_bottom);
    level_vertices.push_back(depth::kOverlayDepth);
    level_vertices.push_back(0.0f);
    level_vertices.push_back(1.0f);

    level_vertices.push_back(level_right);
    level_vertices.push_back(level_bottom);
    level_vertices.push_back(depth::kOverlayDepth);
    level_vertices.push_back(1.0f);
    level_vertices.push_back(1.0f);

    // Render setup.
    sorg::SetupTriangles(top_shader_, top_vertices, indices, indices.size());
    sorg::SetupTriangles(bottom_shader_, bottom_vertices, indices,
                         indices.size());
    sorg::SetupTriangles(level_transition_shader_, level_vertices, indices,
                         indices.size());

    sorg::SetUniform(level_transition_shader_, "uVerticalLimit", -1.0f);
}
