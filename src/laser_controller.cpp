#include "laser_controller.h"

#include <iostream>

void LaserController::Initialise(Board *board_ptr)
{
    board_ptr_ = board_ptr;
    laser_shader_ =
        sorg::SetupShaderProgram("shaders/laser.vs", "shaders/laser.fs");
    laser_source_shader_ = sorg::SetupShaderProgram("shaders/laser_source.vs",
                                                    "shaders/laser_source.fs");

    AddLaserSource(lasers_.size() + index_offset, {-1, 1}, {1, 0},
                   {255.0f, 0.0f, 0.0f, 1.0f});
    AddLaserSource(lasers_.size() + index_offset, {3, -1}, {0, 1},
                   {0.0f, 255.0f, 0.0f, 1.0f});

    RefreshLaserSources();

    CalculateLasers();
}

void LaserController::Update(float current_time)
{
    sorg::SetUniform(laser_source_shader_, "uTime", current_time);
}

void LaserController::Refresh()
{
    RefreshLaserSources();
}

void LaserController::OnBoardLogicChanged()
{
    CalculateLasers();
}

void LaserController::AddLaserSource(const unsigned            id,
                                     const std::pair<int, int> cell,
                                     const std::pair<int, int> direction,
                                     const sorg::Colour       &colour)
{
    lasers_.push_back(Laser(id, cell, direction, colour));
}

