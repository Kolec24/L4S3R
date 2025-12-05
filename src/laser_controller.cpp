#include "laser_controller.h"

#include <iostream>

void LaserController::Initialise(Board *board_ptr)
{
    board_ptr_ = board_ptr;

    laser_shader_ =
        sorg::SetupShaderProgram("shaders/laser.vs", "shaders/laser.fs");
    laser_source_shader_ = sorg::SetupShaderProgram("shaders/laser_source.vs",
                                                    "shaders/laser_source.fs");
    target_nohit_shader_ = sorg::SetupShaderProgram("shaders/target_nohit.vs",
                                                    "shaders/target_nohit.fs");
    target_hit_shader_   = sorg::SetupShaderProgram("shaders/target_hit.vs",
                                                    "shaders/target_hit.fs");

    AddLaserWithTarget(lasers_.size() + index_offset, {-1, 1}, {-1, 3}, {1, 0},
                       {255.0f, 0.0f, 0.0f, 1.0f});
    AddLaserWithTarget(lasers_.size() + index_offset, {3, -1}, {5, 2}, {0, 1},
                       {0.0f, 255.0f, 0.0f, 1.0f});

    Refresh();
}

void LaserController::Update(float current_time)
{
    sorg::SetUniform(laser_source_shader_, "uTime", current_time);
}

void LaserController::Refresh()
{
    RefreshLaserSources();
    RefreshTargets();
    RefreshLasers();
}

void LaserController::OnBoardLogicChanged()
{
    RefreshLasers();
}

bool LaserController::AreAllTargetsHit() const
{
    for (const auto &target : targets_)
    {
        if (!target.hit_)
        {
            return false;
        }
    }

    return true;
}

void LaserController::AddLaserWithTarget(const unsigned            id,
                                         const std::pair<int, int> source_cell,
                                         const std::pair<int, int> target_cell,
                                         const std::pair<int, int> direction,
                                         const sorg::Colour       &colour)
{
    lasers_.push_back(Laser(id, source_cell, direction, colour));
    targets_.push_back(Target(id, target_cell, colour));
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
        Laser laser  = lasers_[i];
        auto  colour = laser.GetNormalisedColour();

        std::vector<std::pair<float, float>> positions2d;
        std::vector<std::pair<float, float>> uv = {
            {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};

        float top_left_x =
            board.GetOriginRelativeX() +
            laser.kSourceCell.first * board.GetHorizontalIncrement();
        float top_left_y =
            board.GetOriginRelativeY() +
            laser.kSourceCell.second * board.GetVerticalIncrement();

        float top_right_x    = top_left_x + board.GetHorizontalIncrement();
        float bottom_left_x  = top_left_x;
        float bottom_right_x = top_right_x;

        float top_right_y    = top_left_y;
        float bottom_left_y  = top_left_y + board.GetVerticalIncrement();
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

void LaserController::RefreshTargets()
{
    if (!board_ptr_)
    {
        std::cout << "[L4S3R] LaserController: Error: Board is a null pointer!"
                  << std::endl;
        return;
    }

    const auto &board = *board_ptr_;

    std::vector<float>    nohit_vertices;
    std::vector<unsigned> nohit_indices;
    std::vector<float>    hit_vertices;
    std::vector<unsigned> hit_indices;

    int hit_number   = 0;
    int nohit_number = 0;
    for (int i = 0; i < targets_.size(); ++i)
    {
        Target      target = targets_[i];
        auto        colour = target.GetNormalisedColour();
        const auto &cell   = target.kCell;

        std::vector<std::pair<float, float>> positions2d;
        std::vector<std::pair<float, float>> uv = {
            {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};

        float top_left_x = board.GetOriginRelativeX() +
                           cell.first * board.GetHorizontalIncrement();
        float top_left_y = board.GetOriginRelativeY() +
                           cell.second * board.GetVerticalIncrement();

        float top_right_x    = top_left_x + board.GetHorizontalIncrement();
        float bottom_left_x  = top_left_x;
        float bottom_right_x = top_right_x;

        float top_right_y    = top_left_y;
        float bottom_left_y  = top_left_y + board.GetVerticalIncrement();
        float bottom_right_y = bottom_left_y;

        positions2d.push_back({top_left_x, top_left_y});
        positions2d.push_back({top_right_x, top_right_y});
        positions2d.push_back({bottom_left_x, bottom_left_y});
        positions2d.push_back({bottom_right_x, bottom_right_y});

        auto &vertices          = target.hit_ ? hit_vertices : nohit_vertices;
        auto &indices           = target.hit_ ? hit_indices : nohit_indices;
        auto &number_of_targets = target.hit_ ? hit_number : nohit_number;
        for (int j = 0; j < positions2d.size(); ++j)
        {
            // 3d positions, uvs, colours.
            vertices.push_back(positions2d[j].first);
            vertices.push_back(positions2d[j].second);
            vertices.push_back(kTargetDepth);
            vertices.push_back(uv[j].first);
            vertices.push_back(uv[j].second);
            vertices.insert(std::end(vertices), std::begin(colour),
                            std::end(colour));

            // Indices, typical quad indices.
            indices.push_back(0 + number_of_targets * positions2d.size());
            indices.push_back(1 + number_of_targets * positions2d.size());
            indices.push_back(2 + number_of_targets * positions2d.size());
            indices.push_back(1 + number_of_targets * positions2d.size());
            indices.push_back(3 + number_of_targets * positions2d.size());
            indices.push_back(2 + number_of_targets * positions2d.size());
        }

        ++number_of_targets;
    }

    sorg::SetupTriangles(target_nohit_shader_, nohit_vertices, nohit_indices,
                         nohit_indices.size());
    sorg::SetupTriangles(target_hit_shader_, hit_vertices, hit_indices,
                         hit_indices.size());
}

void LaserController::RefreshLasers()
{
    if (!board_ptr_)
    {
        std::cout << "[L4S3R] LaserController: Error: Board is a null pointer!"
                  << std::endl;
        return;
    }

    auto &board = *board_ptr_;

    ResetTargets();

    std::vector<float>    vertices;
    std::vector<unsigned> indices;

    unsigned total_number_of_vertices = 0;
    for (auto &laser : lasers_)
    {
        auto colour = laser.GetNormalisedColour();

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
        bool laser_out       = false;
        while (!laser_blocked && !laser_out)
        {
            next_x = current_x + current_direction.first;
            next_y = current_y + current_direction.second;
            if (next_x < 0 || next_x >= board.GetWidth() || next_y < 0 ||
                next_y >= board.GetHeight())
            {
                laser_out = true;
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

        // Check targets - they are assumed to be outside the grid.
        if (laser_out)
        {
            for (auto &target : targets_)
            {
                if (target.kId != laser.kId)
                {
                    continue;
                }

                if (target.kCell.first == next_x &&
                    target.kCell.second == next_y)
                {
                    OnTargetHit(target, true);
                }
            }
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

    // Targets have to be updated after all laser calculation.
    RefreshTargets();

    sorg::SetupLines(laser_shader_, vertices, indices, indices.size());
}

void LaserController::ResetTargets()
{
    for (auto &target : targets_)
    {
        OnTargetHit(target, false);
    }
}

void LaserController::OnTargetHit(Target &target, bool hit)
{
    target.hit_ = hit;
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
        board.GetOriginRelativeY() + board.GetVerticalIncrement() * (y + 0.5f);
}