void LaserController::RefreshLaserSources()
{
    if (!board_ptr_)
    {
        std::cout << "[L4S3R] LaserController: Error: Board is a null pointer!"
                  << std::endl;
        return;
    }

    const auto &board = *board_ptr_;

    std::vector<float>    laser_vertices;
    std::vector<unsigned> laser_indices;

    for (int i = 0; i < lasers_.size(); ++i)
    {
        Laser laser = lasers_[i];

        std::vector<std::pair<float, float>> positions2d;
        std::vector<std::pair<float, float>> uv = {
            {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};

        auto laser_colour = laser.kColour;

        std::vector<float> colour = {
            laser_colour.r / 255.0f, laser_colour.g / 255.0f,
            laser_colour.b / 255.0f, laser_colour.a / 255.0f};

        float top_left_x =
            board.GetOriginRelativeX() +
            laser.kSourceCell.first * board.GetHorizontalIncrement();
        float top_left_y =
            board.GetOriginRelativeY() -
            laser.kSourceCell.second * board.GetVerticalIncrement();

        float top_right_x    = top_left_x + board.GetHorizontalIncrement();
        float bottom_left_x  = top_left_x;
        float bottom_right_x = top_right_x;

        float top_right_y    = top_left_y;
        float bottom_left_y  = top_left_y - board.GetVerticalIncrement();
        float bottom_right_y = bottom_left_y;

        positions2d.push_back({top_left_x, top_left_y});
        positions2d.push_back({top_right_x, top_right_y});
        positions2d.push_back({bottom_left_x, bottom_left_y});
        positions2d.push_back({bottom_right_x, bottom_right_y});

        for (int j = 0; j < positions2d.size(); ++j)
        {
            // 3d positions, uvs, colours.
            laser_vertices.push_back(positions2d[j].first);
            laser_vertices.push_back(positions2d[j].second);
            laser_vertices.push_back(kLaserSourceDepth);
            laser_vertices.push_back(uv[j].first);
            laser_vertices.push_back(uv[j].second);
            laser_vertices.insert(std::end(laser_vertices), std::begin(colour),
                                  std::end(colour));

            // Indices, typical quad indices.
            laser_indices.push_back(0 + i * positions2d.size());
            laser_indices.push_back(1 + i * positions2d.size());
            laser_indices.push_back(2 + i * positions2d.size());
            laser_indices.push_back(1 + i * positions2d.size());
            laser_indices.push_back(3 + i * positions2d.size());
            laser_indices.push_back(2 + i * positions2d.size());
        }
    }

    sorg::SetupTriangles(laser_source_shader_, laser_vertices, laser_indices,
                         laser_indices.size());
}

void LaserController::CalculateLasers()
{
    if (!board_ptr_)
    {
        std::cout << "[L4S3R] LaserController: Error: Board is a null pointer!"
                  << std::endl;
        return;
    }

    auto &board = *board_ptr_;

    std::vector<float>    vertices;
    std::vector<unsigned> indices;

    unsigned total_number_of_vertices = 0;
    for (auto &laser : lasers_)
    {
        auto laser_colour = laser.kColour;

        std::vector<float> colour = {
            laser_colour.r / 255.0f, laser_colour.g / 255.0f,
            laser_colour.b / 255.0f, laser_colour.a / 255.0f};

        int first_x = laser.kSourceCell.first;
        int first_y = laser.kSourceCell.second;

        std::pair<int, int> current_direction = laser.kDirection;

        // Add first vertex.
        float first_horizontal = 0.0f;
        float first_vertical   = 0.0f;
        GetCellCentre(first_x, first_y, first_horizontal, first_vertical);

        vertices.push_back(first_horizontal);
        vertices.push_back(first_vertical);
        vertices.push_back(kLaserDepth);
        vertices.insert(std::end(vertices), std::begin(colour),
                        std::end(colour));

        int current_x = first_x;
        int current_y = first_y;
        int next_x    = 0;
        int next_y    = 0;

        int  number_of_lines = 0;
        bool laser_blocked   = false;
        while (!laser_blocked)
        {
            next_x = current_x + current_direction.first;
            next_y = current_y + current_direction.second;
            if (next_x < 0 || next_x >= board.GetWidth() || next_y < 0 ||
                next_y >= board.GetHeight())
            {
                laser_blocked = true;
                break;
            }

            int   next_cell_state = board.GetBoardState(next_x, next_y);
            float next_horizontal = 0.0f;
            float next_vertical   = 0.0f;
            switch (next_cell_state)
            {
            case 0:
                if (!infiltrating_lasers)
                {
                    board.SetBoardState(next_x, next_y, laser.kId);
                }
                break;
            case 1:
                current_direction = {current_direction.second,
                                     current_direction.first};
                GetCellCentre(next_x, next_y, next_horizontal, next_vertical);

                vertices.push_back(next_horizontal);
                vertices.push_back(next_vertical);
                vertices.push_back(kLaserDepth);
                vertices.insert(std::end(vertices), std::begin(colour),
                                std::end(colour));

                ++number_of_lines;
                break;
            case -1:
                current_direction = {-current_direction.second,
                                     -current_direction.first};
                GetCellCentre(next_x, next_y, next_horizontal, next_vertical);

                vertices.push_back(next_horizontal);
                vertices.push_back(next_vertical);
                vertices.push_back(kLaserDepth);
                vertices.insert(std::end(vertices), std::begin(colour),
                                std::end(colour));

                ++number_of_lines;
                break;
            default:
                laser_blocked = true;
                break;
            }

            current_x = next_x;
            current_y = next_y;
        }

        // There is always the last vertex;
        float next_horizontal = 0.0f;
        float next_vertical   = 0.0f;
        GetCellCentre(next_x, next_y, next_horizontal, next_vertical);

        vertices.push_back(next_horizontal);
        vertices.push_back(next_vertical);
        vertices.push_back(kLaserDepth);
        vertices.insert(std::end(vertices), std::begin(colour),
                        std::end(colour));

        ++number_of_lines;

        for (int i = 0; i < number_of_lines; ++i)
        {
            indices.push_back(i + total_number_of_vertices);
            indices.push_back((i + 1) + total_number_of_vertices);
        }

        total_number_of_vertices += number_of_lines + 1;
    }

    sorg::SetupLines(laser_shader_, vertices, indices, indices.size());
}

void LaserController::GetCellCentre(int x, int y, float &horizontal,
                                    float &vertical) const
{
    if (!board_ptr_)
    {
        std::cout << "[L4S3R] LaserController: Error: Board is a null pointer!"
                  << std::endl;
        return;
    }

    const auto &board = *board_ptr_;

    horizontal = board.GetOriginRelativeX() +
                 board.GetHorizontalIncrement() * (x + 0.5f);
    vertical =
        board.GetOriginRelativeY() - board.GetVerticalIncrement() * (y + 0.5f);
}
